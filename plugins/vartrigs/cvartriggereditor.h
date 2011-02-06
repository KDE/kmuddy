//
// C++ Interface: cvartriggereditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CVARTRIGGEREDITOR_H
#define CVARTRIGGEREDITOR_H

#include <clisteditor.h>


class cVarTriggerEditor : public cListEditor {
 Q_OBJECT
 public:
  cVarTriggerEditor (QWidget *parent);
  ~cVarTriggerEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected:
  struct Private;
  Private *d;
};

#endif  // CVARTRIGGEREDITOR_H
