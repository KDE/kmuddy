//
// C++ Interface: cCmdQueues
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

#ifndef CCMDQUEUES_H
#define CCMDQUEUES_H

#include <cactionbase.h>

#include <qobject.h>

#include <list>

#include <kmuddy_export.h>

/**
This class stores and manages all the command queues.

@author Tomas Mecir
*/

class cCmdQueue;
class QTimer;

class KMUDDY_EXPORT cCmdQueues : public QObject, public cActionBase
{
 Q_OBJECT
 public:
  /** constructor */
  cCmdQueues (int sess);
  /** destructor */
  ~cCmdQueues () override;

  /** add a command queue to the list */
  void addQueue (cCmdQueue *queue);
  /** remove all command queues */
  void removeAll ();
 protected slots:
  void timeout ();
 protected:
  void eventChunkHandler (QString event, int session, cTextChunk *chunk) override;
  void eventStringHandler (QString event, int session, QString &par1, const QString &par2) override;

  /** react on a line */
  void gotLine (const QString &line);
  /** process all queues */
  void processQueues ();
  /** process the queue, send all commands that can be sent before waiting */
  void processQueue (cCmdQueue *queue);
  
  std::list<cCmdQueue *> queues;
  
  /** timer for the wait-like commands and slow-walk */
  QTimer *waitTimer;
};

#endif
