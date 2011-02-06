//
// C++ Interface: ctimereditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CTIMEREDITOR_H
#define CTIMEREDITOR_H

#include <clisteditor.h>


class cTimerEditor : public cListEditor {
 Q_OBJECT
 public:
  cTimerEditor (QWidget *parent);
  ~cTimerEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected:
  struct Private;
  Private *d;
};

#endif  // CTIMEREDITOR_H
