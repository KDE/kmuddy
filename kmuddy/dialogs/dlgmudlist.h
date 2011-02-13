//
// C++ Interface: dlgmudlist
//
// Description: A dialog displaying the list of MUDs from the MUD Connector database.
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

#ifndef DLGMUDLIST_H
#define DLGMUDLIST_H

#include <kdialog.h>
#include <QModelIndex>

class cMUDEntry;
class cMUDList;

class dlgMudList : public KDialog {
 Q_OBJECT
 public:
  static const cMUDEntry *getEntry (QWidget *parent);
 private slots:
  void currentChanged (const QModelIndex &index);
 private:
  dlgMudList (QWidget *parent);
  virtual ~dlgMudList ();
  const cMUDEntry *selectedEntry ();

  struct Private;
  Private *d;

  static cMUDList *lst;
};

#endif  // DLGMUDLIST_H

