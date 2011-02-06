//
// C++ Interface: cCmdQueues
//
// Description: list of all command queues
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  ~cCmdQueues ();

  /** add a command queue to the list */
  void addQueue (cCmdQueue *queue);
  /** remove all command queues */
  void removeAll ();
 protected slots:
  void timeout ();
 protected:
  virtual void eventChunkHandler (QString event, int session, cTextChunk *chunk);
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &par2);

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
