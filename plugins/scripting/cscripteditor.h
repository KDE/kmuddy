//
// C++ Interface: cscripteditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSCRIPTEDITOR_H
#define CSCRIPTEDITOR_H

#include <clisteditor.h>


class cScriptEditor : public cListEditor {
 Q_OBJECT
 public:
  cScriptEditor (QWidget *parent);
  ~cScriptEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected slots:
  void browse1 ();
  void browse2 ();
 protected:
  struct Private;
  Private *d;
};

#endif  // CSCRIPTEDITOR_H
