//
// C++ Implementation: cvartriggerlist
//
// Description:
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


