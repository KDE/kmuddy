/***************************************************************************
                          cshortcutlist.cpp  -  macro key list
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

#include "cshortcutlist.h"

#include "cshortcut.h"
#include "cshortcuteditor.h"

cShortcutList::cShortcutList ():
    cList ("macrokeys")
{
  addStringProperty ("command", "Command");
  addIntProperty ("key", "Key");
  addIntProperty ("modifiers", "Keyboard modifiers");
  addBoolProperty ("send", "Send the command", true);
  addBoolProperty ("overwrite", "Overwrite existing text on the input line", false);
  // script
  addStringProperty ("script", "Script to execute");
}

cShortcutList::~cShortcutList ()
{
}

cListObject *cShortcutList::newObject ()
{
  return new cShortcut (this);
}

cListEditor *cShortcutList::editor (QWidget *parent)
{
  return new cShortcutEditor (parent);
}

bool cShortcutList::handleKey (int key, Qt::KeyboardModifiers state)
{
  matched = false;
  currentKey = key;
  currentModifiers = state & (Qt::KeyboardModifierMask | Qt::KeypadModifier);

  traverse (SHORTCUT_MATCH);

  return matched;
}

