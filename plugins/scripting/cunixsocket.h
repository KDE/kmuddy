/***************************************************************************
                          cunixsocket.h  -  UNIX domain socket
                             -------------------
    begin                : Pi okt 3 2003
    copyright            : (C) 2003-2009 by Tomas Mecir
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

#ifndef CUNIXSOCKET_H
#define CUNIXSOCKET_H

#include <qobject.h>

#include <sys/un.h>

class QSocketNotifier;

class cRunningScript;
class cVariableList;

/**
This class implements a UNIX domain socket, together with variable
manipulation.

Everything is done automatically, so the program can just create it
and forget it (it just needs to delete it when no longer needed, of course).

  *@author Tomas Mecir
  */

class cUnixSocket : public QObject  {
   Q_OBJECT
public: 
  cUnixSocket (int _sess, cRunningScript *rs);
  ~cUnixSocket ();
  const QString &getName ();
protected slots:
  void readData (int id);
  void writeData (int id);
protected:
  void processRequest (const QString &type, const QString &data);
  void sendResult (const QString &result);

  struct sockaddr_un sa;

  int sess;
  cRunningScript *script;

  /** file name */
  QString name;

  /** things that are to be read/written */
  QString readCache, writeCache;
  
  /** socket descriptors */
  int id, id2;
  bool connected;

  /** necessary notifiers */
  QSocketNotifier *readnotifier, *writenotifier;

  /** variable list*/
  cVariableList *varlist;
};

#endif
