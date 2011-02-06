//
// C++ Interface: dlgmudlist
//
// Description: A dialog displaying the list of MUDs from the MUD Connector database.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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

