/***************************************************************************
                          cdirlist.h  -  pick a list of directories
                             -------------------
    begin                : So apr 12 2003
    copyright            : (C) 2003 by Tomas Mecir
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

#ifndef CDIRLIST_H
#define CDIRLIST_H

#include <qstringlist.h>
#include <QWidget>

class QListWidget;
class QPushButton;

/**
A widget that allows you to choose a list of directories.
  *@author Tomas Mecir
  */

class cDirList : public QWidget {
   Q_OBJECT
public: 
  cDirList (QWidget *parent=nullptr);
  ~cDirList () override;
  const QStringList &getDirList () { return dirlist; };
  void setDirList (const QStringList &dlist);
protected slots:
  void addEntry ();
  void removeEntry ();
  void moveEntryUp ();
  void moveEntryDown ();
protected:
  /** update listbox */
  void update ();
  
  QListWidget *listbox;
  QPushButton *btadd, *btdel, *btup, *btdown;
  QStringList dirlist;
};

#endif
