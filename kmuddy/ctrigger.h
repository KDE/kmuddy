//
// C++ Interface: ctrigger
//
// Description: one trigger
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

#ifndef CTRIGGER_H
#define CTRIGGER_H

class cTriggerList;

#include <clistobject.h>

#include <qcolor.h>

#include <map>

using namespace std;

/**
One trigger.
  *@author Tomas Mecir
  */

class cTrigger : public cListObject {
public:
  virtual ~cTrigger ();

protected:
  friend class cTriggerList;
  cTrigger (cList *list);

  /** React on an attribute change by updating the pattern object. */
  virtual void attribChanged (const QString &name) override;

  virtual void updateVisibleName() override;

#define TRIGGER_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType) override;

  /** Perform trigger matching. */
  cList::TraverseAction doMatch ();

  bool testCondition ();

  /** Execute the trigger. */
  void executeTrigger ();

  // TODO: methods to manipulate colorizations ? Would be used by scripting ...

  /** compute new colors and recolorize the line */
  void recolorize ();
  /** replace old text with new text */
  void rewrite ();

  struct Private;
  Private *d;
};

#endif  // CTRIGGER_H

