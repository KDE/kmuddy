//
// C++ Interface: cdialoglist
//
// Description: List of dialogs.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CDIALOGLIST_H
#define CDIALOGLIST_H

#include "cactionbase.h"
#include <map>

#include <kmuddy_export.h>

class KDialog;

using namespace std;

/**
This class stores dialog boxes. Its purpose is to allow plug-ins to extend these dialogs in various ways.

	@author Tomas Mecir <kmuddy@kmuddy.com>
*/
class KMUDDY_EXPORT cDialogList : public cActionBase {
 public:
  static cDialogList *self ();
  ~cDialogList();
  KDialog *getDialog (const QString &name);
  void addDialog (const QString &name, KDialog *dlg);
  void removeDialog (const QString &name);
 private:
  cDialogList();
  static cDialogList *_self;
  map<QString, KDialog *> dialogs;
};

#endif
