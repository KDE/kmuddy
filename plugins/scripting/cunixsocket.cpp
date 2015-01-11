/***************************************************************************
                          cunixsocket.cpp  -  UNIX domain socket
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

#include "cunixsocket.h"

#include <qsocketnotifier.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "cactionmanager.h"
#include "crunninglist.h"
#include "cvariablelist.h"

cUnixSocket::cUnixSocket (int _sess, cRunningScript *rs) : sess(_sess)
{
  script = rs;
  
  readnotifier = writenotifier = 0;
  readCache = writeCache = QString::null;
  id = -1;
  varlist = 0;
  connected = false;
  
  //first of all, we need a file name
  char *fname = tempnam("/tmp", "km");
  if (fname != 0)  //only if it didn't fail
  {
    name = fname;
    free (fname);
    //now that we have the name, we create a socket and set some parameters
    id = socket (PF_UNIX, SOCK_STREAM, 0);
    sa.sun_family = AF_UNIX;
    strcpy (sa.sun_path, name.toLatin1());
    fcntl (id, O_NONBLOCK);
    if (bind (id, (const sockaddr *) &sa, sizeof (sa)) == -1)
    {
      close (id);
      id = -1;
      unlink (name.toLatin1());
      return;
    }

    //it's a listening connect...
    listen (id, 1);

    //the notifier will tell us when the connection is here
    readnotifier = new QSocketNotifier (id, QSocketNotifier::Read, this);
    connect (readnotifier, SIGNAL (activated (int)), this, SLOT (readData (int)));

    //finally, get a pointer to the list of variables
    varlist = dynamic_cast<cVariableList *>(cActionManager::self()->object ("variables", sess));
  }          
}

cUnixSocket::~cUnixSocket ()
{
  //delete the notifiers
  readnotifier->setEnabled (false);
  delete readnotifier;
  delete writenotifier;

  //close the socket
  close (id2);

  //and remove its file
  unlink (name.toLatin1());
}

const QString &cUnixSocket::getName ()
{
  return name;
}

void cUnixSocket::readData (int)
{
  if (!connected)
  {
    socklen_t sz = sizeof (sa);
    id2 = accept (id, (struct sockaddr *) &sa, &sz);
    if (id2 > -1)
    {
      connected = true;
      delete readnotifier;
      close (id);

      fcntl (id2, O_NONBLOCK);
      
      //then we create a pair of notifiers
      readnotifier = new QSocketNotifier (id2, QSocketNotifier::Read, this);
      writenotifier = new QSocketNotifier (id2, QSocketNotifier::Write, this);
      writenotifier->setEnabled (false);
      connect (readnotifier, SIGNAL (activated (int)), this, SLOT (readData (int)));
      connect (writenotifier, SIGNAL (activated (int)), this, SLOT (writeData (int)));
    }
    return;
  }
  
  char buffer[201];
  int n = read (id2, buffer, 200);
  buffer[n] = '\0';
  if (n == -1)
    return;   //bah
  if (n == 0)
    readnotifier->setEnabled (false);
           //hack: disable the notifier
           //(prevents problems on disconnect)
  for (int i = 0; i < n; i++)
    if (buffer[i] != '\n')
      readCache += QChar (buffer[i]);
    else
    {
      QString type = readCache.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString data = readCache.section (' ', 1, -1, QString::SectionSkipEmpty);
      processRequest (type, data);
      readCache = QString::null;
    }
}

void cUnixSocket::writeData (int)
{
  int len = writeCache.length();
  if (len == 0)
    return;   //nothing to write
  int n = write (id2, writeCache.toLatin1(), len);
  //if n is -1, nothing was written and we'll send the data when the write
  //notifier ask us to do so (it may never happen, if the socket was closed
  //by the script, but that's not a problem)
  if (n > -1)
    //remove the part that was successfully sent
    writeCache.remove (0, n);
                    
  if (writeCache.isEmpty()) //nothing more to write
    writenotifier->setEnabled (false);
}

void cUnixSocket::processRequest (const QString &type, const QString &data)
{
  //scripts only exist in profile-based connections, so we can assume
  //that cConnPrefs object exists
  cRunningList *list = dynamic_cast<cRunningList *>(cActionManager::self()->object ("runninglist", sess));
  
  /*
  Request types that are currently supported:
   get - get a variable value
   set - set a variable value
   unset - unset a variable
   inc - increase a variable
   dec - decrease a variable
   request - request a resource
   provide - provide a resource
   lock - lock a variable
   unlock - remove a lock
   send - send a command to the MUD
  */
  if (type == "get")
  {
    QString value = varlist->getValue (data);
    sendResult (value);
  }
  if (type == "set")
  {
    QString varname = data.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = data.section (' ', 1, -1, QString::SectionSkipEmpty);
    if (list->canModify (script, varname))
    {
      varlist->set (varname, value);
      sendResult ("OK");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "unset")
  {
    if (list->canModify (script, data))
    {
      varlist->unset (data);
      sendResult ("OK");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "inc")
  {
    QString varname = data.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = data.section (' ', 1, 1, QString::SectionSkipEmpty);
    bool ok;              
    int num = value.toInt (&ok);
    if (!ok)    //not a number
    {
      sendResult ("FAIL");
      return;
    }
    if (num <= 0)  //bad number
    {
      sendResult ("FAIL");
      return;
    }

    if (list->canModify (script, varname))
    {
      varlist->inc (varname, num);
      sendResult ("OK");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "dec")
  {
    QString varname = data.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = data.section (' ', 1, 1, QString::SectionSkipEmpty);
    bool ok;
    int num = value.toInt (&ok);
    if (!ok)    //not a number
    {
      sendResult ("FAIL");
      return;
    }
    if (num <= 0)  //bad number
    {
      sendResult ("FAIL");
      return;
    }
    if (list->canModify (script, varname))
    {
      varlist->dec (varname, num);
      sendResult ("OK");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "request")
  {
    if (list->canModify (script, data))
    {
      if (varlist->requestResource (data))
        sendResult ("OK");
      else
        sendResult ("FAIL");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "provide")
  {
    if (list->canModify (script, data))
    {
      varlist->provideResource (data);
      sendResult ("OK");
    }
    else
      sendResult ("FAIL");
  }
  if (type == "lock")
  {
    if (list->requestLock (script, data))
      sendResult ("OK");
    else
      sendResult ("FAIL");
  }
  if (type == "unlock")
  {
    list->releaseLock (script, data);
    sendResult ("OK");
  }
  if (type == "send")
  {
    cActionManager::self()->invokeEvent ("command", sess, data);
    
    sendResult ("OK");
  }
}

void cUnixSocket::sendResult (const QString &result)
{
  writenotifier->setEnabled (true);

  writeCache = result + "\n";
  writeData (id);
}

#include "cunixsocket.moc"
