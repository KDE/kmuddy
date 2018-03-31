//
// C++ Interface: cstatusvarlist
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CSTATUSVARLIST_H
#define CSTATUSVARLIST_H

#include "clist.h"

class cStatusVar;

/**
This class holds a list of status vars.

@author Tomas Mecir
*/
class cStatusVarList : public cList
{
 public:
  cStatusVarList ();
  ~cStatusVarList ();
  
  static cList *newList () { return new cStatusVarList; };
  virtual cListObject *newObject () override;
  virtual QString objName () override { return "Status variable"; }
  virtual cListEditor *editor (QWidget *parent) override;

  void variableChanged (const QString &varname);
 private:
  struct Private;
  Private *d;
  friend class cStatusVar;

  virtual void listLoaded () override;

  QString variableName () const;

  void scheduleUpdate ();
  void updateStatusBar ();
  void addToStatusBar (const QString &text);
};

#endif
