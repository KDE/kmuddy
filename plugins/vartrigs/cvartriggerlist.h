//
// C++ Interface: cvartriggerlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-208
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  ~cVarTriggerList ();

  static cList *newList () { return new cVarTriggerList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Variable trigger"; }
  virtual cListEditor *editor (QWidget *parent);
  
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
