//
// C++ Interface: calias
//
// Description: 
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CALIAS_H
#define CALIAS_H

#include <clistobject.h>
#include <kmuddy_export.h>

/**
This class represents one alias.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cAlias : public cListObject {
public:
  virtual ~cAlias ();

protected:
  friend class cAliasList;
  cAlias (cList *list);

  virtual void updateVisibleName () override;

  /** React on an attribute change by updating the pattern object. */
  virtual void attribChanged (const QString &name) override;

#define ALIAS_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType) override;

  /** Perform alias matching. */
  cList::TraverseAction doMatch ();

  bool testCondition ();

  /** Execute the alias. */
  void executeAlias ();

  struct Private;
  Private *d;
};

#endif
