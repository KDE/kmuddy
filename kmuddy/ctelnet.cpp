/***************************************************************************
                          ctelnet.cpp  -  telnet...
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pi Jun 14 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define CTELNET_CPP

#include "ctelnet.h"

#include "cactionmanager.h"
#include "cglobalsettings.h"
#include "cmccp.h"
#include "cprofilesettings.h"
#include "cmsp.h"

// needed for removeSession which is called in disconnect
#include "csessionmanager.h"

#ifdef HAVE_MXP
#include "cmxpmanager.h"
#endif

#include <KLocalizedString>
#include <kmessagebox.h>

#include <QTcpSocket>

#include <stdio.h>

using namespace std;

struct cTelnetPrivate {
  /** socket */
  QTcpSocket *socket;
  QString hostName;
  int hostPort;

  QString encoding;

  QStringDecoder *inCoder;
  QStringEncoder *outCoder;

  /** object that handles MCCP */
  cMCCP *MCCP;

  /** object that handles MSP */
  cMSP *MSP;
  bool usingmsp;

#ifdef HAVE_MXP
  bool usingmxp, mxpNegotiated;
  int mxpallow;
#endif
  
  //iac: last char was IAC
  //iac2: last char was DO, DONT, WILL or WONT
  //insb: we're in IAC SB, waiting for IAC SE
  string command;
  bool iac, iac2, insb;
  
  /** has any data arrived since last call to waitingForData() */
  bool newdata;
  /** are we connected or commecting? */
  bool _connected, _connecting;
  
  /** current state of options on our side and on server side */
  bool myOptionState[256], hisOptionState[256];
    /** whether we have announced WILL/WON'T for that option (if we have, we don't
  respond to DO/DON'T sent by the server -- see implementation and RFC 854
  for more information... */
  bool announcedState[256];
  /** whether the server has already announced his WILL/WON'T */
  bool heAnnouncedState[256];
  /** whether we have tried to enable this option */
  bool triedToEnable[256];
  /** amount of bytes sent up to now */
  int sentbytes;
  /** have we received the GA signal? */
  bool recvdGA;
  /** should we prepend newline after receving a GA */
  bool prependGANewLine;
  bool t_cmdEcho;
  bool t_lpmudstyle;
  bool _startupneg;
  /** current dimensions */
  int curX, curY;
  /** offline connection */
  bool offLineConnection;

  QString termType;
};

#define DEFAULT_ENCODING "ISO 8859-1"

cTelnet::cTelnet (int sess) : cActionBase ("telnet", sess)
{
  d = new cTelnetPrivate;

  d->socket = nullptr;
  d->termType = "KMuddy";
  
  d->inCoder = nullptr;
  d->outCoder = nullptr;

  d->iac = d->iac2 = d->insb = false;
  d->command = "";

  d->sentbytes = 0;
  d->offLineConnection=false;
  d->_connected = false;
  d->_connecting = false;
  d->curX = 0;
  d->curY = 0;
  d->_startupneg = true;
  d->encoding = DEFAULT_ENCODING;

  reset ();

  d->MCCP = new cMCCP (this);

  d->MSP = new cMSP (sess);
  d->usingmsp = false;

#ifdef HAVE_MXP
  d->usingmxp = false;
  d->mxpallow = 3;  //auto-detect
  d->mxpNegotiated = false;
#endif

  addEventHandler ("dimensions-changed", 20, PT_INT);
  addEventHandler ("settings-changed", 50, PT_NOTHING);
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cTelnet::~cTelnet()
{
  if (isConnected ())
    disconnect ();

  removeEventHandler ("dimensions-changed");
  removeEventHandler ("settings-changed");
  removeGlobalEventHandler ("global-settings-changed");

  delete d->socket;
  delete d->MCCP;
  delete d->MSP;

  delete d->inCoder;
  delete d->outCoder;

  delete d;
  d = nullptr;
}

void cTelnet::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
      //Command echo parameter passed onwards ot cTelnet
    setCommandEcho (gs->getBool ("command-echo"));

      // LPMUD style prompt handling too...
    setLPMudStyle (gs->getBool ("lpmud-style"));

      //MUD Sound Protocol
    QStringList sounddirs;
    for (int i = 0; i < gs->getInt ("snd-path-count"); ++i)
      sounddirs << gs->getString ("snd-path-" + QString::number(i));
    setMSPGlobalPaths (sounddirs);
    setMSPAllowed (gs->getBool ("msp-allow"));
    setDownloadAllowed (gs->getBool ("msp-allow-downloads"));

  }
  else if (event == "settings-changed") {
    cProfileSettings *sett = settings();
    if (!sett) return;
    // check if we need to update the codec
    QString enc = sett->getString ("encoding");
    if (enc != d->encoding) {
      d->encoding = enc;
      setupEncoding ();
    }
#ifdef HAVE_MXP
    // MXP settings
    setMXPAllowed (sett->getInt ("use-mxp"));
#endif
    // telnet negotiation on startup
    setNegotiateOnStartup (sett->getBool ("startup-negotiate"));
    setLPMudStyle (sett->getBool ("lpmud-style"));
  }
}

void cTelnet::eventIntHandler (QString event, int, int par1, int par2)
{
  if (event == "dimensions-changed") {
    windowSizeChanged (par1, par2);
  }
}

void cTelnet::socketFailed ()
{
  if (d->_connected) return;  // nothing if we already are connected
  QString err = d->socket->errorString();
  cActionManager::self()->invokeEvent ("connection-failed", sess(), err);
  d->_connected = false;
  d->_connecting = false;
  d->socket->deleteLater ();
  d->socket = nullptr;
}

/** establishes a new connection */
void cTelnet::connectIt (const QString &address, int port, cProfileSettings *sett)
{
  // close existing connection first (if any)
  if (isConnected ())
    disconnect ();

  // handle offline connection
  if (isOffLineConnection())
  {
    d->_connected = true;
    d->_connecting = false;
    invokeEvent ("message", sess(), i18n ("--- A connection has been established ---"));
    invokeEvent ("connected", sess());
    return;
  }

  // set up encoding
  d->encoding = sett ? sett->getString ("encoding") : DEFAULT_ENCODING;
  setupEncoding ();

  d->_connecting = true;
  cActionManager::self()->invokeEvent ("message", sess(), i18n ("Connecting..."));
  d->hostName = address;
  d->hostPort = port;
  // TODO add QSslSocket support
  d->socket = new QTcpSocket(this);
  d->socket->connectToHost(address, port);
  d->socket->setSocketOption (QAbstractSocket::KeepAliveOption, 1);
  setupSocketHandlers ();
}

void cTelnet::setupSocketHandlers ()
{
  if (!d->socket) return;
  connect (d->socket, &QTcpSocket::connected, this, &cTelnet::socketConnected);
  connect (d->socket, &QTcpSocket::errorOccurred, this, &cTelnet::socketFailed);
  connect (d->socket, &QTcpSocket::readyRead, this, &cTelnet::socketRead);
  connect (d->socket, &QTcpSocket::disconnected, this, &cTelnet::socketClosed);
  connect (d->socket, &QTcpSocket::hostFound, this, &cTelnet::socketHostFound);
}

void cTelnet::setupEncoding ()
{
  delete d->inCoder;
  delete d->outCoder;

  const char *enc = d->encoding.toLatin1().data();
  d->inCoder = new QStringDecoder(enc);
  d->outCoder = new QStringEncoder(enc);
  if (!d->inCoder->isValid()) {
    delete d->inCoder;
    d->inCoder = new QStringDecoder("ISO-8859-1");
  }
  if (!d->outCoder->isValid()) {
    delete d->outCoder;
    d->outCoder = new QStringEncoder("ISO-8859-1");
  }

}

void cTelnet::reset ()
{
  //prepare option variables
  for (int i = 0; i < 256; i++)
  {
    d->myOptionState[i] = false;
    d->hisOptionState[i] = false;
    d->announcedState[i] = false;
    d->heAnnouncedState[i] = false;
    d->triedToEnable[i] = false;
  }
  //reset telnet status
  d->iac = d->iac2 = d->insb = false;
  d->command = "";
  // reset these so that we report dimensions correctly
  d->curX = 0;
  d->curY = 0;
}

void cTelnet::socketConnected ()
{
  if (d->_connected) return;

  d->_connected = true;
  d->_connecting = false;

  //reset MCCP, MSP and byte counters
  d->MCCP->reset ();
  d->MSP->reset (d->hostName);
  
  reset ();

  //now we should be connected
  invokeEvent ("message", sess(), i18n ("--- A connection has been established ---"));
  cActionManager::self()->invokeEvent ("connected", sess());

#ifdef HAVE_MXP
  // MXP -must- be initialized AFTER the connected message, else it won't work
  setMXPAllowed (d->mxpallow);  //initialize MXP properly
#endif  
  d->sentbytes = 0;
  
  //negotiate some telnet options, if allowed
  if (d->_startupneg)
  {
    //NAWS (used to send info about window size)
    sendTelnetOption (TN_WILL, OPT_NAWS);
    //do not allow server to echo our text!
    sendTelnetOption (TN_DONT, OPT_ECHO);
  }
}

/** closes connection */
void cTelnet::disconnect ()
{
  if (!d->_connected) return;

  d->_connected = false;
  d->_connecting = false;

  reset ();

  if (isOffLineConnection())
    d->offLineConnection=false;

  if (d->socket) {
    d->socket->flush ();
    d->socket->close ();
  
    // schedule socket deletion
    d->socket->deleteLater ();
    d->socket = nullptr;
  }

  //alright - we're disconnected
  cActionManager::self()->invokeEvent ("disconnected", sess());

  // ------------------
  // TODO: the rest will not be here, but in their respective objects, hooked on the event
  
  //remove the session - should now be safe
  cSessionManager::self()->removeSession (sess(), true);
}

void cTelnet::setOffLineConnection (bool type)
{
  d->offLineConnection = type;
}

/** are we connected (offline connection is counted as connected) ? */
bool cTelnet::isConnected ()
{
  return (isOffLineConnection() || d->_connected);
}

void cTelnet::setMSPGlobalPaths (const QStringList &paths)
{
  if (d->MSP)
    d->MSP->setGlobalPaths (paths);
}

bool cTelnet::usingMSP ()
{
  return d->usingmsp;
}

void cTelnet::setMSPAllowed (bool allow)
{
  if (d->MSP)
    d->MSP->setMSPAllowed (allow);
}

void cTelnet::setDownloadAllowed (bool allow)
{
  if (d->MSP)
    d->MSP->setDownloadAllowed (allow);
}

void cTelnet::processSoundRequest (bool isSOUND, QString fName, int volume, int repeats,
    int priority, QString type, QString url)
{
  if (d->MSP)
    d->MSP->processRequest (isSOUND, fName, volume, repeats, priority, type, url);
}

#ifdef HAVE_MXP

bool cTelnet::usingMXP () {
  return d->usingmxp;
}

int cTelnet::MXPAllowed () {
  return d->mxpallow;
}

void cTelnet::setMXPAllowed (int allow)
{
  //update actual MXP usage...
  switch (allow) {
    case 1: //Never
    {
      callAction ("mxpmanager", "set-active", sess(), 0);
      //ask server to disable MXP
      sendTelnetOption (TN_DONT, OPT_MXP);
    }
    break;
    case 2: //If negotiated
    {
      callAction ("mxpmanager", "set-active", sess(), d->mxpNegotiated ? 1 : 0);
      callAction ("mxpmanager", "switch-open", sess());
    }
    break;
    case 3: //Auto-detect
    {
      callAction ("mxpmanager", "set-active", sess(), 1);
    }
    break;
    case 4: //Always
    {
      callAction ("mxpmanager", "set-active", sess(), 1);
      callAction ("mxpmanager", "switch-open", sess());
    }
    break;
  };
  d->mxpallow = allow;
}

#endif //HAVE_MXP

bool cTelnet::sendData (const QString &data)
{
  if (!(isConnected()))
    return false;
  // return true and dont send, since offline is used
  if (isOffLineConnection())
    return true;

  if (d->t_cmdEcho == true && d->t_lpmudstyle)
    d->prependGANewLine = false;

  QByteArray outdata = d->outCoder->encode(data);

  // IAC byte must be doubled
  int len = outdata.length();
  bool gotIAC = false;
  for (int i = 0; i < len; i++)
    if ((unsigned char) outdata[i] == TN_IAC) {
      gotIAC = true;
      break;
    }
  if (gotIAC) {
    QByteArray d;
    // double IACs
    for (int i = 0; i < len; i++)
    {
      d += outdata[i];
      if ((unsigned char) outdata[i] == TN_IAC)
        d += outdata[i];  //double IAC
    }
    outdata = d;
  }

  //data ready, send it
  return doSendData (outdata);
}

void cTelnet::waitingForData () {
  d->newdata = false;
}

bool cTelnet::newData () {
  return d->newdata;
}

bool cTelnet::doSendData (const QByteArray &data)
{
  if (!(isConnected()))
    return false;
  if (isOffLineConnection())
    return true;
  //write data to socket - it's so complicated because sometimes only a part of data
  //is accepted at a time
  int dataLength = data.length ();
  const char *dd = data.data();
  int written = 0;
  do {
    int w = d->socket->write (dd + written, dataLength - written);
    // TODO: need some error diagnostics
    if (w == -1)  // buffer full - try again
      continue;
    written += w;
  } while (written < dataLength);

  //update counter
  d->sentbytes += dataLength;
  return true;
}

void cTelnet::windowSizeChanged (int x, int y)
{
  //remember the size - we'll need it if NAWS is currently disabled but will
  //be enabled. Also remember it if no connection exists at the moment;
  //we won't be called again when connecting
  if (!(isConnected()))
    return;
  if (!d->myOptionState[OPT_NAWS]) return;   //only if we have negotiated this option
  if ((x == d->curX) && (y == d->curY)) return;   // don't spam sizes if we have sent the current one already

  QByteArray s;
  s += TN_IAC;
  s += TN_SB;
  s += OPT_NAWS;
  unsigned char x1, x2, y1, y2;
  x1 = (unsigned char) x / 256;
  x2 = (unsigned char) x % 256;
  y1 = (unsigned char) y / 256;
  y2 = (unsigned char) y % 256;
  //IAC must be doubled
  s += x1;
  if (x1 == TN_IAC)
    s += TN_IAC;
  s += x2; 
  if (x2 == TN_IAC)
    s += TN_IAC;
  s += y1;
  if (y1 == TN_IAC)
    s += TN_IAC;
  s += y2;
  if (y2 == TN_IAC)
    s += TN_IAC;
  
  s += TN_IAC;
  s += TN_SE;
  doSendData (s);
}

void cTelnet::sendTelnetOption (unsigned char type, unsigned char option)
{
  QByteArray s;
  s += TN_IAC;
  s += (unsigned char) type;
  s += (unsigned char) option;
  doSendData (s);
}

// TODO: sort out this mess, allow custom protocol handlers, etc.
void cTelnet::processTelnetCommand (const string &command)
{
  unsigned char ch = command[1];
  unsigned char option;
  switch (ch) {
    case TN_AYT:
      doSendData ("I'm here! Please be more patient!\r\n");
          //well, this should never be executed, as the response would probably
          //be treated as a command. But that's server's problem, not ours...
          //If the server wasn't capable of handling this, it wouldn't have
          //sent us the AYT command, would it? Impatient server = bad server.
          //Let it suffer! ;-)
      break;
    case TN_GA:
      d->recvdGA = true;
      //signal will be emitted later
      break;
    case TN_WILL:
      //server wants to enable some option (or he sends a timing-mark)...
      option = command[2];

      d->heAnnouncedState[option] = true;
      if (d->triedToEnable[option])
      {
        d->hisOptionState[option] = true;
        d->triedToEnable[option] = false;
      }
      else
      {
        if (!d->hisOptionState[option])
            //only if this is not set; if it's set, something's wrong wth the server
            //(according to telnet specification, option announcement may not be
            //unless explicitly requested)
        {
          if ((option == OPT_SUPPRESS_GA) || (option == OPT_STATUS) ||
              (option == OPT_TERMINAL_TYPE) || (option == OPT_NAWS))
                 //these options are supported; compression is handled
                 //separately
          {
            sendTelnetOption (TN_DO, option);
            d->hisOptionState[option] = true;
          }
#ifdef HAVE_MXP
          else
          if (option == OPT_MXP)
          {
            // allow or disallow, MXP depending on whether it's disabled
            sendTelnetOption ((d->mxpallow >= 2) ? TN_DO : TN_DONT, option);
            d->hisOptionState[option] = true;

            //MXP is now negotiated
            d->mxpNegotiated = true;
            if (d->mxpallow == 2)  //MXP: if negotiated
            {
              cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
              mm->setMXPActive (true);
              puts ("KMuddy: MXP enabled !");
            }
          }
#endif
          else
          if (option == OPT_MSP)
          {
            sendTelnetOption (TN_DO, option);
            d->hisOptionState[option] = true;

            //MSP is now enabled
            d->usingmsp = true;
            d->MSP->enableMSP ();
            puts ("KMuddy: MSP enabled !");
          }
          else
          if ((option == OPT_COMPRESS) || (option == OPT_COMPRESS2))
          //these are handled separately, as they're a bit special
          {
            if ((option == OPT_COMPRESS) && (d->hisOptionState[OPT_COMPRESS2]))
            {
              //protocol says: reject MCCP v1 if you have previously accepted
              //MCCP v2...
              sendTelnetOption (TN_DONT, option);
              d->hisOptionState[option] = false;
              puts ("KMuddy: Rejecting MCCP v1, because v2 is already used !");
            }
            else
            {
              sendTelnetOption (TN_DO, option);
              d->hisOptionState[option] = true;
              //inform MCCP object about the change
              if ((option == OPT_COMPRESS)) {
                d->MCCP->setMCCP1 (true);
                puts ("KMuddy: MCCP v1 enabled !");
              }
              else {
                d->MCCP->setMCCP2 (true);
                puts ("KMuddy: MCCP v2 enabled !");
              }
            }
          }
          else
          {
            sendTelnetOption (TN_DONT, option);
            d->hisOptionState[option] = false;
          }
        }
      }
      break;
    case TN_WONT:
      //server refuses to enable some option...
      option = command[2];
      if (d->triedToEnable[option])
      {
        d->hisOptionState[option] = false;
        d->triedToEnable[option] = false;
        d->heAnnouncedState[option] = true;
      }
      else
      {
        //send DONT if needed (see RFC 854 for details)
        if (d->hisOptionState[option] || (!d->heAnnouncedState[option]))
        {
          sendTelnetOption (TN_DONT, option);
          d->hisOptionState[option] = false;
          //inform MCCP object about the change - won't cause problems in
          //cMCCP - see cmccp.cpp for more info
          if ((option == OPT_COMPRESS)) {
            d->MCCP->setMCCP1 (false);
            puts ("KMuddy: MCCP v1 disabled !");
          }
          if ((option == OPT_COMPRESS2)) {
            d->MCCP->setMCCP2 (false);
            puts ("KMuddy: MCCP v1 disabled !");
          }
        }
        d->heAnnouncedState[option] = true;
      }
      if (option == OPT_MSP)
      {
        //MSP is now disabled
        d->usingmsp = false;
        d->MSP->disableMSP ();
        puts ("KMuddy: MSP disabled !");
      }
#ifdef HAVE_MXP
      if (option == OPT_MXP)
      {
        //MXP is now disabled
        cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
        mm->setMXPActive (false);
        puts ("KMuddy: MXP disabled !");
      }
#endif
      break;
    case TN_DO:
      //server wants us to enable some option
      option = command[2];
      if (option == OPT_TIMING_MARK)
      {
        //send WILL TIMING_MARK
        sendTelnetOption (TN_WILL, option);
      }
#ifdef HAVE_MXP
      else
      if (option == OPT_MXP)
      {
        // allow or disallow, MXP depending on whether it's disabled
        sendTelnetOption ((d->mxpallow >= 2) ? TN_WILL : TN_WONT, option);
        d->hisOptionState[option] = true;

        //MXP is now negotiated
        d->mxpNegotiated = true;
        if (d->mxpallow == 2)  //MXP: if negotiated
        {
          cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
          mm->setMXPActive (true);
          puts ("KMuddy: MXP enabled !");
        }
      }
#endif
 
      else if (!d->myOptionState[option])
      //only if the option is currently disabled
      {
        if ((option == OPT_SUPPRESS_GA) || (option == OPT_STATUS) ||
            (option == OPT_TERMINAL_TYPE) || (option == OPT_NAWS))
        {
          sendTelnetOption (TN_WILL, option);
          d->myOptionState[option] = true;
          d->announcedState[option] = true;
        }
        else
        {
          sendTelnetOption (TN_WONT, option);
          d->myOptionState[option] = false;
          d->announcedState[option] = true;
        }
      }
      if (option == OPT_NAWS)  //NAWS here - window size info must be sent
        windowSizeChanged (d->curX, d->curY);
      break;
    case TN_DONT:
      //only respond if value changed or if this option has not been announced yet
      option = command[2];
#ifdef HAVE_MXP
      if (option == OPT_MXP)
      {
        //MXP is now disabled
        cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
        mm->setMXPActive (false);
        puts ("KMuddy: MXP disabled !");
      }
#endif
      if (d->myOptionState[option] || (!d->announcedState[option]))
      {
        sendTelnetOption (TN_WONT, option);
        d->announcedState[option] = true;
      }
      d->myOptionState[option] = false;
      break;
    case TN_SB:
      //subcommand - we analyze and respond...
      option = command[2];
      switch (option) {
        case OPT_STATUS:
          //see OPT_TERMINAL_TYPE for explanation why I'm doing this
          if (true /*myOptionState[OPT_STATUS]*/)
          {
            if (command[3] == TNSB_SEND)
            //request to send all enabled commands; if server sends his
            //own list of commands, we just ignore it (well, he shouldn't
            //send anything, as we do not request anything, but there are
            //so many servers out there, that you can never be sure...)
            {
              QByteArray s;
              s += TN_IAC;
              s += TN_SB;
              s += OPT_STATUS;
              s += TNSB_IS;
              for (int i = 0; i < 256; i++)
              {
                if (d->myOptionState[i])
                {
                  s += TN_WILL;
                  s += (unsigned char) i;
                }
                if (d->hisOptionState[i])
                {
                  s += TN_DO;
                  s += (unsigned char) i;
                }
              }
              s += TN_IAC;
              s += TN_SE;
              doSendData (s);
            }
          }
        break;
        case OPT_TERMINAL_TYPE:
          if (d->myOptionState[OPT_TERMINAL_TYPE])
          {
            if (command[3] == TNSB_SEND)
              //server wants us to send terminal type; he can send his own type
              //too, but we just ignore it, as we have no use for it...
            {
              QByteArray s;
              s += TN_IAC;
              s += TN_SB;
              s += OPT_TERMINAL_TYPE;
              s += TNSB_IS;
              s += d->termType.toLatin1().data();
              s += TN_IAC;
              s += TN_SE;
              doSendData (s);
            }
          }
        break;
        //other cmds should not arrive, as they were not negotiated.
        //if they do, they are merely ignored
      };
      break;
    //other commands are simply ignored (NOP and such, see .h file for list)
  };
}

void cTelnet::socketRead ()
{
  /*
  This function makes heavy use of char* instead of relying on QString,
  because otherwise there are problems with locale-based data and/or
  telnet commands.
  */

  // Looks like we often get this before getting the connected event ...
  // TODO: is this still true with the Qt4/KDE4 stuff ?
  if (d->_connecting && (!d->_connected))
    socketConnected();

  char buffer[32769];  //we should never receive such a big amount of data,
        //but we want to be sure that we read all we can...

  char data[32769]; //clean data after decompression

  int amount = d->socket->read (buffer, 32768);
  if (amount == -1)
    return;   //something is wrong (no data?)
  if (amount == 0)
    return;   //0 means socket has been closed; maybe I'll do something more?
  buffer[amount] = '\0';    //just to be sure - mark end of string

  invokeEvent ("raw-data-comp", sess(), QString (buffer));

  //we'll need cProfileSettings later on
  cProfileSettings *sett = settings();

  //MCCP will be handled first. This is done in a separate class, which
  //detects all MCCP-related codes itself. I know that this involves some
  //code duplication (two telnet option parsers), but it's needed because
  //telnet sequences may (and will) appear inside the compressed streams,
  //so it's best to get plain uncompressed data first. Another reason is
  //improper design of MCCP v1, which uses unterminated telnet subsequences.
  //This has been fixed in MCCP v2, but some MUDs may still use the old
  //version.

  d->MCCP->prepareDecompression (buffer, data, amount, 32768);
  int datalen;
  while ((datalen = d->MCCP->uncompressNext ()) != -1)
  {
    //inform plug-ins about uncompressed data (can be equal to raw data if MCCP isn't used
    invokeEvent ("raw-data", sess(), QString (data));

#if 0
printf ("DATA IN: ");
for (int i = 0; i < datalen; ++i) printf ("%02x ", (unsigned char) data[i]);
printf ("\n");
#endif

    data[datalen] = '\0';
    string cleandata;

    //we have some data...
    d->newdata = true;

    //clear the GO-AHEAD flag
    d->recvdGA = false;
    
    //now we have the data, but we cannot forward it to next stage of processing,
    //because the data contains telnet commands
    //so we parse the text and process all telnet commands:

    for (unsigned int i = 0; i < (unsigned int) datalen; i++)
    {
      unsigned char ch = data[i];
      if (d->iac || d->iac2 || d->insb || (ch == TN_IAC))
      {
        //there are many possibilities here:
        //1. this is IAC, previous character was regular data
        if (! (d->iac || d->iac2 || d->insb) && (ch == TN_IAC))
        {
          d->iac = true;
          d->command += ch;
        }
        else
        //2. seq. of two IACs
          if (d->iac && (ch == TN_IAC) && (!d->insb))
        {
          d->iac = false;
          cleandata += ch;
          d->command = "";
        }
        else
        //3. IAC DO/DONT/WILL/WONT
          if (d->iac && (!d->insb) &&
            ((ch == TN_WILL) || (ch == TN_WONT) || (ch == TN_DO) || (ch == TN_DONT)))
        {
          d->iac = false;
          d->iac2 = true;
          d->command += ch;
        }
        else
        //4. IAC DO/DONT/WILL/WONT <command code>
          if (d->iac2)
        {
          d->iac2 = false;
          d->command += ch;
          processTelnetCommand (d->command);
          d->command = "";
        }
        else
        //5. IAC SB
          if (d->iac && (!d->insb) && (ch == TN_SB))
        {
          d->iac = false;
          d->insb = true;
          d->command += ch;
        }
        else
        //6. IAC SE without IAC SB - error - ignored
          if (d->iac && (!d->insb) && (ch == TN_SE))
        {
          d->command = "";
          d->iac = false;
        }
        else
        //7. inside IAC SB
          if (d->insb)
        {
          d->command += ch;
          if (d->iac && (ch == TN_SE))  //IAC SE - end of subcommand
          {
            processTelnetCommand (d->command);
            d->command = "";
            d->iac = false;
            d->insb = false;
          }
          if (d->iac)
            d->iac = false;
          else
          if (ch == TN_IAC)
              d->iac = true;
        }
        else
        //8. IAC fol. by something else than IAC, SB, SE, DO, DONT, WILL, WONT
        {
          d->iac = false;
          d->command += ch;
          processTelnetCommand (d->command);
          //this could have set receivedGA to true; we'll handle that later
          // (at the end of this function)
          d->command = "";
        }
      }
      else   //plaintext
      {
        //everything except CRLF is okay; CRLF is replaced by LF(\n) (CR ignored)
        if (ch != 13)
          cleandata += ch;
      }

      // TODO: do something about all that code duplication ...

      //we've just received the GA signal - higher layers shall be informed about it
      if (d->recvdGA)
      {
        //ask MSP parser to look for MSP tags if MSP is enabled
        //look if the user want to have MSP all the time
        bool alwaysmsp = sett ? sett->getBool ("always-msp") : false;
        //hand data to MSP parser if desired
        if (d->usingmsp || alwaysmsp)
          cleandata = d->MSP->parseServerOutput (cleandata);

        //prepend a newline, if needed
        if (d->prependGANewLine && d->t_lpmudstyle)
          cleandata = "\n" + cleandata;
        d->prependGANewLine = false;
        //forward data for further processing
        QString unicodeData = d->inCoder->decode (QByteArray(cleandata.data(), cleandata.length()));
        invokeEvent ("data-received", sess(), unicodeData);

        if (sett && sett->getBool ("prompt-console"))
          //we'll need to prepend a new-line in next data sending
          d->prependGANewLine = true;
        //we got a prompt
        invokeEvent ("received-ga", sess());

        //clean the flag, and the data (so that we don't send it multiple times)
        cleandata = "";
        d->recvdGA = false;
      }
    }

    //some data left to send - do it now!
    if (!cleandata.empty())
    {
      //ask MSP parser to look for MSP tags if MSP is enabled
      //look if the user want to have MSP all the time
      bool alwaysmsp = sett ? sett->getBool ("always-msp") : false;
      //hand data to MSP parser if desired
      if (d->usingmsp || alwaysmsp)
        cleandata = d->MSP->parseServerOutput (cleandata);

      //prepend a newline, if needed
      if (d->prependGANewLine && d->t_lpmudstyle)
        cleandata = "\n" + cleandata;
      d->prependGANewLine = false;
      //forward data for further processing
      QString unicodeData = d->inCoder->decode (QByteArray(cleandata.data(), cleandata.length()));
      invokeEvent ("data-received", sess(), unicodeData);
    }
  }

  invokeEvent ("text-here", sess());
}

void cTelnet::socketClosed ()
{
  disconnect ();
}

void cTelnet::socketHostFound ()
{
  invokeEvent ("message", sess(), i18n ("The remote server has been found, attempting connection."));
}

bool cTelnet::isOffLineConnection () { 
  return d->offLineConnection;
}

int cTelnet::compressedBytes () {
  return d->MCCP->compressedBytes ();
}

int cTelnet::uncompressedBytes () {
  return d->MCCP->uncompressedBytes ();
}

int cTelnet::sentBytes () {
  return d->sentbytes;
}

bool cTelnet::usingMCCP () {
  return d->MCCP->usingMCCP();
}

int cTelnet::MCCPVer () {
  return d->MCCP->MCCPVer();
}

void cTelnet::setCommandEcho (bool cmdEcho)
{
  d->t_cmdEcho = cmdEcho;
}

void cTelnet::setLPMudStyle (bool lpmudstyle)
{
  d->t_lpmudstyle = lpmudstyle;
}

void cTelnet::setNegotiateOnStartup (bool startupneg)
{
  d->_startupneg = startupneg;
}

#include "moc_ctelnet.cpp"
