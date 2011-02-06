//
// C++ Implementation: cvartrigger
//
// Description: One variable trigger.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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

