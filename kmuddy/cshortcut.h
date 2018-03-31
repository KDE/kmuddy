/***************************************************************************
                          cshortcut.h  -  macro key/shortcut
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

#ifndef CSHORTCUT_H
#define CSHORTCUT_H

#include "clistobject.h"

/**
Represents one macro key/shortcut.
  *@author Tomas Mecir
  */

class cShortcut : public cListObject  {
 public: 
  virtual ~cShortcut ();

  virtual void attribChanged (const QString &name) override;

  virtual void updateVisibleName() override;

#define SHORTCUT_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType) override;
  
  static QString keyToString (int _key, int _state);
 private:
  friend class cShortcutList;
  cShortcut (cList *list);

  struct Private;
  Private *d;
};

#endif
