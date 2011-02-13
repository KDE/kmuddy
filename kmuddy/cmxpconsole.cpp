//
// C++ Implementation: cmxpconsole
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
