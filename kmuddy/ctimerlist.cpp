/***************************************************************************
                          ctimerlist.cpp  -  list of timers
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

#include "ctimerlist.h"
#include "ctimer.h"
#include "ctimereditor.h"

#include <qtimer.h>

struct cTimerList::Private {
  int tickCount;
  QTimer *timer;
};

cTimerList::cTimerList () :
    cList ("timers")
{
  d = new Private;

  addIntProperty ("interval", "Execution interval", 60);
  addStringProperty ("command", "Command");
  addStringProperty ("command-advance", "Command executed several seconds before the timer ticks.", "/echo TIMER TICK IN 5 SECONDS");
  addBoolProperty ("advance", "Do we want to run a command in advance?", false);
  addIntProperty ("advance-seconds", "When to execute the advance command?", 5);
  // a single-shot timer disables itself after it fires
  addBoolProperty ("single-shot", "Single-shot timer", false);
  // script
  addStringProperty ("script", "Script to execute");

  d->tickCount = 0;
  d->timer = new QTimer;
  connect (d->timer, SIGNAL (timeout ()), this, SLOT (timeout ()));
  d->timer->start (1000);
}

cTimerList::~cTimerList ()
{
  delete d->timer;
  delete d;
}

cListObject *cTimerList::newObject ()
{
  return new cTimer (this);
}

cListEditor *cTimerList::editor (QWidget *parent)
{
  return new cTimerEditor (parent);
}

void cTimerList::setEnabled (bool en)
{
  cList::setEnabled (en);
  if (en) {
    //reset tick counts first
    d->tickCount = 0;
    traverse (TIMER_RESET);
    d->timer->start (1000);
  } else {
    d->timer->stop ();
  }
}

void cTimerList::timeout ()
{
  //update tick count
  d->tickCount++;
  //this does not check for overflow, therefore could stop working after
  //about 49 days on a 32-bit architecture

  // advance each timer, executing the command if needed
  traverse (TIMER_TICK);
}

int cTimerList::tickCount ()
{
  return d->tickCount;
}

