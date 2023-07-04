/***************************************************************************
                          dlgrunninglist.cpp  -  Running Scripts dialog
                             -------------------
    begin                : So jan 18 2003
    copyright            : (C) 2003-2009 by Tomas Mecir
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

#include "dlgrunninglist.h"

#include "crunninglist.h"

#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>

#include <kaction.h>
#include <klocale.h>

dlgRunningList::dlgRunningList (QWidget *parent) : QDockWidget (parent)
{
  setWindowTitle (i18n ("Running scripts"));
    
  //main widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);

  setWidget (page);

  QLabel *label = new QLabel (i18n ("&Running scripts"), page);
  view = new QTreeView (page);
  view->setModel (0);
  view->setAllColumnsShowFocus (true);
  view->setRootIsDecorated (false);
  view->setUniformRowHeights (true);
  view->setContextMenuPolicy (Qt::ActionsContextMenu);
  label->setBuddy (view);

  // actions
  KAction *actTerminate = new KAction (view);
  actTerminate->setText (i18n ("&Terminate"));
  connect (actTerminate, SIGNAL (triggered()), this, SLOT (terminateScript()));
  view->addAction (actTerminate);
  KAction *actKill = new KAction (view);
  actKill->setText (i18n ("&Kill"));
  connect (actKill, SIGNAL (triggered()), this, SLOT (killScript()));
  view->addAction (actKill);

  layout->setSpacing (5);
  layout->addWidget (label);
  layout->addWidget (view);
}

dlgRunningList::~dlgRunningList()
{
  //nothing here
}

void dlgRunningList::switchRunningList (cRunningList *newlist)
{
  rlist = newlist;
  view->setModel (rlist ? rlist->getModel() : 0);
}

void dlgRunningList::terminateScript ()
{
  int id = selectedId ();
  if (id < 0) return;
  rlist->terminate (id);
}

void dlgRunningList::killScript ()
{
  int id = selectedId ();
  if (id < 0) return;
  rlist->kill (id);
}

int dlgRunningList::selectedId ()
{
  if (!rlist) return -1; //if there is no runninglist, there's nothing to do

  QModelIndex idx = view->currentIndex ();
  if (!idx.isValid()) return -1;  // something must be selected

  return rlist->getModel()->data (idx, Qt::UserRole).toInt();
}

#include "moc_dlgrunninglist.cpp"
