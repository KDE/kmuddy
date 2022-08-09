/***************************************************************************
                          ctimerlist.h  -  list of timers
                             -------------------
    begin                : St maj 8 2003
    copyright            : (C) 2003-2008 by Tomas Mecir
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

#ifndef CTIMERLIST_H
#define CTIMERLIST_H

#include "clist.h"
#include <QObject>

/**
List of timers.
  *@author Tomas Mecir
  */

class cTimerList : public QObject, public cList {
   Q_OBJECT
public:
  cTimerList ();
  ~cTimerList () override;

  static cList *newList () { return new cTimerList; };
  cListObject *newObject () override;
  QString objName () override { return "Timer"; }
  cListEditor *editor (QWidget *parent) override;

  void setEnabled (bool en = true) override;
protected slots:
  void timeout ();
protected:
  friend class cTimer;

  int tickCount ();

  struct Private;
  Private *d;
};

#endif
