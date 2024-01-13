/***************************************************************************
                          ctimer.cpp  -  description
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

#include "ctimer.h"

#include "cactionmanager.h"
#include "cscripteval.h"
#include "ctimerlist.h"

#include <KLocalizedString>

struct cTimer::Private {
  int nextLaunch;
};

cTimer::cTimer (cList *list) : cListObject (list)
{
  d = new Private;
  d->nextLaunch = -1;
}

cTimer::~cTimer ()
{
  delete d;
}

cList::TraverseAction cTimer::traverse (int traversalType)
{
  if (traversalType == TIMER_RESET) {
    resetTickCount();
    return cList::Continue;
  } else if (traversalType == TIMER_TICK) {
    cTimerList *tl = (cTimerList *) list();
    tick (tl->tickCount());
    return cList::Continue;
  } else
    return cList::Stop;
}

void cTimer::setEnabled (bool en)
{
  // if we are enabling/disabling the timer, we need to reset the nextLaunch
  if (en != enabled())
    d->nextLaunch = -1;
  cListObject::setEnabled (en);
}

void cTimer::attribChanged (const QString &name)
{
  if ((name == "command") || (name == "interval") || (name == "single-shot") || (name == "script"))
    updateVisibleName();
}

void cTimer::updateVisibleName()
{
  //FIXME: fix word puzzle
  QString command = strVal ("command");
  if (command.isEmpty() && (!strVal ("script").isEmpty())) command = "(script)";
  if (command.isEmpty()) {
    cListObject::updateVisibleName();
    return;
  }
  QString name = command;
  name += "(" + QString::number(intVal("interval")) + i18n(" sec");
  if (boolVal ("single-shot"))
    name += i18n(", single-shot");
  name += ")";
  setVisibleName (name);
}

void cTimer::tick (int tickTime)
{
  int interval = intVal ("interval");
  if (interval < 1)  //no interval set
    return;

  if (d->nextLaunch == -1) //no next-launch time set
  {
    //update next-launch time and return
    d->nextLaunch = tickTime + interval;
    return;
  }

  // do we want to run a command in advance ?
  if (boolVal ("advance")) {
    int secs = intVal ("advance-seconds");
    if (d->nextLaunch - tickTime == secs)
      cActionManager::self()->invokeEvent ("command", list()->session(), strVal ("command-advance"));
  }

  //check if we should execute now
  if (d->nextLaunch == tickTime)
    execute ();
}

void cTimer::execute ()
{
  cActionManager *am = cActionManager::self();
  int sess = list()->session();
  // send the command
  QString cmd = strVal ("command");
  if (!cmd.isEmpty())
    am->invokeEvent ("command", sess, cmd);
  // execute the script, if any
  QString script = strVal ("script");
  if (!script.isEmpty()) {
    cScriptEval *eval = dynamic_cast<cScriptEval *>(am->object ("scripteval", sess));
    if (eval) eval->eval (script);
  }


  int interval = intVal ("interval");
  int tickCount = ((cTimerList *) list())->tickCount();
  d->nextLaunch = tickCount + interval;
  //if we're a single-shot timer, we need to disable now
  if (boolVal ("single-shot"))
    setEnabled (false);
}

int cTimer::tickTimeout ()
{
  if (d->nextLaunch == -1) return intVal ("interval");

  cTimerList *tl = (cTimerList *) list();
  return d->nextLaunch - tl->tickCount();
}

void cTimer::resetTickCount ()
{
  d->nextLaunch = -1;
}

