//
// C++ Interface: cstatusvar
//
// Description: One variable entry in the status bar
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

#ifndef CSTATUSVAR_H
#define CSTATUSVAR_H

#include "clistobject.h"

/**
This class represents one variable entry in the status bar.

@author Tomas Mecir
*/
class cStatusVar : public cListObject
{
 public:
  /** destructor */
  ~cStatusVar();

#define STATUSVAR_MATCH 1
#define STATUSVAR_UPDATE 2
  
  virtual cList::TraverseAction traverse (int traversalType);

  virtual void attribChanged (const QString &name);

  virtual void updateVisibleName();

  void updateBar ();

 protected:
  friend class cStatusVarList;
  /** constructor */
  cStatusVar (cList *list);
 
  virtual void objectMoved ();
  virtual void objectDisabled ();
  virtual void objectEnabled ();
 
  struct Private;
  Private *d;
};

#endif
