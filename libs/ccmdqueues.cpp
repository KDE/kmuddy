//
// C++ Implementation: cCmdQueues
//
// Description: list of all command queues
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "ccmdqueues.h"

#include "ccmdqueue.h"
#include "ctextchunk.h"

#include <qtimer.h>

cCmdQueues::cCmdQueues (int sess)
 : QObject (), cActionBase ("cmdqueues", sess)
{
  waitTimer = new QTimer;
  connect (waitTimer, &QTimer::timeout, this, &cCmdQueues::timeout);
  
  // this must react BEFORE the triggers ! Other wise we'll get weird results with queues that
  // are invoked from a trigger and want to wait for the same pattern that invoked the trigger
  // those would get invoked immediately - not what we want
  addEventHandler ("got-line", 10, PT_TEXTCHUNK);
  
  // create a new command queue containing this one command ...
  addEventHandler ("command", 50, PT_STRING);
}

cCmdQueues::~cCmdQueues()
{
  removeEventHandler ("got-line");
  removeEventHandler ("command");

  delete waitTimer;
}

void cCmdQueues::eventChunkHandler (QString event, int, cTextChunk *chunk)
{
  if (event == "got-line")
    gotLine (chunk->plainText());
}

void cCmdQueues::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "command")
  {
    cCmdQueue *queue = new cCmdQueue (sess());
    queue->addCommand (par1);
    addQueue (queue);
  }
}

void cCmdQueues::addQueue (cCmdQueue *queue)
{
  // process the queue first
  processQueue (queue);
  if (queue->finished())
    delete queue;
  else
    queues.push_back (queue);
}

void cCmdQueues::removeAll ()
{
  std::list<cCmdQueue *>::iterator it;
  for (it = queues.begin(); it != queues.end(); ++it)
    delete *it;
  queues.clear ();
}

void cCmdQueues::timeout ()
{
  // TODO: handle the queues waiting for some time here ...

}

void cCmdQueues::gotLine (const QString &/*line*/)
{
  // TODO: handle the queues waiting for a line here ...
  
}

void cCmdQueues::processQueues ()
{
  std::list<cCmdQueue *>::iterator it;
  for (it = queues.begin(); it != queues.end(); )
  {
    processQueue (*it);
    if ((*it)->finished()) {
      // queue finished - delete it
      delete *it;
      it = queues.erase (it);
    }
    else
      // queue not finished - just continue with the next one
      ++it;
  }
}

void cCmdQueues::processQueue (cCmdQueue *queue)
{
  bool canExec = true;
  if (queue->waiting ()) {
    // if the queue is waiting for something, check if conditions are met
    // TODO: check conditions
  }

  if (canExec && (!queue->finished ())) {
    // block commands - so that all commands from a queue are sent in one packet, if possible
    invokeEvent ("command-block", sess());

    // conditions are met - keep executing commands while we can
    while (canExec)
    {
      // execute next command in the queue
      queue->executeNext ();

      // if the queue waits or has finished, no more execution
      if (queue->waiting() || (queue->finished()))
        canExec = false;
    }

    // send all queued commands
    invokeEvent ("send-commands", sess());
  }
}

#include "moc_ccmdqueues.cpp"
