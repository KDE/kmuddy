//
// C++ Implementation: cvartrigger
//
// Description: One variable trigger.
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cvartrigger.h"

#include "cvartriggerlist.h"
#include <QStringList>

struct cVarTrigger::Private {
  QString varname;
  QStringList commands;
};

cVarTrigger::cVarTrigger (cList *list) : cListObject (list)
{
  d = new Private;
}

cVarTrigger::~cVarTrigger()
{
  delete d;
}

void cVarTrigger::updateVisibleName ()
{
  QString variable = strVal ("variable");
  if (variable.isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (variable);
}

void cVarTrigger::attribChanged (const QString &name)
{
  if (name == "variable") {
    QString v = strVal ("variable");
    d->varname = v;
    updateVisibleName ();
    // remove leading $
    // note that this will lead to this function being called again
    if ((!v.isEmpty()) && (v[0] == '$'))
      setStr ("variable", v.mid (1));
  }
}

cList::TraverseAction cVarTrigger::traverse (int traversalType)
{
  if (traversalType == VARTRIGGER_MATCH) {
    QString variable = ((cVarTriggerList *) list())->variableName();
    if (variable == d->varname)
      executeCommands ();
    return cList::Continue;
  }
  return cList::Stop;
}

void cVarTrigger::executeCommands ()
{
  d->commands.clear();

  int commands = strListCount ("command");
  // if we have only one empty command, we do nothing
  if ((commands == 1) && (strListValue ("command", 1).isEmpty()))
    return;

  // add commands to be sent
  for (int i = 1; i <= commands; ++i) {
    QString cmd = strListValue ("command", i);
    d->commands << cmd;
  }
  // send commands
  ((cVarTriggerList *) list())->processCommands (d->commands);
}

