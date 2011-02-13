//
// C++ Interface: dlgobjects
//
// Description: Object Manager
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef DLGOBJECTS_H
#define DLGOBJECTS_H

#include <kdialog.h>
#include "cactionbase.h"

class cListObject;

class dlgObjects: public KDialog, public cActionBase {
 Q_OBJECT
 public:
  dlgObjects (QWidget *parent = 0);
  ~dlgObjects ();

 private slots:
  void listChanged (int index);
  void activeObjectChanged (cListObject *obj);
  void saveChanges ();
 private:
  void eventNothingHandler (QString event, int session);
  void eventIntHandler (QString event, int, int par1, int);

  void switchList ();

  struct Private;
  Private *d;
};

#endif  // DLGOBJECTS_H
