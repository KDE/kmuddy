/***************************************************************************
                          cconnection.cpp  -  handles connections
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Jul 22 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#include "cconnection.h"

#include <config-mxp.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <qdatetime.h>
#include <qtimer.h>

#include "kmuddy.h"
#include "ccmdprocessor.h"
#include "clistmanager.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"
#include "csessionmanager.h"
#include "ctelnet.h"

#include "caliaslist.h"
#include "ctriggerlist.h"
#include "cbuttonlist.h"
#include "cgaugelist.h"
#include "ctimerlist.h"
#include "cshortcutlist.h"
#include "cstatusvarlist.h"
#include "cvariablelist.h"
#include "cwindowlist.h"

#include "ctranscript.h"

#include <map>

#include "dialogs/dlgprofilesettings.h"
#include "dialogs/dlgwindows.h"

#define AUTOSAVE_INTERVAL 5

// TODO: lists will go to a separate class - cListManager

/** this class holds private data of a cConnection object */
struct cConnectionPrivate {

  bool profileConnection;
  QString profileName;
  QString quickServer;
  int quickPort;  // server/port for quick connections

  QString caption;

  QString commands;
  // waiting for commands - no immediate sending
  bool waitCommands;

  int commandNum;

  QTimer *connecting;
  QTimer *saver;

  dlgProfileSettings *sdlg;

  bool destroying;

  bool connclosed;
  bool canCT;

  bool connInProgress, qconnInProgress, sendNothing;

  //login sequence and its iterator
  QStringList loginSeq;
  QStringList::iterator seqIter;

  //lists with data
  cVariableList *variablelist;
  cWindowList *windowlist;
};

cConnection::cConnection (int sess) : cActionBase ("connection", sess)
{
  d = new cConnectionPrivate;
  d->profileConnection = false;
  d->destroying = false;
  d->connecting = 0;
  d->connclosed = false;
  d->canCT = false;
  d->connInProgress = d->qconnInProgress = d->sendNothing = false;
  d->commands = QString();
  d->waitCommands = false;
  d->commandNum = 0;

  d->variablelist = 0;
  d->windowlist = 0;

  d->saver = new QTimer;
  connect (d->saver, SIGNAL (timeout ()), this, SLOT (saveSession ()));

  addEventHandler ("connected", 10, PT_NOTHING);
  addEventHandler ("save", 10, PT_NOTHING);
  addEventHandler ("disconnected", 500, PT_NOTHING);
  addEventHandler ("connection-failed", 100, PT_STRING);
  addEventHandler ("send-command", 50, PT_STRING);
  addEventHandler ("command-block", 50, PT_NOTHING);
  addEventHandler ("send-commands", 50, PT_NOTHING);
}

cConnection::~cConnection ()
{
  d->destroying = true;
  disconnect ();  //maybe we aren't connected, but disconnect() can handle it

  removeEventHandler ("connected");
  removeEventHandler ("disconnected");
  removeEventHandler ("connection-failed");
  removeEventHandler ("send-command");
  removeEventHandler ("command-block");
  removeEventHandler ("send-commands");

  delete d->saver;
  delete d;
}

#include <kdebug.h>
void cConnection::eventNothingHandler (QString event, int)
{
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (telnet == 0)
    return;

  if (event == "connected") {
    // TODO: better way of detecting quick/normal - other classes need it too
    // probably just some setting set upon creating objects (loader knows session type)

    setAttrib ("connected", 1);
    d->connclosed = false;
    d->canCT = false;
    d->commands = QString();
    updateMenus ();
    KMuddy::self()->enableConnectedOptions ();

    if (d->connInProgress) {
      // register the internal lists if needed
      registerLists ();

      // initialize the profile
      invokeEvent ("profile-init", sess(), d->profileName);

      // load the variable list and the window list
      // these two are not handled by the list manager
      // variable list must be loaded before the other lists are
      d->variablelist = new cVariableList (sess());
      d->windowlist = new cWindowList (sess());

      // load the profile
      invokeEvent ("profile-load", sess(), d->profileName);

      // profile is loaded, so we can fetch settings by session ID
      cProfileSettings *sett = cProfileManager::self()->settings (sess());
      // adjust everything based on the profile settings
      invokeEvent ("settings-changed", sess());

      QString login = sett->getString ("login");
      QString profile = cProfileManager::self()->visibleProfileName (d->profileName);

      // set some variables - $character, $session, $profile
      cVariableList *vl = dynamic_cast<cVariableList *>(object ("variables"));
      vl->set ("character", login);
      vl->set ("profile", d->profileName);
      vl->set ("session", profile);

      KMuddy::self()->enableAdvancedOptions ();

      //update caption
      d->caption = login.isEmpty() ? profile : (login + "@" + profile);
      cSessionManager::self()->setSessionName (sess(), d->caption);

      // Here be autologging
      if (settings()->getBool ("auto-adv-transcript"))
      {
        cTranscript *transcript = dynamic_cast<cTranscript *>(object ("transcript"));
        transcript->load();
        transcript->startAdvTranscript();
      }

      //everything's ready - schedule sending of login sequence if needed...
      if (!d->sendNothing)
      {
        // generate the login sequence
        d->loginSeq.clear();
        int seqn = sett->getInt ("on-connect-count");
        for (int i = 0; i < seqn; ++i)
          d->loginSeq << sett->getString ("on-connect-"+QString::number(i));
        
        d->seqIter = d->loginSeq.begin();
        if ((seqn > 1) || ((seqn == 1) && (d->loginSeq[0].length() > 0)))
        {
          d->connecting = new QTimer;
          connect (d->connecting, SIGNAL (timeout()), this,
              SLOT (sendLoginAndPassword()));
          telnet->waitingForData();
          d->connecting->start (400);
        }
      }

      //also schedule automatic saving of settings and objects
      d->saver->start (AUTOSAVE_INTERVAL * 60 * 1000);
      (KApplication::kApplication())->processEvents ();

    } else
    {
      KMuddy::self()->disableAdvancedOptions ();

  #ifdef HAVE_MXP
      //allow MXP auto-detection for quick connections
      telnet->setMXPAllowed (3);
  #endif

      //update application caption
      //update caption
      d->caption = d->quickServer + ":" + QString::number (d->quickPort);
      cSessionManager::self()->setSessionName (sess(), d->caption);
    }

    d->connInProgress = d->qconnInProgress = false;
  }
  else if (event == "disconnected") {
    disconnect ();
  }
  else if (event == "save") {
    save ();
  }
  else if (event == "command-block") {
    d->waitCommands = true;
  }
  else if (event == "send-commands") {
    sendCommands ();
  }
}

void cConnection::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "send-command") {
    if (par1.isEmpty())
      addCommand (par1);  // allow empty commands to be sent
    else
      addCommands (par1.split ("\n"));
  }
  else if (event == "connection-failed") {
    invokeEvent ("message", sess(), i18n ("Network error: %1", par1));
    setConnectionClosed (true);
    updateMenus ();
  }
}

QString cConnection::actionStringHandler (QString action, int, QString &par1, const QString &)
{
  if (action == "send-command") {
    addCommands (par1.split ("\n"));
  }

  return QString();
}

QString cConnection::actionNothingHandler (QString action, int)
{
  if (action == "command-block") {
    d->waitCommands = true;
  }
  else if (action == "send-commands") {
    sendCommands ();
  }

  return QString();
}

QString cConnection::getCaption () {
  return d->caption;
}

void cConnection::setConnectionClosed (bool canCloseTab) {
  setAttrib ("connected", 0);
  d->connclosed = true;
  d->canCT = canCloseTab;
}

void cConnection::unsetConnectionClosed () {
  setAttrib ("connected", 1);
  d->connclosed = false;
  d->canCT = false;
}

bool cConnection::connectionClosed () {
  return d->connclosed;
}

void cConnection::showConnPrefsDialog ()
{
  cProfileSettings *sett = settings ();
  if (!sett) return;    //not a profile-based connection - do nothing
  //so first we have to create the dialog...
  d->sdlg = new dlgProfileSettings (KMuddy::self());

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (d->sdlg, SIGNAL (accepted()), this,
      SLOT (getSettingsFromDialog()));
  connect (d->sdlg->button (QDialogButtonBox::Apply), SIGNAL (clicked()), this,
      SLOT (getSettingsFromDialog()));

  //next we fill in its data
  putSettingsToDialog ();

  //dialog is ready - show it!
  d->sdlg->exec ();

  //further action is handled via slots issued by buttons, so that we only
  //have to destroy the dialog...
  delete d->sdlg;
}

void cConnection::getSettingsFromDialog ()
{
  cProfileSettings *sett = settings ();
  if (!sett) return;  // should never happen

  dlgProfileSettings *sdlg = d->sdlg;

  sett->setBool ("use-ansi", sdlg->useAnsi ());
  sett->setBool ("limit-repeater", sdlg->limitRepeater ());
  sett->setString ("encoding", sdlg->encoding ());
  sett->setBool ("startup-negotiate", sdlg->negotiateOnStartup ());
  sett->setBool ("lpmud-style", sdlg->LPMudStyle());
  sett->setBool ("prompt-label", sdlg->promptLabel());
  sett->setBool ("prompt-status", sdlg->statusPrompt());
  sett->setBool ("prompt-console", sdlg->consolePrompt());
  sett->setBool ("auto-adv-transcript", sdlg->autoAdvTranscript());

  for (int i = 0; i < 10; i++)
    sett->setString ("movement-command-"+QString::number(i), sdlg->movementCmd (i));
  sett->setString ("script-directory", sdlg->scriptDir ());
  sett->setString ("script-working-directory", sdlg->scriptWorkDir ());
  sett->setString ("transcript-directory", sdlg->transcriptDir ());

  QStringList sdirs = sdlg->soundDirList();
  sett->setInt ("sound-dir-count", sdirs.size());
  int idx = 0;
  QStringList::iterator it;
  for (it = sdirs.begin(); it != sdirs.end(); ++it)
    sett->setString ("sound-dir-" + QString::number (++idx), *it);
  sett->setBool ("use-msp", sdlg->useMSP());
  sett->setBool ("always-msp", sdlg->alwaysMSP());
  sett->setBool ("midline-msp", sdlg->midlineMSP());

#ifdef HAVE_MXP
  sett->setInt ("use-mxp", sdlg->MXPAllowed());
  sett->setString ("mxp-variable-prefix", sdlg->variablePrefix ());
#endif  //HAVE_MXP

  //inform everyone ...
  invokeEvent ("dialog-save", sess(), "profile-prefs");
  invokeEvent ("settings-changed", sess());

  sett->save ();
}

void cConnection::putSettingsToDialog ()
{
  cProfileSettings *sett = settings ();
  if (!sett) return;  // should never happen

  dlgProfileSettings *sdlg = d->sdlg;

  sdlg->setUseAnsi (sett->getBool ("use-ansi"));
  sdlg->setLimitRepeater (sett->getBool ("limit-repeater"));
  sdlg->setEncoding (sett->getString ("encoding"));
  sdlg->setNegotiateOnStartup (sett->getBool ("startup-negotiate"));
  sdlg->setLPMudStyle (sett->getBool ("lpmud-style"));
  sdlg->setPromptLabel (sett->getBool ("prompt-label"));
  sdlg->setStatusPrompt (sett->getBool ("prompt-status"));
  sdlg->setConsolePrompt (sett->getBool ("prompt-console"));
  sdlg->setAutoAdvTranscript (sett->getBool ("auto-adv-transcript"));

  for (int i = 0; i < 10; i++)
    sdlg->setMovementCmd (i, sett->getString ("movement-command-"+QString::number(i)));
  sdlg->setScriptDir (sett->getString ("script-directory"));
  sdlg->setScriptWorkDir (sett->getString ("script-working-directory"));
  sdlg->setTranscriptDir (sett->getString ("transcript-directory"));

  QStringList sdirs;
  int cnt = sett->getInt ("sound-dir-count");
  for (int i = 1; i <= cnt; ++i)
    sdirs << sett->getString ("sound-dir-" + QString::number (i));
  sdlg->setSoundDirList (sdirs);
  sdlg->setUseMSP (sett->getBool ("use-msp"));
  sdlg->setAlwaysMSP (sett->getBool ("always-msp"));
  sdlg->setMidlineMSP (sett->getBool ("midline-msp"));

#ifdef HAVE_MXP
  sdlg->setMXPAllowed (sett->getInt ("use-mxp"));
  sdlg->setVariablePrefix (sett->getString ("mxp-variable-prefix"));
#endif  //HAVE_MXP

  invokeEvent ("dialog-create", sess(), "profile-prefs");
}

bool cConnection::handleDisconnect ()
{
  if (!isConnected())
    return true;    //no longer connected
  //ask if he really wants to disconnect
  if (KMessageBox::questionYesNoCancel (KMuddy::self(),
        i18n("Do you want to close connection to %1?", d->caption),
        i18n("Disconnect")) == KMessageBox::Yes)
  {
    disconnect ();
    return true;
  }
  return false;
}

void cConnection::establishConnection (const QString &profileName, bool sendNothing)
{
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (!telnet)
    return;
  cProfileSettings *sett = cProfileManager::self()->settings (profileName);
  if (!sett) return;

  d->profileConnection = true;
  d->profileName = profileName;
  d->sendNothing = sendNothing;

  if (telnet->isConnected ())
    disconnect ();

  d->connInProgress = true;
  d->qconnInProgress = false;
  telnet->connectIt (sett->getString ("server"), sett->getInt ("port"), sett);
  // now we wait for event from telnet (standard connections only, offline ones
  // return immediately)
}

void cConnection::sendLoginAndPassword ()
{
  //This function is used to send login sequence (usually login name and
  //password) to the MUD.
  //Problem: it's not very good at detecting login prompt - name can be sent
  //too early, causing password to be sent too early as well...
  //I can only hope that the server will handle that properly.

  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (!telnet)
    return;
  static int wasData = false; //have some data already arrived?

  cProfileSettings *sett = cProfileManager::self()->settings (sess());
  if (!sett) return;

  if (telnet->isConnected())
  {
    if (wasData)
    {
      if (telnet->newData() || (d->seqIter == d->loginSeq.begin()))
      //only send next command if something new has arrived (should be reply
      //to previous sent command). Also send it if it's the first command,
      //where the required data was received during previous call - used to
      //wait for intro screens and such).
      {
        //get next command from login sequence
        QString command = *d->seqIter;
        QString cmd = "", cmddisp = "";

        //expand $name and $password
        int len = command.length();
        QString varname = QString();

        bool expanded = false;
        bool invar = false;
        for (int i = 0; i < len; i++)
        {
          if (invar)
          {
            varname += command[i];
            if (varname == "$name")
            {
              varname = "";
              invar = false;
              cmd += sett->getString ("login");
              cmddisp += sett->getString ("login");
              expanded = true;
            }
            if (varname == "$password")
            {
              varname = "";
              invar = false;
              cmd += sett->getString ("password");
              cmddisp += i18n("(password is hidden)");
              expanded = true;
            }
          }
          else
            if (command[i].toLatin1() == '$')
            {
              invar = true;
              varname = "$";
            }
            else
            {
              cmd += command[i];
              cmddisp += command[i];
            }
        }

        //send resulting command and display notification
        //lines containing the name and password are sent as-is, others are sent to the
        //command processor - hence aliases/script names/... get expanded
        if (expanded)
        {
          if (!cmd.isEmpty()) {  // but don't send anything if we expanded to nothing - otherwise we send two empty lines if the user didn't specify name/password
            telnet->sendData(cmd);
            telnet->sendData("\n");
            invokeEvent ("command-sent", sess(), cmddisp);
          }
        }
        else
          // we have a command to send ...
          invokeEvent ("command", sess(), cmd);

        telnet->waitingForData();

        //advance to next command (that will be sent on next call)
        ++d->seqIter;

        //finish execution if there's nothing more to send
        if (d->seqIter == d->loginSeq.end())
        {
          d->connecting->stop ();
          //I have no idea why this is needed
          QObject::disconnect (d->connecting, SIGNAL (timeout()),
              this, SLOT (sendLoginAndPassword()));
          delete d->connecting;
          d->connecting = 0;
          wasData = false;
        }
      }
    }
    else
      if (telnet->newData())
      {
        telnet->waitingForData();
        wasData = true;
      }
  }
  else
  {
    d->connecting->stop ();
    //I have no idea why this is needed
    QObject::disconnect (d->connecting, SIGNAL (timeout()),
        this, SLOT (sendLoginAndPassword()));
    delete d->connecting;
    d->connecting = NULL;
    wasData = false;
  }
}

void cConnection::establishQuickConnection (const QString &server, int port)
{
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (!telnet)
    return;

  if (telnet->isConnected ())
    disconnect ();
  d->profileConnection = true;
  d->quickServer = server;
  d->quickPort = port;
  d->connInProgress = false;
  d->qconnInProgress = true;
  telnet->connectIt (server, port);

  // now wait for event from telnet
}

bool cConnection::isConnected ()
{
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (!telnet)
    return false;

  return telnet->isConnected ();
}

void cConnection::disconnect ()
{
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (telnet == 0)
    return;

  if (d->connecting != NULL)
  {
    d->connecting->stop ();
    delete d->connecting;
    d->connecting = NULL;
  }
  d->saver->stop ();
  setAttrib ("connected", 0);
  if (cProfileManager::self()->settings (sess()))
    invokeEvent ("save", sess());

  //clear command queue, so that pending commands don't close the tab
  //immediately
  d->commands = QString();

  // adjust menus, but only if this session is the active one
  if ((!d->destroying) && (cActionManager::self()->activeSession() == sess()))
  {
    KMuddy::self()->disableConnectedOptions ();
    KMuddy::self()->disableAdvancedOptions ();
  }

  // this dontDelete thing is a work-around for the event priorities problem, should be removed after that's fixed
  static bool dontDelete = false;
  if (dontDelete) return;
  dontDelete = true;

  if (telnet->isConnected())
    telnet->disconnect ();

  dontDelete = false;

  // delete our held lists - must be after the disconnected event, as the variable list
  // may still be needed there
  delete d->variablelist;
  delete d->windowlist;
  d->variablelist = 0;
  d->windowlist = 0;
}

void cConnection::reconnect ()
{
  //inform plug-ins
  invokeEvent ("reconnect", sess());

  //reconnect...
  if (d->profileConnection)
    establishConnection (d->profileName, d->sendNothing);
  else
    establishQuickConnection (d->quickServer, d->quickPort);
}

void cConnection::addCommands (const QStringList &commands)
{
  QStringList::const_iterator it;
  for (it = commands.begin(); it != commands.end(); ++it)
    addCommand (*it);
}

void cConnection::addCommand (const QString &command)
{
  QString cmd = command;
  invokeEvent ("new-command", sess(), cmd);
  if ((command != QString()) && (cmd == QString())) {
    // the handler has cleared the command - do not send it
    return;
  }
  d->commands += cmd;
  d->commands += "\r\n";
  if (!d->waitCommands)
    sendCommands ();

  // technically, this is wrong, because the command can still be in the queue,
  // but I believe it's sufficient
  invokeEvent ("command-sent", sess(), command);
}

void cConnection::sendCommands ()
{
  // TODO: reimplent using the modern architecture
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (telnet == 0)
    return;
  if (d->commands != QString())
    telnet->sendData (d->commands);
  d->commands = QString();
  d->waitCommands = false;
}

int cConnection::sentCommands ()
{
  return d->commandNum;
}

void cConnection::handleWindowsDialog ()
{
  cWindowList *wl = dynamic_cast<cWindowList *>(object ("windowlist"));
  if (!wl)
    return;

  dlgWindows *wdlg = new dlgWindows (wl, KMuddy::self());

  wdlg->exec ();

  delete wdlg;

  invokeEvent ("save", sess());
}

void cConnection::switchAliases (bool val)
{
  cListManager *lm = cListManager::self();
  cAliasList *al = dynamic_cast<cAliasList *>(lm->getList (sess(), "aliases"));
  if (!al)
    return;
  al->setEnabled (val);
  invokeEvent ("message", sess(),
      val ? i18n ("Aliases are now enabled.") : i18n ("Aliases are now disabled."));
}

void cConnection::switchTriggers (bool val)
{
  cListManager *lm = cListManager::self();
  cTriggerList *tl = dynamic_cast<cTriggerList *>(lm->getList (sess(), "triggers"));
  if (!tl)
    return;
  tl->setEnabled (val);
  invokeEvent ("message", sess(),
      val ? i18n ("Triggers are now enabled.") : i18n ("Triggers are now disabled."));
}

void cConnection::switchTimers (bool val)
{
  cListManager *lm = cListManager::self();
  cTimerList *tl = dynamic_cast<cTimerList *>(lm->getList (sess(), "timers"));
  if (!tl)
    return;
  tl->setEnabled (val);
  invokeEvent ("message", sess(),
      val ? i18n ("Timers are now enabled.") : i18n ("Timers are now disabled."));
}

void cConnection::switchShortcuts (bool val)
{
  cListManager *lm = cListManager::self();
  cShortcutList *sl = dynamic_cast<cShortcutList *>(lm->getList (sess(), "shortcuts"));
  if (!sl)
    return;
  sl->setEnabled (val);
  invokeEvent ("message", sess(),
      val ? i18n ("Macro keys are now enabled.") : i18n ("Macro keys are now disabled."));
}

void cConnection::updateMenus ()
{
  // do nothing if this session is not active
  if (sess() != cSessionManager::self()->activeSession())
    return;

  // TODO: reimplent using the modern architecture
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet", sess()));
  if (!telnet)
    return;
  if (telnet->isConnected())
  {
    KMuddy::self()->unsetClosedConn ();
    KMuddy::self()->enableConnectedOptions();
    if (settings())
      KMuddy::self()->enableAdvancedOptions();
    else
      KMuddy::self()->disableAdvancedOptions();

    if (settings())
    {
      cListManager *lm = cListManager::self();
      cList *al = lm->getList (sess(), "aliases");
      cList *tl = lm->getList (sess(), "triggers");
      cList *ml = lm->getList (sess(), "timers");
      cList *sl = lm->getList (sess(), "shortcuts");
      if (al) KMuddy::self()->setMenuAliasesEnabled (al->enabled ());
      if (tl) KMuddy::self()->setMenuTriggersEnabled (tl->enabled ());
      if (ml) KMuddy::self()->setMenuTimersEnabled (ml->enabled ());
      if (sl) KMuddy::self()->setMenuShortcutsEnabled (sl->enabled ());
    }
  }
  else
  {
    KMuddy::self()->disableConnectedOptions();
    if (d->connclosed)
      KMuddy::self()->setClosedConn (d->canCT);
    else
      KMuddy::self()->unsetClosedConn ();
  }
}

void cConnection::registerLists ()
{
  static bool registered = false;
  if (registered) return;
  registered = true;

  cListManager *lm = cListManager::self();
 
  lm->registerType ("aliases", i18n ("Aliases"), cAliasList::newList);
  lm->registerType ("triggers", i18n ("Triggers"), cTriggerList::newList);
  lm->registerType ("buttons", i18n ("Buttons"), cButtonList::newList);
  lm->registerType ("timers", i18n ("Timers"), cTimerList::newList);
  lm->registerType ("macrokeys", i18n ("Macro Keys"), cShortcutList::newList);
  lm->registerType ("statusvars", i18n ("Status variables"), cStatusVarList::newList);
  lm->registerType ("gauges", i18n ("Gauges"), cGaugeList::newList);
  // lm->registerType ("windows", i18n ("Output Windows"), cWindowList::newList);
}

void cConnection::saveSession ()
{
  invokeEvent ("save", sess());
}

void cConnection::save ()
{
  if (d->variablelist)
    d->variablelist->save();
  if (d->windowlist)
    d->windowlist->save();
}

#include "cconnection.moc"
