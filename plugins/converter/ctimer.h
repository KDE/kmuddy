/***************************************************************************
                          ctimer.h  -  description
                             -------------------
    begin                : St maj 8 2003
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

#ifndef CTIMER_H
#define CTIMER_H

#include "csaveablefield.h"

/**
One timer.
  *@author Tomas Mecir
  */

class cTimer : public cSaveableField  {
public:
  cTimer (int _sess);
  ~cTimer () override;

  /** creates a new instance of the class */
  cSaveableField *newInstance () override;

  /** load data from a config file*/
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_TIMER; };

  void setInterval (int what);
  int interval () { return _interval; };
  void setSingleShot (bool what) { _singleshot = what; };
  bool singleShot () { return _singleshot; };
  void setActive (bool what) { _active = what; };
  bool active () { return _active; };
  void setCommand (const QString &what) { _command = what; };
  const QString &command () { return _command; };
protected:
  int _interval;
 
  bool _singleshot, _active;
  QString _command;

  int sess;
};

#endif
