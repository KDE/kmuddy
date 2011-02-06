//
// C++ Implementation: cvartriggerlist
//
// Description:
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cvartriggerlist.h"

#include "cactionmanager.h"
#include "ccmdqueue.h"
#include "ccmdqueues.h"
#include "cvartrigger.h"
#include "cvartriggereditor.h"

struct cVarTriggerList::Private {
  QString varName;
};

cVarTriggerList::cVarTriggerList ()
  : cList ("vartriggers")
{
  d = new Private;

  addStringProperty ("variable", "Variable to react on");
  addIntProperty ("command-count", "Command count", 0);
  // then we have a "command-"+i string for each
}

cVarTriggerList::~cVarTriggerList()
{
  delete d;
}

cListObject *cVarTriggerList::newObject ()
{
  return new cVarTrigger (this);
}

cListEditor *cVarTriggerList::editor (QWidget *parent)
{
  return new cVarTriggerEditor (parent);
}

void cVarTriggerList::variableChanged (const QString &varname)
{
  d->varName = varname;

  traverse (VARTRIGGER_MATCH);
}

void cVarTriggerList::processCommands (const QStringList &commands)
{
  //okay, send the commands!
  cCmdQueues *queues = (cCmdQueues *) cActionManager::self()->object ("cmdqueues", session());
  if (!queues) return;
  cCmdQueue *queue = new cCmdQueue (session());
  QStringList::const_iterator it;
  for (it = commands.begin(); it != commands.end(); ++it)
    queue->addCommand (*it);
  queues->addQueue (queue);
}

QString cVarTriggerList::variableName () const
{
  return d->varName;
}


