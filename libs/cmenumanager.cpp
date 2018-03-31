//
// C++ Implementation: cMenuManager
//
// Description: Menu manager.
//
/*
Copyright 2006-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cmenumanager.h"

#include <QAction>
#include <QMenuBar>
#include <QMenu>

#include <list>
#include <map>

using namespace std;

struct cMenuManagerPrivate {
  QMenuBar *menuBar;
  map<QString, QAction *> positions;  // where to place individual items
  map<QString, QMenu *> locations;    // which position is in which menu
  map<QMenu *, QString> lastPositionInMenu;
  map<QMenu *, QAction *> menuActions;
  map<QAction *, QMenu *> actionLocations;
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

void cMenuManager::setMenuBar (QMenuBar *menuBar)
{
  d->menuBar = menuBar;
}

void cMenuManager::addMenuPosition (const QString &name)
{
  addItemPosition (name, 0);
}

void cMenuManager::addItemPosition (const QString &name, QMenu *menu)
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

void cMenuManager::addMenu (QMenu *menu, const QString &label, const QString &position)
{
  // nothing if there's no such position
  if (!d->positions.count (position)) return;
  
  // plug the actual item
  QAction *before = d->positions[position];
  QMenu *inMenu = d->locations[position];
  if (inMenu) return;
  if (before)
    d->menuBar->insertMenu (before, menu);
  else
    d->menuBar->addMenu (menu);
  menu->setTitle (label);
}

void cMenuManager::removeMenu (QMenu *menu)
{
  d->menuBar->removeAction (d->menuActions[menu]);
  d->menuActions.erase (menu);
  d->lastPositionInMenu.erase (menu);

  // remove all positions tied to this menu
  QStringList pl;
  map<QString, QMenu *>::iterator it;
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
  QMenu *menu = d->locations[position];
  if (!menu) return;
  if (before)
    menu->insertAction (before, action);
  else
    menu->addAction (action);
  d->actionLocations[action] = menu;
}

void cMenuManager::unplug (QAction *action)
{
  QMenu *menu = d->actionLocations[action];
  if (menu) 
    menu->removeAction (action);
  d->actionLocations.erase (action);
}


