/***************************************************************************
                          cscript.cpp  -  one script
                             -------------------
    begin                : So dec 7 2002
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

#include "cscript.h"

#include <qdir.h>
#include <kconfig.h>
#include <kconfiggroup.h>

cScript::cScript (int _sess) : sess(_sess)
{
  //set some defaults...
  name = ""; //name must be set before script can be executed...
  comment = "";
  command = "";
  workdir = QDir::homePath ();
  prefix = "";
  suffix = "";
  enableinput = true;
  enablestdout = true;
  sendstdout = true;
  enablestderr = false;
  sendstderr = false;
  sendusercommands = false;
  useadvcomm = false;
  allowparams = true;
  singleinstance = false;
  shellexpansion = true;
  noflowcontrol = false;
  allowvars = false;
  onlyifmatch = true;
}

cScript::~cScript()
{
  //nothing here
}

cSaveableField *cScript::newInstance ()
{
  return new cScript (sess);
}

void cScript::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);
  name = g.readEntry ("Name", "");
  comment = g.readEntry ("Comment", "");
  command = g.readEntry ("Command", "");
  workdir = g.readEntry ("Workdir", QDir::homePath ());
  prefix = g.readEntry ("Prefix", "");
  suffix = g.readEntry ("Suffix", "");
  enableinput = g.readEntry ("Enable input", true);
  enablestdout = g.readEntry ("Enable output", true);
  sendstdout = g.readEntry ("Send output", true);
  enablestderr = g.readEntry ("Include stderr", false);
  sendstderr = g.readEntry ("Send stderr", false);
  sendusercommands = g.readEntry ("Send user commands", false);
  useadvcomm = g.readEntry ("Use advanced communication", false);
  allowparams = g.readEntry ("Allow parameters", true);
  singleinstance = g.readEntry ("Single instance", false);
  shellexpansion = g.readEntry ("Shell expansion", true);
  noflowcontrol = g.readEntry ("No flow control", false);
  allowvars = g.readEntry ("Communicate variables", false);

  setText (g.readEntry ("Text", ""));
  setType (g.readEntry ("Type", (int)substring));
  onlyifmatch = g.readEntry ("Only if match", true);
}

