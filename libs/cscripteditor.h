//
// C++ Interface: cscripteditor
//
// Description: Script editor - used in dialogs
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef CSCRIPTEDITOR_H
#define CSCRIPTEDITOR_H

#include <QWidget>
#include <kmuddy_export.h>

class KMUDDY_EXPORT cScriptEditor : public QWidget {
Q_OBJECT
 public:
  cScriptEditor (QWidget *parent);
  ~cScriptEditor ();
  void setText (const QString &text);
  QString text() const;
 protected slots:
  void timedCheckScript ();
  void checkScript ();
 private:
  struct Private;
  Private *d;
};

#endif   // CSCRIPTEDITOR_H
