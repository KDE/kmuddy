//
//
// C++ Interface: cprompt
//
// Description:
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CPROMPT_H
#define CPROMPT_H

#include <cactionbase.h>
#include <kmuddy_export.h>

#include <qlabel.h>

class KMUDDY_EXPORT cPrompt : public QLabel, public cActionBase {
   Q_OBJECT
 public:
  cPrompt (int sess, QWidget *parent);
  ~cPrompt();
  void updatePrompt (const QString &text);
 protected:
  virtual void eventStringHandler (QString event, int, QString &par1, const QString &);
  virtual void eventNothingHandler (QString event, int);

};

#endif
