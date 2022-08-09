/***************************************************************************
                          dlgconnect.h  -  Connect dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Út Jul 23 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGCONNECT_H
#define DLGCONNECT_H

#include <QDialog>
#include <QItemSelection>
#include <QModelIndex>

class QTreeView;
class KPushButton;
class QCheckBox;
class QDialogButtonBox;

class dlgEditProfile;

/**
Connect dialog. Shown and handled by cConnection.
  *@author Tomas Mecir
  */

class dlgConnect : public QDialog {
Q_OBJECT
public:
  dlgConnect (QWidget *parent=nullptr);
  ~dlgConnect ();

  virtual QSize sizeHint() const override;
  QString selectedProfile ();
  bool sendNothing ();
  bool isOffLine ();
protected:
  QTreeView *lw;
  QCheckBox *chkSendNothing;
  QCheckBox *chkOffline;
  dlgEditProfile *mdlg;
  QDialogButtonBox *buttons;

  void updateProfileFromDialog (const QString &profile);
protected slots:
  void selectionChanged (const QItemSelection &);
  void doubleClicked (const QModelIndex &);
  
  void addPressed ();
  void modifyPressed ();
  void deletePressed ();
  void duplicatePressed ();

  void doAdd ();
  void doModify ();
};

#endif
