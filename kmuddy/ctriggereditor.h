//
// C++ Interface: ctriggereditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CTRIGGEREDITOR_H
#define CTRIGGEREDITOR_H

#include <clisteditor.h>

class cTriggerEditor : public cListEditor {
 Q_OBJECT
 public:
  cTriggerEditor (QWidget *parent);
  ~cTriggerEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected slots:
  void updateEditButton (const QString &comboText);
  void editRegExp ();
  void updateTest (const QString &);
  void updateTest (bool);
  void updateTest ();

  void addColorization ();
  void removeColorization ();
  void updateColorizationsList ();

  void browseForSoundFile ();
  void createOutputWindow ();
  void rewriteChanged (bool val);
 protected:
  struct Private;
  Private *d;
};

#endif  // CTRIGGEREDITOR_H
