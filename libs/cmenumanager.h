//
// C++ Interface: cMenuManager
//
// Description: Menu manager.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CMENUMANAGER_H
#define CMENUMANAGER_H

#include <cactionbase.h>
#include <kmuddy_export.h>

struct cMenuManagerPrivate;

class KMenu;
class QAction;
class KMenuBar;

/**
Menu manager. It manages the menubar, allowing plug-ins to insert items into the menu.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cMenuManager : public cActionBase
{
public:
  static cMenuManager *self ();
  ~cMenuManager ();

  /** assign a menubar to the object. It can't work without having one. */
  void setMenuBar (KMenuBar *menuBar);

  /** create a position for menus */
  void addMenuPosition (const QString &name);
  /** create a position for menu items */
  void addItemPosition (const QString &name, KMenu *menu);
  
  /** add a new menu */
  void addMenu (KMenu *menu, const QString &label, const QString &position);
  /** remove an existing menu */
  void removeMenu (KMenu *menu);
  
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
