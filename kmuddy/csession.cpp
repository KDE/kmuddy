/***************************************************************************
                          csession.cpp  -  basis of multi-world support
                              -------------------
    begin                : Pi nov 15 2002
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

#include "csession.h"

#include "cansiparser.h"
#include "ccmdparser.h"
#include "ccmdprocessor.h"
#include "ccmdqueues.h"
#include "cconnection.h"
#include "cgaugebar.h"
#include "cglobalsettings.h"
#include "cinputline.h"
#include "cmultiinputline.h"
#include "coutput.h"
#include "cprompt.h"
#include "cscriptapi.h"
#include "cscripteval.h"
#include "cstatus.h"
#include "ctelnet.h"
#include "ctextprocessor.h"
#include "ctranscript.h"

#ifdef HAVE_MXP
#include "cmxpmanager.h"
#endif  //HAVE_MXP

// for variable change reporting
#include "clistmanager.h"
#include "cgaugelist.h"
#include "cstatusvarlist.h"

#include <kapplication.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <khbox.h>

#include <QPushButton>
#include <QToolTip>
#include <QVBoxLayout>

cSession::cSession (int sess, QWidget *parent) : QWidget (parent), cActionBase ("session", sess)
{
  _flashing = false;
  defaultName = true;

  //create objects
  _telnet = new cTelnet (sess);
#ifdef HAVE_MXP
  _mxpmanager = new cMXPManager (sess);
#endif
  _ansiparser = new cANSIParser (sess);
  _parser = new cCmdParser (sess);
  _cmdproc = new cCmdProcessor (sess);
  _cmdqueues = new cCmdQueues (sess);
  _textproc = new cTextProcessor (sess);
  _connection = new cConnection (sess);

  _output = new cOutput (sess, this);
  
  KHBox *promptinput = new KHBox (this);
  _prompt = new cPrompt (sess, promptinput);
  
  _inputline = new cInputLine (sess, "inputline", promptinput);
  _inputline->initialize ();
  
  _multiinputline = new cMultiInputLine (sess, promptinput);
  _multiinputline->hide ();
  _multiinputline->setToolTip (i18n ("Ctrl+Enter adds new line, unless changed in settings"));
  
  QPushButton *switchlinebutton = new QPushButton (promptinput);
  switchlinebutton->setIcon (QIcon::fromTheme ("format-justify-center"));
  switchlinebutton->setToolTip (i18n ("Switches between standard and multi-line input line."));
  
  //the button should take as few space as possible
  switchlinebutton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Minimum);
  switchlinebutton->setFocusPolicy (Qt::NoFocus);
 
  _auxinputline = new cInputLine (sess, "auxinputline", this);
  _auxinputline->initialize ();
  _auxinputline->hide ();
  
  _gaugebar = new cGaugeBar (sess, this);
  _gaugebar->hide();

  KStatusBar *statusbar = new KStatusBar (this);
  _status = new cStatus (sess, statusbar);

  QVBoxLayout *layout = new QVBoxLayout (this);
  layout->addWidget (_output);
  layout->addWidget (promptinput);
  layout->addWidget (_auxinputline);
  layout->addWidget (_gaugebar);
  layout->addWidget (statusbar);

  _transcript = new cTranscript (sess);
  _scripteval = new cScriptEval (sess);
  _scriptapi = new cScriptAPI (sess);
  _scripteval->addObject ("kmuddy", _scriptapi);

  //the button switches between standard input and multi-line input
  connect (switchlinebutton, SIGNAL (clicked ()), this, SLOT (switchInputLines ()));

  KApplication::kApplication()->processEvents ();

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
  
  addEventHandler ("var-changed", 50, PT_STRING);
}

cSession::~cSession ()
{
  removeGlobalEventHandler ("global-settings-changed");
  
  removeEventHandler ("var-changed");

  delete _scripteval;
  //transcript must be deleted before the console!
  delete _transcript;

  delete _status;  //this also destroys the statusbar
  delete _multiinputline;
  delete _auxinputline;
  delete _inputline;
  delete _output;
  delete _prompt;

  delete _connection;
  delete _textproc;
  delete _cmdqueues;
  delete _cmdproc;
  delete _parser;
  delete _ansiparser;
#ifdef HAVE_MXP
  delete _mxpmanager;
#endif
  delete _telnet;
}

QString cSession::actionNothingHandler (QString action, int)
{
  if (action == "name")
    return name();

  return QString();
}

void cSession::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    setAuxInput (gs->getBool ("aux-input"));
  }
}

void cSession::eventStringHandler (QString event, int session,
    QString &par1, const QString &)
{
  // pass variable changes to lists that need them
  // this is a bit of a hack, necessary because lists cannot react on events
  // ultimately the affected lists (gauges, statusvars) should move
  // to plugins, and the plugin should handle these events

  if (event == "var-changed") {
    cListManager *lm = cListManager::self();
    cGaugeList *gl = (cGaugeList *) lm->getList (sess(), "gauges");
    cStatusVarList *svl = (cStatusVarList *) lm->getList (sess(), "statusvars");

    if (gl) gl->variableChanged (par1);
    if (svl) svl->variableChanged (par1);
  }
}

void cSession::setName (const QString &name, bool defName)
{
  //don't overwrite non-default name with a default one
  if (defName && (!defaultName))
    return;
  
  _name = name;
  defaultName = defName;
}

void cSession::setAuxInput (bool val)
{
  val ? _auxinputline->show () : _auxinputline->hide ();
}

void cSession::switchInputLines ()
{
  if (_inputline->isVisible())
  {
    _inputline->hide();
    _multiinputline->show();
    _multiinputline->setFocus ();
  }
  else
  {
    _multiinputline->hide();
    _inputline->show();
    _inputline->setFocus ();
  }
}

