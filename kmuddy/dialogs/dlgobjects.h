//
// C++ Interface: dlgobjects
//
// Description: Object Manager
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DLGOBJECTS_H
#define DLGOBJECTS_H

#include <kdialog.h>
#include "cactionbase.h"

class cListObject;

class dlgObjects: public KDialog, public cActionBase {
 Q_OBJECT
 public:
  dlgObjects (QWidget *parent = 0);
  ~dlgObjects ();

 private slots:
  void listChanged (int index);
  void activeObjectChanged (cListObject *obj);
  void saveChanges ();
 private:
  void eventNothingHandler (QString event, int session);
  void eventIntHandler (QString event, int, int par1, int);

  void switchList ();

  struct Private;
  Private *d;
};

#endif  // DLGOBJECTS_H
