//
// C++ Interface: cgenericlist
//
// Description: A generic list that can only store data.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CGENERICLIST_H
#define CGENERICLIST_H

#include <clist.h>

class cGenericList : public cList {
 public:
  cGenericList ();
  ~cGenericList ();

  static cList *newList () { return new cGenericList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return QString(); }
  virtual cListEditor *editor (QWidget *) { return 0; };

  void init ();
  /** Save the generic list into a file - we cannot use cListManager here. */
  bool saveList (const QString &file);

  void intProperty (const QString &name, int defaultValue = 0);
  void stringProperty (const QString &name, QString defaultValue = QString());
  void boolProperty (const QString &name, bool defaultValue = false);
};

#endif  // CGENERICLIST_H
