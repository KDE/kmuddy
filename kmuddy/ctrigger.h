//
// C++ Interface: ctrigger
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
  virtual void attribChanged (const QString &name);

  virtual void updateVisibleName();

#define TRIGGER_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType);

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

