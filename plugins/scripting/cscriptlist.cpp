/***************************************************************************
                          cscriptlist.cpp  -  list of scripts
                             -------------------
    begin                : Pi dec 13 2002
    copyright            : (C) 2002-2009 by Tomas Mecir
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

#include "cscriptlist.h"

#include "cactionmanager.h"
#include "crunninglist.h"
#include "crunningscript.h"
#include "cscript.h"
#include "cscripteditor.h"

#include <klocale.h>

struct cScriptList::Private {
  QString nameToFind;
  bool nameFound;
  QString paramList;
};

cScriptList::cScriptList () : cList ("scripts")
{
  d = new Private;

  addStringProperty ("command", "Command to execute");
  addStringProperty ("work-directory", "Working directory");
  addBoolProperty ("send-user-commands", "Send user commands to the script", false);
  addBoolProperty ("adv-comunication", "Enable advanced communication", false);
  addBoolProperty ("flow-control", "Enable flow control", true);
  addBoolProperty ("enable-variables", "Enable variable server", false);
  addBoolProperty ("single-instance", "Single-instance script", false);
}

cScriptList::~cScriptList ()
{
  delete d;
}

cListObject *cScriptList::newObject ()
{
  return new cScript (this);
}

cListEditor *cScriptList::editor (QWidget *parent)
{
  return new cScriptEditor (parent);
}

QString cScriptList::nameToFind ()
{
  return d->nameToFind;
}

void cScriptList::setNameFound ()
{
  d->nameFound = true;
}

bool cScriptList::nameExists (const QString &name)
{
  d->nameToFind = name;
  d->nameFound = false;
  traverse (SCRIPT_FIND);
  return d->nameFound;
}

bool cScriptList::runScript (QString name, const QString &paramlist)
{
  d->paramList = paramlist;
  d->nameToFind = name;
  d->nameFound = false;
  traverse (SCRIPT_EXECUTE);
  if (!d->nameFound)
    cActionManager::self()->invokeEvent ("message", session(), i18n ("Such external script does not exist!"));
  return d->nameFound;
}

bool cScriptList::runScript (cScript *script)
{
  if (script == 0) return false;
  cRunningScript *rs = script->prepareToLaunch (d->paramList);
  if (rs != 0)
  {
    cRunningList *rl = dynamic_cast<cRunningList *>(cActionManager::self()->object ("runninglist", session()));
    rl->addScript (rs);
    rs->launch (rl->fcState());
    return true;
  }
  return false;
}

