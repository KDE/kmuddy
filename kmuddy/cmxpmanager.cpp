//
// C++ Implementation: cmxpmanager
//
// Description:
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cmxpmanager.h"

#ifdef HAVE_MXP

#include "cgaugelist.h"
#include "cglobalsettings.h"
#include "cprofilesettings.h"
#include "cstatusvarlist.h"
#include "ctelnet.h"
#include "cvariablelist.h"
#include "kmuddy-version.h"
#include "cmxpconsole.h"

#include <klocale.h>

// from ctextchunk.h
#define ATTRIB_BOLD 1
#define ATTRIB_ITALIC 2
#define ATTRIB_UNDERLINE 4
#define ATTRIB_STRIKEOUT 8

#endif

cMXPManager::cMXPManager (int sess) : cActionBase ("mxpmanager", sess)
{
#ifdef HAVE_MXP
  havehandler = false;
  reset ();
  addEventHandler ("connected", 100, PT_NOTHING);
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
#endif
}

cMXPManager::~cMXPManager ()
{
#ifdef HAVE_MXP
  removeEventHandler ("connected");
  removeGlobalEventHandler ("global-settings-changed");

  if (havehandler)
    mxpDestroyHandler (h);
  havehandler = false;
#endif
}

#ifdef HAVE_MXP

void cMXPManager::eventNothingHandler (QString event, int)
{
  if (event == "connected")
    reset ();
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    setDefaultFont (gs->getFont ("console-font"),
        gs->getColor ("color-" + QString::number(gs->getInt ("fg-color"))),
        gs->getColor ("color-" + QString::number(gs->getInt ("bg-color"))));
  }
}

QString cMXPManager::actionIntHandler (QString action, int, int par1, int par2)
{
  if (action == "set-active") {
    setMXPActive (par1 != 0);
  }
  return QString();
}

QString cMXPManager::actionNothingHandler (QString action, int)
{
  if (action == "switch-open") switchToOpenMode ();
  return QString();
}

void cMXPManager::setMXPActive (bool active)
{
  if (active == mxpactive)
    return;    //nothing to do

  if (active)
  {
    if (!havehandler)
      initLibrary ();
  }

  mxpactive = active;
}

void cMXPManager::processText (const QString &text)
{
  if (!havehandler)
    return;

  //give the text to the library
  mxpProcessText (h, text.toLocal8Bit());

  //process each result
  while (mxpHasResults (h)) {
    mxpResult *res = mxpNextResult (h);
    if (res->type >= 2)  //got some MXP stuff...
      mxpinuse = true;
    processResult (res);
  };
}

void cMXPManager::switchToOpenMode ()
{
  if (!havehandler)
    return;
  mxpSwitchToOpen (h);
}

void cMXPManager::setDefaultFont (QFont font, QColor fg, QColor bg)
{
  deffont = font;
  deffg = fg;
  defbg = bg;
  //send font info to the lib, if we have a handler
  if (havehandler)
    sendDefaultFont ();
}

void cMXPManager::processResult (mxpResult *res)
{
  switch (res->type) {
    case 0: { /* NOTHING */ }
    break;
    case 1: {   //text or newline or SBR
      char *s = (char *) res->data;
      if (s && (strcmp (s, "\r\n") == 0))  //newlines are always sent separately
        emit gotNewLine ();
      else
      if (s && ((s[0] == 0x1f) && (s[1] == '\0')))
        //<SBR> tag - ignored - converted into a space
        emit gotNewText (QString (" "));
      else
        emit gotNewText (s ? QString::fromLocal8Bit (s) : QString (""));
    }
    break;
    case 3: {
    // NOTHING HERE YET
    }
    break;
    case 4: {   //variable
      varStruct *vs = (varStruct *) res->data;
      char *n = vs->name;
      char *v = vs->value;
      cVariableList *vl = dynamic_cast<cVariableList *>(object ("variables"));
      cProfileSettings *sett = settings();
      if (sett && n)  //we need a profile and a variable name
      {
        QString prefix = sett->getString ("mxp-variable-prefix");
        if (vs->erase)
          //erase that variable
          vl->unset (prefix + QString(n));
        else
          //set that variable
          vl->set (prefix+QString(n), v ? QString::fromLocal8Bit(v) : QString());
      }
    }
    break;
    case 5: {  //formatting
      formatStruct *fs = (formatStruct *) res->data;
      unsigned char mask = fs->usemask;

      //colors
      if (mask & USE_FG)
        emit gotFgColor (toColor (fs->fg));
      if (mask & USE_BG)
        emit gotBgColor (toColor (fs->bg));

      //font and font size: IGNORED

      //attributes
      int oldattrib = curattrib;
      if (mask & USE_BOLD)
        curattrib = (fs->attributes & Bold) ? (curattrib | ATTRIB_BOLD) :
            (curattrib & ~ATTRIB_BOLD);
      if (mask & USE_ITALICS)
        curattrib = (fs->attributes & Italic) ? (curattrib | ATTRIB_ITALIC) :
            (curattrib & ~ATTRIB_ITALIC);
      if (mask & USE_UNDERLINE)
        curattrib = (fs->attributes & Underline) ? (curattrib | ATTRIB_UNDERLINE) :
            (curattrib & ~ATTRIB_UNDERLINE);
      if (mask & USE_STRIKEOUT)
        curattrib = (fs->attributes & Strikeout) ? (curattrib | ATTRIB_STRIKEOUT) :
            (curattrib & ~ATTRIB_STRIKEOUT);
      if (curattrib != oldattrib) //attributes have changed somehow
        emit gotAttrib (curattrib);
    }
    break;
    case 6: {  //A-link
      linkStruct *ls = (linkStruct *) res->data;
      QString name = ls->name ? QString::fromLocal8Bit (ls->name) : QString();
      QString url = ls->url ? QString::fromLocal8Bit (ls->url) : QString();
      QString text = ls->text ? QString::fromLocal8Bit (ls->text) : QString();
      QString hint = ls->hint ? QString::fromLocal8Bit (ls->hint) : QString();
      emit gotALink (name, url, text, hint);
    }
    break;
    case 7: {  //SEND-link
      sendStruct *ss = (sendStruct *) res->data;
      QString name = ss->name ? QString::fromLocal8Bit (ss->name) : QString();
      QString command = ss->command ? QString::fromLocal8Bit (ss->command) : QString();
      QString text = ss->text ? QString::fromLocal8Bit (ss->text) : QString();
      QString hint = ss->hint ? QString::fromLocal8Bit (ss->hint) : QString();
      emit gotSENDLink (name, command, text, hint, ss->toprompt, ss->ismenu);
    }
    break;
    case 8: {  //expire link
      char *name = (char *) res->data;
      if (name)
        emit gotExpire (QString (name));
      else
        emit gotExpire (QString());
    }
    break;
    case 9: {  //send this
      char *s = (char *) res->data;
      cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet"));
      telnet->sendData (s);
    }
    break;
    case 11: {  //sound/music
      soundStruct *ss = (soundStruct *) res->data;
      QString fName = ss->fname ? QString (ss->fname) : QString();
      QString url = ss->url ? QString (ss->url) : QString();
      QString type = ss->type ? QString (ss->type) : QString();
      int priority = ss->isSOUND ? ss->priority : (ss->continuemusic ? 1 : 0);

      cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet"));
      telnet->processSoundRequest (ss->isSOUND, fName, ss->vol, ss->repeats,
          priority, type, url);
    }
    break;
    case 22: {  //gauge
      gaugeStruct *gs = (gaugeStruct *) res->data;
      QString var = gs->variable ? QString (gs->variable) : QString();
      QString maxvar = gs->maxvariable ? QString (gs->maxvariable) : QString();
      QString caption = gs->caption ? QString::fromLocal8Bit (gs->caption) : QString();
      QColor color = toColor (gs->color);

      cGaugeList *gl = dynamic_cast<cGaugeList *>(object ("gaugelist"));
#warning We need to re-enable the MXP gauges !
      //if (gl && (!var.isEmpty()))
        //gl->requestGauge (var, maxvar, caption, color, false);
    }
    break;
    case 23: {  //status bar
      statStruct *ss = (statStruct *) res->data;
      QString var = ss->variable ? QString (ss->variable) : QString();
      QString maxvar = ss->maxvariable ? QString (ss->maxvariable) : QString();
      QString caption = ss->caption ? QString::fromLocal8Bit (ss->caption) : QString();

      cStatusVarList *svl = dynamic_cast<cStatusVarList *>(object ("statusvarlist"));
#warning We need to re-enable the MXP status variables !
      //if (svl && (!var.isEmpty()))
        //svl->requestStatusVar (var, maxvar, caption, false, false);
    }
    break;
    case -1: {  //error
      char *s = (char *) res->data;
      cMXPConsole::self()->addError (sess(), QString::fromLocal8Bit (s));
    }
    break;
    case -2: {  //warning
      char *s = (char *) res->data;
      cMXPConsole::self()->addWarning (sess(), QString::fromLocal8Bit (s));
    }
    break;
    default: {
      QString reqName;
      switch (res->type) {
        case 2: reqName = i18n ("line tag"); break;
        case 8: reqName = i18n ("expire link"); break;
        case 10: reqName = i18n ("horizontal line"); break;
        case 12: reqName = i18n ("create window"); break;
        case 13: reqName = i18n ("create internal window"); break;
        case 14: reqName = i18n ("close window"); break;
        case 15: reqName = i18n ("set active window"); break;
        case 16: reqName = i18n ("move cursor"); break;
        case 17: reqName = i18n ("erase text"); break;
        case 18: reqName = i18n ("relocate"); break;
        case 19: reqName = i18n ("send username/password"); break;
        case 20: reqName = i18n ("image"); break;
        case 21: reqName = i18n ("image map"); break;
        default: reqName = i18n ("unknown"); break;
      };
      cMXPConsole::self()->addWarning (sess(), i18n ("Ignoring unsupported MXP request (%1)", reqName));
    }
    break;
  };
}

QColor cMXPManager::toColor (RGB mxpColor)
{
  return QColor (mxpColor.r, mxpColor.g, mxpColor.b);
}

void cMXPManager::sendDefaultFont ()
{
  const char *name = deffont.family ().toLatin1();
  int size = deffont.pointSize ();
  bool bold = deffont.bold ();
  bool italics = deffont.italic ();
  bool underline = deffont.underline ();
  bool strikeout = deffont.strikeOut ();
  RGB fg, bg;
  fg.r = deffg.red ();
  fg.g = deffg.green ();
  fg.b = deffg.blue ();
  bg.r = defbg.red ();
  bg.g = defbg.green ();
  bg.b = defbg.blue ();

  //remember current attributes
  curattrib = ATTRIB_BOLD * (bold?1:0) + ATTRIB_ITALIC * (italics?1:0) +
      ATTRIB_UNDERLINE * (underline?1:0) + ATTRIB_STRIKEOUT * (strikeout?1:0);

  mxpSetDefaultText (h, name, size, bold, italics, underline, strikeout, fg, bg);
  mxpSetNonProportFont (h, name);
  //header params... most of these aren't used anyway ;)
  mxpSetHeaderParams (h, 1, name, size * 3, true, false, true, false, fg, bg);
  mxpSetHeaderParams (h, 2, name, size * 2, true, false, false, false, fg, bg);
  mxpSetHeaderParams (h, 3, name, size * 3 / 2, true, false, false, false, fg, bg);
  mxpSetHeaderParams (h, 4, name, size * 4 / 3, true, false, false, false, fg, bg);
  mxpSetHeaderParams (h, 5, name, size + 2, true, false, false, false, fg, bg);
  mxpSetHeaderParams (h, 6, name, size, true, false, false, false, fg, bg);
}

void cMXPManager::initLibrary ()
{
  h = mxpCreateHandler ();

  mxpSetClient (h, PACKAGE, VERSION);
  sendDefaultFont ();

  //set supported features...
  mxpSupportsLink (h, true);
  mxpSupportsGauge (h, true);
  mxpSupportsStatus (h, true);
  mxpSupportsSound (h, true);
  mxpSupportsFrame (h, false);
  mxpSupportsImage (h, false);
  mxpSupportsRelocate (h, false);

  //mxpSetScreenProps is not issued, because we don't need it yet

  havehandler = true;
}

#endif

//slot - must always exist
void cMXPManager::reset ()
{
#ifdef HAVE_MXP

  if (havehandler)
    mxpDestroyHandler (h);
  havehandler = false;
  mxpactive = false;
  mxpinuse = false;
  deffont.setFamily ("fixed");  //some default font, will be changed when settings are loaded
  deffg = Qt::lightGray;
  defbg = Qt::black;
  curattrib = 0;

#endif  //HAVE_MXP
}

