//
// C++ Interface: cstatusvareditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSTATUSVAREDITOR_H
#define CSTATUSVAREDITOR_H

#include <clisteditor.h>


class cStatusVarEditor : public cListEditor {
 Q_OBJECT
 public:
  cStatusVarEditor (QWidget *parent);
  ~cStatusVarEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected:
  struct Private;
  Private *d;
};

#endif  // CSTATUSVAREDITOR_H
