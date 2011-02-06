//
// C++ Implementation: cMenuManager
//
// Description: Menu manager.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2006-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmenumanager.h"

#include <qaction.h>
#include <kmenubar.h>
#include <kmenu.h>

#include <list>
#include <map>

using namespace std;

struct cMenuManagerPrivate {
  KMenuBar *menuBar;
  map<QString, QAction *> positions;  // where to place individual items
  map<QString, KMenu *> locations;    // which position is in which menu
  map<KMenu *, QString> lastPositionInMenu;
  map<KMenu *, QAction *> menuActions;
  map<QAction *, KMenu *> actionLocations;
};

cMenuManager *cMenuManager::_self = 0;

cMenuManager::cMenuManager () : cActionBase ("menumanager", 0)
{
  d = new cMenuManagerPrivate;
  d->menuBar = 0;
}

cMenuManager::~cMenuManager ()
{
  delete d;
}

cMenuManager *cMenuManager::self ()
{
  if (!_self) _self = new cMenuManager;
  return _self;
}

void cMenuManager::setMenuBar (KMenuBar *menuBar)
{
  d->menuBar = menuBar;
}

void cMenuManager::addMenuPosition (const QString &name)
{
  addItemPosition (name, 0);
}

void cMenuManager::addItemPosition (const QString &name, KMenu *menu)
{
  if (d->positions.count (name)) return;
  
  // we are now the last position without a separator
  // so assign a separator to the previously old position
  if (d->lastPositionInMenu.count (menu)) {
    QString last = d->lastPositionInMenu[menu];
    d->positions[last] = menu ? menu->addSeparator () : d->menuBar->addSeparator();
  }
  d->positions[name] = 0;
  d->lastPositionInMenu[menu] = name;
  d->locations[name] = menu;
}

void cMenuManager::addMenu (KMenu *menu, const QString &label, const QString &position)
{
  // nothing if there's no such position
  if (!d->positions.count (position)) return;
  
  // plug the actual item
  QAction *before = d->positions[position];
  KMenu *inMenu = d->locations[position];
  if (inMenu) return;
  if (before)
    d->menuBar->insertMenu (before, menu);
  else
    d->menuBar->addMenu (menu);
  menu->setTitle (label);
}

void cMenuManager::removeMenu (KMenu *menu)
{
  d->menuBar->removeAction (d->menuActions[menu]);
  d->menuActions.erase (menu);
  d->lastPositionInMenu.erase (menu);

  // remove all positions tied to this menu
  QStringList pl;
  map<QString, KMenu *>::iterator it;
  for (it = d->locations.begin(); it != d->locations.end(); ++it)
    if (it->second == menu)
      pl << it->first;
  QStringList::iterator ii;
  for (ii = pl.begin(); ii != pl.end(); ++ii)
  {
    d->positions.erase (*ii);
    d->locations.erase (*ii);
  }
}

void cMenuManager::plug (QAction *action, QString position)
{
  // nothing if there's no such position
  if (!d->positions.count (position)) return;
  
  // plug the actual item
  QAction *before = d->positions[position];
  KMenu *menu = d->locations[position];
  if (!menu) return;
  if (before)
    menu->insertAction (before, action);
  else
    menu->addAction (action);
  d->actionLocations[action] = menu;
}

void cMenuManager::unplug (QAction *action)
{
  KMenu *menu = d->actionLocations[action];
  if (menu) 
    menu->removeAction (action);
  d->actionLocations.erase (action);
}


