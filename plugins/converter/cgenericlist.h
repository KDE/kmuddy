//
// C++ Interface: cgenericlist
//
// Description: A generic list that can only store data.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CGENERICLIST_H
#define CGENERICLIST_H

#include <clist.h>

class cGenericList : public cList {
 public:
  cGenericList ();
  ~cGenericList ();

  static cList *newList () { return new cGenericList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return QString(); }
  virtual cListEditor *editor (QWidget *) { return nullptr; };

  void init ();
  /** Save the generic list into a file - we cannot use cListManager here. */
  bool saveList (const QString &file);

  void intProperty (const QString &name, int defaultValue = 0);
  void stringProperty (const QString &name, QString defaultValue = QString());
  void boolProperty (const QString &name, bool defaultValue = false);
};

#endif  // CGENERICLIST_H
