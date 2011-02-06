//
// C++ Interface: cshortcuteditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSHORTCUTEDITOR_H
#define CSHORTCUTEDITOR_H

#include <clisteditor.h>


class cShortcutEditor : public cListEditor {
 Q_OBJECT
 public:
  cShortcutEditor (QWidget *parent);
  ~cShortcutEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected slots:
  void grabKey ();
 protected:
  struct Private;
  Private *d;
};

#endif  // CSHORTCUTEDITOR_H
