//
// C++ Interface: calias
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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

  virtual void updateVisibleName ();

  /** React on an attribute change by updating the pattern object. */
  virtual void attribChanged (const QString &name);

#define ALIAS_MATCH 1

  virtual cList::TraverseAction traverse (int traversalType);

  /** Perform alias matching. */
  cList::TraverseAction doMatch ();

  bool testCondition ();

  /** Execute the alias. */
  void executeAlias ();

  struct Private;
  Private *d;
};

#endif
