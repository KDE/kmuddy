//
// C++ Implementation: cdialoglist
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

#include "cdialoglist.h"

cDialogList *cDialogList::_self = 0;

cDialogList::cDialogList() : cActionBase ("dialog-list", 0)
{
}

cDialogList::~cDialogList()
{
}

cDialogList *cDialogList::self ()
{
  if (!_self)
    _self = new cDialogList();
  return _self;
}

QDialog *cDialogList::getDialog (const QString &name) {
  if (dialogs.count (name))
    return dialogs[name];
  return 0;
}

void cDialogList::addDialog (const QString &name, QDialog *dlg) {
  dialogs[name] = dlg;
}

void cDialogList::removeDialog (const QString &name) {
  dialogs.erase (name);
}


