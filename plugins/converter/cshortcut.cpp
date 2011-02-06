/***************************************************************************
                          cshortcut.cpp  -  macro key/shortcut
                             -------------------
    begin                : St máj 28 2003
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

#include "cshortcut.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cShortcut::cShortcut (int _sess) : sess(_sess)
{
  _key = _state = 0;
  sendit = true;
  overwriteinput = false;
}

cShortcut::~cShortcut ()
{

}

cSaveableField *cShortcut::newInstance ()
{
  return new cShortcut (sess);
}

void cShortcut::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);

  setText (g.readEntry ("Text", ""));
  setKey (g.readEntry ("Key", 0));
  setState (g.readEntry ("State", 0));
  setSendIt (g.readEntry ("Send it", true));
  setOverwriteInput (g.readEntry ("Overwrite input", false));
}

