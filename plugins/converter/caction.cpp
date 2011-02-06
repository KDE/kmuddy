/***************************************************************************
                          caction.cpp  -  action toolbar item
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne nov 3 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#include "caction.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cAction::cAction (int _sess) : sess(_sess)
{
  caption = "";
  command = "";
  iconname = "";
  command2 = "";
  pushdown = false;
}

cAction::~cAction ()
{
}

cSaveableField *cAction::newInstance ()
{
  return new cAction (sess);
}

void cAction::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);
  caption = g.readEntry ("Caption", "cmd_placeholder");
  command = g.readEntry ("Command", "");
  iconname = g.readEntry ("Icon name", "unknown");
  pushdown = g.readEntry ("Pushdown", false);
  command2 = g.readEntry ("Command2", "");
  
}

void cAction::setCaption (const QString &s)
{
  caption = s;
}

QString cAction::getCaption ()
{
  return caption;
}

void cAction::setCommand (const QString &s)
{
  command = s;
}

void cAction::setCommand2 (const QString &s)
{
  command2 = s;
}

void cAction::setPushDown (bool how)
{
  pushdown = how;
}

bool cAction::isPushDown ()
{
  return pushdown;
}

QString cAction::getCommand ()
{
  return command;
}

QString cAction::getCommand2 ()
{
  return command2;
}

void cAction::setIconName (const QString &s)
{
  iconname = s;
}

QString cAction::getIconName ()
{
  return iconname;
}

