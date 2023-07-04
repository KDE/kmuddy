/***************************************************************************
                          cdirlist.cpp  -  pick a list of directories
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

#include "cdirlist.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QIcon>
#include <QListWidget>
#include <QPushButton>

#include <kapplication.h>
#include <klocale.h>

cDirList::cDirList (QWidget *parent) : QWidget(parent)
{
  //prepare layout
  QGridLayout *layout = new QGridLayout (this);

  //create all widgets
  listbox = new QListWidget (this);
  btadd = new QPushButton (i18n ("&Add..."), this);
  btdel = new QPushButton (i18n ("Delete"), this);
  btup = new QPushButton (QIcon::fromTheme ("go-up"), i18n ("Move &up"), this);
  btdown = new QPushButton (QIcon::fromTheme ("go-down"), i18n ("Move &down"), this);

  //add widgets to layout
  layout->setSpacing (10);
  layout->setRowStretch (4, 10);
  layout->addWidget (listbox, 0, 0, 5, 1);
  layout->addWidget (btadd, 0, 1);
  layout->addWidget (btdel, 1, 1);
  layout->addWidget (btup, 2, 1);
  layout->addWidget (btdown, 3, 1);
  
  //establish connections
  connect (btadd, SIGNAL (clicked ()), this, SLOT (addEntry ()));
  connect (btdel, SIGNAL (clicked ()), this, SLOT (removeEntry ()));
  connect (btup, SIGNAL (clicked ()), this, SLOT (moveEntryUp ()));
  connect (btdown, SIGNAL (clicked ()), this, SLOT (moveEntryDown ()));
  
  //update our contents
  update ();
}

cDirList::~cDirList ()
{

}

void cDirList::setDirList (const QStringList &dlist)
{
  dirlist = dlist;
  update ();
}

void cDirList::addEntry ()
{
  QString dirName = QFileDialog::getExistingDirectory (this, i18n ("Choose sound directory"));
  if (dirName.isEmpty()) return;

  dirlist.append (dirName);
  update();
}

void cDirList::removeEntry ()
{
  int item = listbox->currentRow ();
  if (item == -1)  //nothing selected
    return;
  //find it
  QStringList::iterator it = dirlist.begin() + item;
  //kill it
  dirlist.erase (it);
  update ();
}

void cDirList::moveEntryUp ()
{
  int item = listbox->currentRow ();
  if (item == -1)  //nothing selected
    return;
  if (item == 0)  //first item
    return;
  QStringList::iterator it = dirlist.begin() + item;
  QString text = *it;
  //it2 will point at previous item
  QStringList::iterator it2 = it;
  it2--;
  //erase that item
  dirlist.erase (it);
  //and insert it before previous item
  dirlist.insert (it2, text);
  update ();
  listbox->setCurrentRow (item - 1);
}

void cDirList::moveEntryDown ()
{
  int item = listbox->currentRow ();
  if (item == -1)  //nothing selected
    return;
  if (item == dirlist.count() - 1) //last item
    return;
  QStringList::iterator it = dirlist.begin() + item;
  QString text = *it;
  //erase that item, make it point to next item
  it = dirlist.erase (it);
  //and insert it after that item
  dirlist.insert (++it, text);
  update ();
  listbox->setCurrentRow (item + 1);
}

void cDirList::update ()
{
  int item = listbox->currentRow ();
  listbox->clear ();
  listbox->addItems (dirlist);
  if (item != -1)
    listbox->setCurrentRow (item);
}

#include "moc_cdirlist.cpp"
