//
// C++ Implementation: cSessionManager
//
// Description: Session manager.
//
/*
Copyright 2002-2008 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "csessionmanager.h"

#include "cactionmanager.h"
#include "cconnection.h"
#include "cglobalsettings.h"
#include "cinputline.h"
#include "csession.h"
#include "ctabwidget.h"
#include "ctelnet.h"
#include "kmuddy.h"

#include <kdialog.h>
#include <KLocalizedString>

#include <QIcon>

struct cSessionManagerPrivate {
  cTabWidget *widget;

  cActionManager *am;

  /** icons for the tab bar */
  QIcon iconOk, iconNo, iconFlag;

  /** should the tab bar be displayed even if there is only one connection? */
  bool wantTabBar;
};

cSessionManager *cSessionManager::_self = nullptr;

cSessionManager::cSessionManager () : cActionBase ("session-manager", 0)
{
  d = new cSessionManagerPrivate;
  d->widget = nullptr;
  cActionManager::self()->setActiveSession (0);
  d->am = cActionManager::self();

  //icons for tabs
  d->iconOk = QIcon::fromTheme ("dialog-ok");
  d->iconNo = QIcon::fromTheme ("dialog-cancel");
  d->iconFlag = QIcon::fromTheme ("flag");

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cSessionManager::~cSessionManager ()
{
  removeGlobalEventHandler ("global-settings-changed");
  delete d;
}

cSessionManager *cSessionManager::self()
{
  if (!_self) _self = new cSessionManager;
  return _self;
}

void cSessionManager::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    setAlwaysTabBar (cGlobalSettings::self()->getBool ("always-tab-bar"));
  }
}

int cSessionManager::count ()
{
  return d->am->sessions();
}

void cSessionManager::setMainWidget (cTabWidget *widget)
{
  d->widget = widget;
}

int cSessionManager::activeSession ()
{
  return cActionManager::self()->activeSession();
}

int cSessionManager::getSessionByTab (int tab)
{
  cSession *sess = (cSession *) d->widget->widget (tab);
  if (!sess) return -1;
  return sess->sess();
}

int cSessionManager::getTabBySession (int sess)
{
  if (!d->am->sessionExists (sess)) return -1;
  cSession *s = dynamic_cast<cSession *>(d->am->object ("session", sess));
  if (!s) return -1;
  return d->widget->indexOf (s);
}

int cSessionManager::addSession (bool profile)
{
  if (KMuddy::isGoingDown())
    return 0;

  //first, one very special case - if we only have one session
  //and that one is not connected, we return that session
  //and don't add any new one. This session will be used for
  //the connection that is being established.
  if (count() == 1)
  {
    int which = d->am->sessionList().front();
    cTelnet *telnet = dynamic_cast<cTelnet *>(d->am->object ("telnet", which));
    if (!telnet->isConnected())
    {
      cActionManager::self()->setSessionAttrib (which, "profile", profile?1:0);
      cGlobalSettings::self()->notifyChange ();
      return which;
    }
    //else: continue
  }

  //now for the standard operation

  //find a first free session ID for the connection
  int which = 1;
  while (true) {
    if (!d->am->sessionExists (which)) break;
    ++which;
  }

  d->am->registerSession (which);
  cActionManager::self()->setSessionAttrib (which, "profile", profile?1:0);
  cSession *session = new cSession (which, d->widget);
  d->widget->addTab (session, i18n ("No connection"));
  setIcon (which, IconNo);

  // inform everyone
  d->am->invokeEvent ("session-created", 0, which);
  d->am->invokeEvent ("created", which);
  
  setSession (which);
  //just to be sure...
  cActionManager::self()->setActiveSession (which);

  (KApplication::kApplication())->processEvents();

  //apply global settings to the newly created session
  //this also shows/hides the tabbar if needed
  cGlobalSettings::self()->notifyChange ();

  return which;
}

bool cSessionManager::removeSession (int which, bool dontClose)
{
  if (KMuddy::isGoingDown())
    return false;

  //reconnect shortcut
  QString reconnectText = KMuddy::self()->reconnectText ();
  
  if (!d->am->sessionExists (which))
    return false;

  //now the special case - only one session; we do almost nothing!
  if (count() == 1)
  {
    setSessionName (which, i18n ("No connection"));
    //no connection...
    setIcon (which, IconNo);
    cConnection *connection = dynamic_cast<cConnection *>(d->am->object ("connection", which));
    connection->setConnectionClosed (false);
    connection->updateMenus ();
    d->am->invokeEvent ("message", which, reconnectText);
    if (!d->wantTabBar)
      d->widget->hideTabBar ();
    return true;
  }

  if (!dontClose)
  {
    //inform everyone
    d->am->invokeEvent ("session-destroyed", 0, which);
    d->am->invokeEvent ("destroyed", which);
    //remove widget
    d->widget->removeTab (getTabBySession (which));
    cSession *sess = dynamic_cast<cSession *>(d->am->object ("session", which));
    delete sess;
    d->am->unregisterSession (which);

    //hide the tabbar if needed
    if ((!d->wantTabBar) && (count() <= 1))
      d->widget->hideTabBar ();
  }
  else
  {
    setSessionName (which, i18n ("Connection closed."));
    setIcon (which, IconNo);
    cConnection *connection = dynamic_cast<cConnection *>(d->am->object ("connection", which));
    connection->setConnectionClosed (true);
    connection->updateMenus ();
    d->am->invokeEvent ("message", which, reconnectText);
  }

  return true;
}

void cSessionManager::setSession (int which)
{
  if (!d->am->sessionExists (which))
    return;
  //this also invokes changeSession, which updates active session
  d->widget->setCurrentIndex (getTabBySession (which));
}

void cSessionManager::changeSession (int which)
{
  d->am->invokeEvent ("deactivated", which);
  d->am->invokeEvent ("session-deactivated", 0, which);

  cActionManager::self()->setActiveSession (which);

  //disable flashing (if enabled)
  cSession *sess = dynamic_cast<cSession *>(d->am->object ("session", which));
  if (sess && sess->flashing())
  {
    sess->setFlashing (false);

    //if flashing was on, icon could be wrong - fix it
    cTelnet *telnet = dynamic_cast<cTelnet *>(d->am->object ("telnet", which));
    setIcon (which, telnet->isConnected () ? IconOk : IconNo);
  }

  d->am->invokeEvent ("session-activated", 0, which);
  d->am->invokeEvent ("activated", which);

  //global caption
  int tab = getTabBySession (which);
  KMuddy::self()->setPlainCaption (KDialog::makeStandardCaption (d->widget->tabText (tab)));

  //update menus (active items and such)
  int s = activeSession();
  cConnection *connection = dynamic_cast<cConnection *>(d->am->object ("connection", s));
  if (!connection) return; // session not initialised yet
  connection->updateMenus ();

  // update windows
  KMuddy::self()->updateWindows ();

  //give focus to the input line
  //cInputLine::focus[In/Out]Event also restores selections :)
  cInputLine *inputline = dynamic_cast<cInputLine *>(d->am->object ("inputline", s));
  inputline->setFocus ();
}

void cSessionManager::setSessionName (int which, QString name, bool defName)
{
  cSession *sess = dynamic_cast<cSession *>(d->am->object ("session", which));
  sess->setName (name, defName);
  //update the name - maybe it wasn't changed?
  name = sess->name();
  int tab = getTabBySession (which);
  d->widget->setTabText (tab, name);
  //change icon to Ok (we'll change it later if it's not correct)
  setIcon (which, IconOk);
  if (which == activeSession ())
    KMuddy::self()->setPlainCaption (KDialog::makeStandardCaption (name));
}

bool cSessionManager::alwaysTabBar ()
{
  return d->wantTabBar;
}

void cSessionManager::setAlwaysTabBar (bool value)
{
  d->wantTabBar = value;
  if (d->widget == nullptr)
    return;
  bool show = true;
  if ((!value) && (count() == 1)) show = false;
  show ? d->widget->showTabBar() : d->widget->hideTabBar();
}

void cSessionManager::setIcon (int sess, ProfileIcon icon)
{
  int tab = getTabBySession (sess);
  QIcon i;
  switch (icon) {
    case IconOk: i = d->iconOk; break;
    case IconNo: i = d->iconNo; break;
    case IconFlag: i = d->iconFlag; break;
  }
  d->widget->setTabIcon (tab, i);
}

void cSessionManager::setNotifyFlag (int sess)
{
  cSession *session = dynamic_cast<cSession *>(d->am->object ("session", sess));
  session->setFlashing (true);

  cConnection *connection = dynamic_cast<cConnection *>(d->am->object ("connection", sess));
  bool connClosed = connection->connectionClosed();
  setIcon (sess, connClosed ? IconNo : IconFlag);
}


