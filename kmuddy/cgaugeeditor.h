//
// C++ Interface: cgaugeeditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CGAUGEEDITOR_H
#define CGAUGEEDITOR_H

#include <clisteditor.h>


class cGaugeEditor : public cListEditor {
 Q_OBJECT
 public:
  cGaugeEditor (QWidget *parent);
  ~cGaugeEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected:
  struct Private;
  Private *d;
};

#endif  // CGAUGEEDITOR_H
