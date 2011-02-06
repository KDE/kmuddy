//
// C++ Interface: cbuttonlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CBUTTONLIST_H
#define CBUTTONLIST_H

#include "clist.h"

#include <qstring.h>

class cButton;

/**
List of buttons.

@author Tomas Mecir
*/
class cButtonList : public cList
{
public:
  cButtonList ();
  ~cButtonList ();

  static cList *newList () { return new cButtonList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Button"; }
  virtual cListEditor *editor (QWidget *parent);
  
  void updateButtons ();
 private:
  bool loaded;
  friend class cButton;

  virtual void listLoaded ();
};

#endif  //CBUTTONLIST_H
