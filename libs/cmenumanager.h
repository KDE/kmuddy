//
// C++ Interface: cMenuManager
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

#ifndef CMENUMANAGER_H
#define CMENUMANAGER_H

#include <cactionbase.h>
#include <kmuddy_export.h>

struct cMenuManagerPrivate;

class QMenu;
class QAction;
class QMenuBar;

/**
Menu manager. It manages the menubar, allowing plug-ins to insert items into the menu.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cMenuManager : public cActionBase
{
public:
  static cMenuManager *self ();
  ~cMenuManager () override;

  /** assign a menubar to the object. It can't work without having one. */
  void setMenuBar (QMenuBar *menuBar);

  /** create a position for menus */
  void addMenuPosition (const QString &name);
  /** create a position for menu items */
  void addItemPosition (const QString &name, QMenu *menu);
  
  /** add a new menu */
  void addMenu (QMenu *menu, const QString &label, const QString &position);
  /** remove an existing menu */
  void removeMenu (QMenu *menu);
  
  /** plug an action to a given position */
  void plug (QAction *action, QString position);
  /** unplug an action */
  void unplug (QAction *action);

protected:
  cMenuManager ();
  
  static cMenuManager *_self;
  
  cMenuManagerPrivate *d;
};

#endif
