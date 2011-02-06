//
// C++ Implementation: cmxpconsole
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmxpconsole.h"

#ifdef HAVE_MXP

#include "cactionmanager.h"
#include "dialogs/dlgmxpconsole.h"

#include <kmainwindow.h>

cMXPConsole *cMXPConsole::_self = 0;

cMXPConsole::cMXPConsole ()
{
  createDialog ();
}

cMXPConsole::~cMXPConsole ()
{
  delete dlgmxpconsole;
}

cMXPConsole *cMXPConsole::self ()
{
  if (!_self)
    _self = new cMXPConsole;
  return _self;
}

QDockWidget *cMXPConsole::dialog ()
{
  return dlgmxpconsole;
}

void cMXPConsole::createDialog ()
{
  KMainWindow *wnd = cActionManager::self()->mainWindow();
  dlgmxpconsole = new dlgMXPConsole (wnd);
  dlgmxpconsole->hide();  // hidden by default
  dlgmxpconsole->setObjectName ("mxpconsole");
  wnd->addDockWidget (Qt::RightDockWidgetArea, dlgmxpconsole);
  dlgmxpconsole->setFloating (true);
}

#endif  //HAVE_MXP

//slots - these need to exist even if HAVE_MXP is off

void cMXPConsole::addError (int sess, const QString &text)
{
#ifdef HAVE_MXP
  
  if (!dlgmxpconsole)
    createDialog ();

  QString name = cActionManager::self()->callAction ("session", "name", sess);
  dlgmxpconsole->addLine ("[ error ] (" + name + ") " + text);
  
#endif  //HAVE_MXP  
}

void cMXPConsole::addWarning (int sess, const QString &text)
{
#ifdef HAVE_MXP
  
  if (!dlgmxpconsole)
    createDialog ();

  QString name = cActionManager::self()->callAction ("session", "name", sess);
  dlgmxpconsole->addLine ("[ warning ] (" + name + ") " + text);
  
#endif  //HAVE_MXP  
}


#include "cmxpconsole.moc"
