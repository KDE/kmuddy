//
// C++ Interface: cdialoglist
//
// Description: List of dialogs.
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CDIALOGLIST_H
#define CDIALOGLIST_H

#include "cactionbase.h"
#include <map>

#include <kmuddy_export.h>

class QDialog;

using namespace std;

/**
This class stores dialog boxes. Its purpose is to allow plug-ins to extend these dialogs in various ways.

	@author Tomas Mecir <kmuddy@kmuddy.com>
*/
class KMUDDY_EXPORT cDialogList : public cActionBase {
 public:
  static cDialogList *self ();
  ~cDialogList() override;
  QDialog *getDialog (const QString &name);
  void addDialog (const QString &name, QDialog *dlg);
  void removeDialog (const QString &name);
 private:
  cDialogList();
  static cDialogList *_self;
  map<QString, QDialog *> dialogs;
};

#endif
