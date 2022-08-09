//
// C++ Interface: cvartriggerlist
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

#ifndef CVARTRIGGERLIST_H
#define CVARTRIGGERLIST_H

#include "clist.h"

/**
List of variable triggers.

@author Tomas Mecir
*/

class cVarTriggerList : public cList
{
 public:
  cVarTriggerList ();
  ~cVarTriggerList () override;

  static cList *newList () { return new cVarTriggerList; };
  cListObject *newObject () override;
  QString objName () override { return "Variable trigger"; }
  cListEditor *editor (QWidget *parent) override;
  
  /** execute appropriate variable trigger, if any */
  void variableChanged (const QString &varname);
 private:
  struct Private;
  Private *d;
  friend class cVarTrigger;

  QString variableName () const;

  void processCommands (const QStringList &commands);
};

#endif
