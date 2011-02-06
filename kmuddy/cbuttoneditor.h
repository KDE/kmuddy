//
// C++ Interface: cbuttoneditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CBUTTONEDITOR_H
#define CBUTTONEDITOR_H

#include <clisteditor.h>


class cButtonEditor : public cListEditor {
 Q_OBJECT
 public:
  cButtonEditor (QWidget *parent);
  ~cButtonEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected:
  struct Private;
  Private *d;
};

#endif  // CBUTTONEDITOR_H
