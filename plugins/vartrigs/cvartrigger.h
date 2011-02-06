//
// C++ Interface: cvartrigger
//
// Description: One variable trigger.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  virtual ~cVarTrigger ();

protected:
  friend class cVarTriggerList;
  cVarTrigger (cList *list);

  /** React on an attribute change. */
  virtual void attribChanged (const QString &name);

  virtual void updateVisibleName();

#define VARTRIGGER_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType);

  virtual void executeCommands ();

  struct Private;
  Private *d;
};

#endif
