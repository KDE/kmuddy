/***************************************************************************
                          ctimer.cpp  -  description
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

#include "ctimer.h"

#include "cactionmanager.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cTimer::cTimer (int _sess) : sess(_sess)
{
  _interval = 60;
  _singleshot = false;
  _active = false;
  _command = QString();

}

cTimer::~cTimer ()
{

}

cSaveableField *cTimer::newInstance ()
{
  return new cTimer (sess);
}

void cTimer::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);

  setInterval (g.readEntry ("Interval", 60));
  setSingleShot (g.readEntry ("Single shot", false));
  setActive (g.readEntry ("Active", false));
  setCommand (g.readEntry ("Command", QString()));
}

void cTimer::setInterval (int what)
{
  if (what >= 1)  //only positive intervals are allowed
    _interval = what;
}

