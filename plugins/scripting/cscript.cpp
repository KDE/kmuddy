/***************************************************************************
                          cscript.cpp  -  one script
                             -------------------
    begin                : So dec 7 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
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

#include "cscript.h"

#include "cactionmanager.h"
#include "crunninglist.h"
#include "crunningscript.h"
#include "cscriptlist.h"

#include <KLocalizedString>
#include <QProcess>

cScript::cScript (cList *list) : cListObject (list)
{
  runningCount = 0;
}

cScript::~cScript()
{
  //nothing here
}

cList::TraverseAction cScript::traverse (int traversalType)
{
  cScriptList *sl = (cScriptList *) list();
  if (name() != sl->nameToFind())  // are we the correct script ?
    return cList::Continue;
  sl->setNameFound ();

  if (traversalType == SCRIPT_FIND)
    return cList::Stop;
  if (traversalType == SCRIPT_EXECUTE)
  {
    sl->runScript (this);
    return cList::Stop;
  }
  return cList::Stop;  // unknown action
}

void cScript::updateVisibleName ()
{
  if (name().isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (name());
}

cRunningScript *cScript::prepareToLaunch (const QString &params)
{
  //do nothing if we're a single-instance script that is already running
  if (boolVal ("single-instance") && (runningCount != 0))
  {
    cActionManager::self()->invokeEvent ("message", list()->session(), i18n ("Sorry, single-instance script."));
    return 0;
  }
  
  cRunningList *rl = dynamic_cast<cRunningList *>(cActionManager::self()->object ("runninglist", list()->session()));
  if (!rl) return 0;

  //create a new instance of cRunningScript
  cRunningScript *script = new cRunningScript (this);
  
  //setup its parameters:

  script->sendusercommands = boolVal ("send-user-commands");
  script->useadvcomm = boolVal ("adv-communication");
  script->flowcontrol = boolVal ("flow-control");

  //create a QProcess instance
  script->process = new QProcess;
  
  script->process->setWorkingDirectory (strVal ("work-directory"));

  //establish socket if needed
  if (boolVal ("enable-variables"))
    script->establishSocket (list()->session());
  
  //split the command into parts...
  QString cmd = strVal ("command");
  QString pars = params.trimmed ();
  if (pars.length() > 0)
    cmd += QString(" ") + pars;
  QString oneparam;
  bool inquotedparam = false;
  QChar quotechar;
  bool wasbackslash = false;
  for (int i = 0; i < cmd.length(); i++)
  {
    if (wasbackslash)
    {
      oneparam += cmd[i];
      wasbackslash = false;
    }
    else
      if (cmd[i].isSpace())
      {
        if (inquotedparam)
          oneparam += cmd[i];
        else
          if (oneparam.length() > 0)
          {
            script->args << oneparam;  //add this parameter!
            oneparam = QString();
          }
      }
      else
        if (cmd[i].toLatin1() == '\\')    //backslash
          wasbackslash = true;
        else
          if (inquotedparam && (cmd[i] == quotechar)) //quote ends
            inquotedparam = false;
          else
            if ((cmd[i].toLatin1() == '\'') || (cmd[i].toLatin1() == '"'))
            {     //quote starts
              inquotedparam = true;
              quotechar = cmd[i];
            }
            else    //normal character
              oneparam += cmd[i];
  }
  if (oneparam.length() > 0)
    script->args << oneparam;  //add last parameter

  // first argument is the command name
  script->command = script->args.takeFirst();

  // Command & parameters are set
  //return that instance
  return script;
}

void cScript::scriptIsStarting ()
{
  runningCount++;
}

void cScript::scriptIsTerminating ()
{
  runningCount--;
}
