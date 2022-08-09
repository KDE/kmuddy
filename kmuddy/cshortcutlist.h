/***************************************************************************
                          cshortcutlist.h  -  macro key list
                             -------------------
    begin                : St máj 28 2003
    copyright            : (C) 2003-2008 by Tomas Mecir
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

#ifndef CSHORTCUTLIST_H
#define CSHORTCUTLIST_H

#include "clist.h"

/**
List of macro keys/shortcuts

  *@author Tomas Mecir
  */

class cShortcutList : public cList {
public: 
  cShortcutList ();
  ~cShortcutList () override;
  
  static cList *newList () { return new cShortcutList; };
  cListObject *newObject () override;
  QString objName () override { return "Macro key"; }
  cListEditor *editor (QWidget *parent) override;

  /** this key was pressed - send a command if needed, returns true if
  some shortcut matched the key */
  bool handleKey (int key, Qt::KeyboardModifiers state);
protected:
  friend class cShortcut;
  bool matched;
  int currentKey;
  Qt::KeyboardModifiers currentModifiers;
};

#endif
