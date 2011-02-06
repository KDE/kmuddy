//
// C++ Interface: cstatusvar
//
// Description: One variable entry in the status bar
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
