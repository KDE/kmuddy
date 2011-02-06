//
// C++ Implementation: cdialoglist
//
// Description: List of dialogs.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

KDialog *cDialogList::getDialog (const QString &name) {
  if (dialogs.count (name))
    return dialogs[name];
  return 0;
}

void cDialogList::addDialog (const QString &name, KDialog *dlg) {
  dialogs[name] = dlg;
}

void cDialogList::removeDialog (const QString &name) {
  dialogs.erase (name);
}


