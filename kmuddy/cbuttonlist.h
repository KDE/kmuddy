//
// C++ Interface: cbuttonlist
//
// Description: 
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

#ifndef CBUTTONLIST_H
#define CBUTTONLIST_H

#include "clist.h"

#include <qstring.h>

class cButton;

/**
List of buttons.

@author Tomas Mecir
*/
class cButtonList : public cList
{
public:
  cButtonList ();
  ~cButtonList ();

  static cList *newList () { return new cButtonList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Button"; }
  virtual cListEditor *editor (QWidget *parent);
  
  void updateButtons ();
 private:
  bool loaded;
  friend class cButton;

  virtual void listLoaded ();
};

#endif  //CBUTTONLIST_H
