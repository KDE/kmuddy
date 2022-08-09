/***************************************************************************
                          ctimer.h  -  description
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

#ifndef CTIMER_H
#define CTIMER_H

#include "clistobject.h"

/**
One timer.
  *@author Tomas Mecir
  */

class cTimer : public cListObject {
public:
  ~cTimer () override;

  /** Execute this timer now. */
  void execute ();
  /** How long till the next tick ? */
  int tickTimeout ();
protected:
  friend class cTimerList;
  cTimer (cList *list);

  void setEnabled (bool en = true) override;

  /** React on an attribute change by adjusting the visible name. */
  void attribChanged (const QString &name) override;

  void updateVisibleName() override;

#define TIMER_RESET 1
#define TIMER_TICK 2

  cList::TraverseAction traverse (int traversalType) override;

  virtual void tick (int tickTime);
  void resetTickCount ();
protected:
  struct Private;
  Private *d;
};

#endif
