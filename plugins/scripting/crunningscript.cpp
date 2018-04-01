/***************************************************************************
                          crunningscript.cpp  -  running script
                             -------------------
    begin                : Ne dec 8 2002
    copyright            : (C) 2002-2009 by Tomas Mecir
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

#include "crunningscript.h"

#include "cactionmanager.h"
#include "cscript.h"
#include "cunixsocket.h"

#include <QProcess>

#include <stdlib.h>

cRunningScript::cRunningScript (cScript *s)
{
  script = s;
  process = 0;
  scriptDying    = false;
  dontSignal     = false;
  sendInProgress = false; // No data being sent yet
  stdinBuffer = "";       // Ensure buffer is empty
  unixsocket = 0;
  launchAfter = 0;
}

cRunningScript::~cRunningScript ()
{
  dontSignal = true;
  process->close ();
  process->deleteLater();;
  if (unixsocket) delete unixsocket;
  unixsocket  = 0;
  scriptDying = true; // Prevent further data being sent to this script
  
  if (sendInProgress && flowcontrol)
  {
    //some text was about to be sent, but it won't be sent anymore...
    //we behave as if it was sent, to prevent deadlock in cRunningList
    // (it would endlessly wait for the signal, and no new text would
    // be sent to any other script)
    //
    // scriptDying flag set to prevent the signal causing
    // further data to be sent to this script.
    emit textSent ();
  }  
}

void cRunningScript::launch (int fcState)
{
  // Register the script as started, even if it's actually delayed by flow
  // control.  This prevents multiple instances of a single instance script.
  script->scriptIsStarting ();
  
  if (!flowcontrol)
    doLaunch ();
  else
  {
    //we'll launch when flow control reaches the current point
    launchAfter = fcState;
    if (fcState == 0)
      doLaunch ();
  }
}

void cRunningScript::doLaunch ()
{
  launchAfter = 0;
  
  // connect some signals
  connect (process, SIGNAL (readyReadStandardOutput ()),
      this, SLOT (processScriptStdOutput ()));
  connect (process, SIGNAL (readyReadStandardError ()),
      this, SLOT (processScriptStdError ()));
  connect(process, SIGNAL(bytesWritten (qint64)),
      this, SLOT(stdinReady()));
  connect (process, SIGNAL (finished (int, QProcess::ExitStatus)),
      this, SLOT (finished (int, QProcess::ExitStatus)));
  connect (process, SIGNAL (error (QProcess::ProcessError)),
      this, SLOT (failed (QProcess::ProcessError)));

  //start the process
  process->start (command, args);
}

void cRunningScript::cleanup ()
{
  if (unixsocket) delete unixsocket;
  unixsocket  = 0;
  scriptDying = true; // Prevent further data being sent to this script
  script->scriptIsTerminating ();
  cleanupSend ();
}

void cRunningScript::finished (int exitCode, QProcess::ExitStatus status)
{
  cleanup ();
  if (dontSignal) return;

  if (status == QProcess::NormalExit)
    emit scriptFinished (this, exitCode);
  else
    emit scriptKilled (this);
}

void cRunningScript::failed (QProcess::ProcessError)
{
  if (isRunning()) return;
  cleanup ();
  if (dontSignal) return;
  emit scriptFailed (this);
}

//improvements here by Alex Bache
void cRunningScript::sendCommandToScript (const QString &command, char type)
{
  actuallySent = false;
  
  if (process == 0)
    return;

  //are we waiting for flow control?
  if (launchAfter)
  {
    launchAfter--;
    if (launchAfter == 0)
      //we're in sync - start the script!
      doLaunch ();
    //do not send current line... if we have started the script
    //now, next line will be the first one that it will get
    return;
  }

  if ((type == USERCOMMAND) && (!sendusercommands))
      //type = user command, user commands are not sent
    return;
/*  no, we'll send prompt even if there's no adv.comm
  if ((type == PROMPT) && (!useadvcomm))
      //prompt only sent if using adv.communication
    return;
*/
  
  // don't send anything if the script is in the process of shutting down
  if (scriptDying)
    return;
  
  QString txt = command;
  //advanced communication?
  if (useadvcomm)
  {
    QString beg = QChar (type);
    beg += QChar (' ');
    txt = beg + txt;
  }
  
  //send it if nothing else in progress
  
  if (sendInProgress)
  {
    stdinBuffer.append(txt);
  } // endif must buffer for later
  else
  {
    stdinSending   = txt;
    sendInProgress = true;
    actuallySent   = true;
    process->write (stdinSending.toLocal8Bit());
  } // endelse able to send immediately
  if (flowcontrol)
    emit textAccepted ();
}

//function written by Alex Bache
// stdinReady() called when script has processed the data on its STDIN and is ready for more
void cRunningScript::stdinReady()
{
  //only if we're sending something
  if (!sendInProgress) return;
  if (stdinBuffer.length() > 0)
  {
    //this shouldn't happen with the new input flow control, but just in case
    stdinSending = stdinBuffer;
    stdinBuffer  = "";
    process->write (stdinSending.toLocal8Bit());
  } // endif more data to send to script's STDIN
  else
  {
    sendInProgress = false;
  } // endelse no more data to send at this time

  // inform cRunningList only when ALL buffered text has been sent
  if ((!sendInProgress) && (flowcontrol))
    emit textSent ();
} // cRunningScript::stdinReady

void cRunningScript::processScriptStdOutput ()
{
  processScriptOutput (process->readAllStandardOutput(), true);
}

void cRunningScript::processScriptStdError ()
{
  processScriptOutput (process->readAllStandardError(), false);
}

void cRunningScript::processScriptOutput (const QByteArray &output, bool sendoutput)
{
  QString *line = sendoutput ? &outLine : &errLine;
  // cScript::prepareToLaunch connects to processScriptStdOutput
  // and processScriptStdError. They call this and tell if
  // the output needs to be sent to the MUD.
  int size = output.size();
  for (int i = 0; i < size; i++)
  {
    if (output[i] != '\n')
      *line += QString::fromLocal8Bit (output.data()+i, 1);
    else
    {
      //these will be connected by cRunningList
      if (sendoutput)
        emit sendText (*line);
      else
        emit displayText (*line);
      *line = QString();
    }
  }
}

const QString cRunningScript::name () const
{
  return script->name();
}


bool cRunningScript::isRunning () const
{
  if (process == 0)
    return false;
  //nothing if the script waits for FC
  if (launchAfter) return false;
  return (process->state() == QProcess::Running);
}

void cRunningScript::terminate ()
{
  if (process == 0)
    return;
    
  scriptDying = true; // Prevent further data being sent to this script
  if (isRunning ())
    process->terminate ();
  cleanupSend ();
}

void cRunningScript::kill ()
{
  if (process == 0)
    return;

  scriptDying = true; // Prevent further data being sent to this script
  //now go and kill it
  if (isRunning ())
    process->kill ();
  cleanupSend ();
}

void cRunningScript::cleanupSend ()
{
  if (!sendInProgress) return;
  // if some text was being sent
  // we behave as if the text was delivered successfully, to avoid deadlocking
  sendInProgress = false;
  if (flowcontrol)
    emit textSent ();
}

void cRunningScript::establishSocket (int sess)
{
  //we create a new socket
  unixsocket = new cUnixSocket (sess, this);
  
  //then we get its name
  QString sname = unixsocket->getName ();
  
  //and store it in environment for the script to pick up
  QStringList env = QProcess::systemEnvironment();
  env << "KMUDDY_SOCKET="+sname;
  process->setEnvironment(env);
}

