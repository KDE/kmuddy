/***************************************************************************
                          dlgrunninglist.h  -  Running Scripts dialog
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

#ifndef DLGRUNNINGLIST_H
#define DLGRUNNINGLIST_H

#include <QDockWidget>

class cRunningList;
class QTreeView;

/**
This dialog manages currently running scripts. It uses the cRunningList class.
  *@author Tomas Mecir
  */

class dlgRunningList : public QDockWidget {
   Q_OBJECT
public: 
  dlgRunningList (QWidget *parent);
  ~dlgRunningList ();

  /** use another running list */
  void switchRunningList (cRunningList *newlist);

protected slots:
  void terminateScript ();
  void killScript ();
protected:

  int selectedId ();

  cRunningList *rlist;
  QTreeView *view;
};

#endif
