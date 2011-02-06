/***************************************************************************
                          crunningscript.h  -  running script
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

#ifndef CRUNNINGSCRIPT_H
#define CRUNNINGSCRIPT_H

#include <qobject.h>

class cScript;
class cUnixSocket;

#include <QProcess>

#define SERVEROUTPUT '1'
#define USERCOMMAND '2'
#define PROMPT '3'

/**
One currently running script.
  *@author Tomas Mecir
  */

class cRunningScript : public QObject  {
   Q_OBJECT
protected: 
  friend class cScript;
  friend class cRunningList;
  
  /** this object can only be created by cScript, ensuring that unauthorized
  classes don't mess it up (we rely on cScript heavily)
  Then it's registered with cRunningList, which takes care of deleting of
  this object. That's why we need cRunningScript as a friend too. */
  cRunningScript (cScript *s);
  ~cRunningScript ();

public:

  /** send command to script, if we have this feature enabled. Type is
  used for advanced communication, it determines type of sent line;
  1 = server output, 2 = user command, 3 = prompt; more types coming later */
  void sendCommandToScript (const QString &command, char type = SERVEROUTPUT);

  /** Launch the script !!! */
  void launch (int fcState);

  /** get script name */
  const QString name () const;
  /** is the script running? */
  bool isRunning () const;
  /** try to terminate the script */
  void terminate ();
  /** kill the script */
  void kill ();

  int getId () const { return id; };
  /** set script ID; used by cRunningList */
  void setId (int _id) { id = _id; };

  /** is flow control enabled for this script? */
  bool flowControl () const { return flowcontrol; };
  /** has input actually been transmitted as a result of sendCommandToScript() ? */
  bool actuallySentCommand() const { return actuallySent; }
signals:
  /** successfully sent last text to the script */
  void textSent ();
  /** text accepted by the class, will be sent now */
  void textAccepted ();
  /** command to be sent */
  void sendText (const QString &text);
  /** text to be displayed */
  void displayText (const QString &text);
  /** script finished normally, exit value is returnValue */
  void scriptFinished (cRunningScript *me, int returnValue);
  /** script was killed with a signal */
  void scriptKilled (cRunningScript *me);
  /** script couldn't be started */
  void scriptFailed (cRunningScript *me);
protected slots:
  void processScriptStdOutput ();
  void processScriptStdError ();
  void finished (int, QProcess::ExitStatus);
  void failed (QProcess::ProcessError);
  void stdinReady();
protected:
  void establishSocket (int sess);
  void processScriptOutput (const QByteArray &output, bool sendoutput);
  /** launch the script, after it has been waiting for flow control
  to synchronize */
  void doLaunch ();
  void cleanup ();
  void cleanupSend ();

  cUnixSocket *unixsocket;
  cScript *script;
  
  /** should we emit a signal when the process has ended? Set to false in
  destructor before deleting the QProcess object to prevent double deletion
  of this object (that would cause a crash) */
  bool dontSignal;

  /** script ID; this has nothing to do with its PID! */
  int id;

  int launchAfter;
  
  QString outLine, errLine;
  bool scriptDying;

  bool onlyifmatch;

  QString command;
  QStringList args;

  QProcess *process;
  bool sendusercommands;
  bool useadvcomm;

  /** should be use flow control? */
  bool flowcontrol;

  bool    actuallySent;   // Was data actually transmitted to this script's STDIN?
  bool    sendInProgress; // Is data being sent to this script's STDIN?
  QString stdinBuffer;    // Buffered data to send to script's STDIN
  QString stdinSending;   // Data in transit to script's STDIN
};

#endif
