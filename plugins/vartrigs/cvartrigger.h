//
// C++ Interface: cvartrigger
//
// Description: One variable trigger.
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

#ifndef CVARTRIGGER_H
#define CVARTRIGGER_H

#include "clistobject.h"

/**
This class represents one variable trigger.


@author Tomas Mecir
*/
class cVarTrigger : public cListObject
{
public:
  ~cVarTrigger () override;

protected:
  friend class cVarTriggerList;
  cVarTrigger (cList *list);

  /** React on an attribute change. */
  void attribChanged (const QString &name) override;

  void updateVisibleName() override;

#define VARTRIGGER_MATCH 1

  cList::TraverseAction traverse (int traversalType) override;

  virtual void executeCommands ();

  struct Private;
  Private *d;
};

#endif
