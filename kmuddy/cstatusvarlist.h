//
// C++ Interface: cstatusvarlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CSTATUSVARLIST_H
#define CSTATUSVARLIST_H

#include "clist.h"

class cStatusVar;

/**
This class holds a list of status vars.

@author Tomas Mecir
*/
class cStatusVarList : public cList
{
 public:
  cStatusVarList ();
  ~cStatusVarList ();
  
  static cList *newList () { return new cStatusVarList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Status variable"; }
  virtual cListEditor *editor (QWidget *parent);

  void variableChanged (const QString &varname);
 private:
  struct Private;
  Private *d;
  friend class cStatusVar;

  virtual void listLoaded ();

  QString variableName () const;

  void scheduleUpdate ();
  void updateStatusBar ();
  void addToStatusBar (const QString &text);
};

#endif
