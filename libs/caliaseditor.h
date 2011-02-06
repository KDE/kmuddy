//
// C++ Interface: caliaseditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CALIASEDITOR_H
#define CALIASEDITOR_H

#include <clisteditor.h>

#include <kmuddy_export.h>

class KMUDDY_EXPORT cAliasEditor : public cListEditor {
 Q_OBJECT
 public:
  cAliasEditor (QWidget *parent);
  ~cAliasEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected slots:
  void updateEditButton (const QString &comboText);
  void editRegExp ();
  void updateTest (const QString &);
  void updateTest (bool);
  void updateTest ();
 protected:
  struct Private;
  Private *d;
};

#endif  // CALIASEDITOR_H
