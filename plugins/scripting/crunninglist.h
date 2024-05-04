/***************************************************************************
                          crunninglist.h  -  list of running scripts
                             -------------------
    begin                : Ne dec 22 2002
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

#ifndef CRUNNINGLIST_H
#define CRUNNINGLIST_H

class cRunningScript;

using namespace std;

#include "cactionbase.h"

#include <qobject.h>
#include <list>
#include <map>
#include <qcolor.h>

class QSocketNotifier;
class QAbstractItemModel;
class cRunningModel;

/**
Manages all running scripts.

  *@author Tomas Mecir
  */

class cRunningList: public QObject, public cActionBase {
Q_OBJECT
public: 
  cRunningList (int sess);
  virtual ~cRunningList ();

  QAbstractItemModel *getModel ();

  /** add a new script to my list */
  void addScript (cRunningScript *script);

  /** flow control state, i.e. how many lines are waiting in the
  flow control's queue */
  int fcState () { return textQueue.size (); };
  
  void killAll ();

  /** get script name */
  const QString name (int id);
  /** try to terminate the script */
  void terminate (int id);
  /** kill the script NOW! */
  void kill (int id);

  /** attempt to give a write-lock to a script */
  bool requestLock (cRunningScript *script, const QString &varname);
  /** release a lock */
  void releaseLock (cRunningScript *script, const QString &varname);
  /** look if the variable can be modified by a script (returns true
  if the variable is not locked or if this script has the lock) */
  bool canModify (cRunningScript *script, const QString &varname);
private:
  QColor getColor(QString s);
  QColor m_currentFgColor;
  QColor m_currentBkColor;

signals:
  void stateChanged ();
protected:
  virtual void eventStringHandler (QString event, int, QString &par1, const QString &);
  /** remove one script from my list */
  void removeScript (cRunningScript *script);
  void sendThisNow (const QString &text, int type, bool noFC = false);
  /** send input to flow-controlled scripts */
  void sendToFlowControlled(const QString &text, int type);
  /** send user command to all scripts */
  void sendCommand (const QString &text);
  /** send server output to all scripts */
  void sendServerOutput (const QString &text);
  /** send server prompt to all scripts */
  void sendPrompt (const QString &prompt);

  /** list of all running scripts */
  map<int, cRunningScript *> scripts;
  /** last used script ID */
  int lastid;
  
  cRunningScript *getRunningScript (int id);
  /** text sending synchronization */
  int waitCounter;
  /** text sending operation in progress */
  bool waitLock;
  /** text strings waiting to be sent to scripts */
  list<QString> textQueue;
  /** types of those strings */
  list<int> typeQueue;
  map<QString, cRunningScript *> locks;

  cRunningModel *model;
  friend class cRunningModel;
protected slots:
  /** send a text from a script to the server */
  void sendText (const QString &text);
  /** display a text from the script */
  void displayText (const QString &text);
  
  /** one script has finished succesfully */
  void scriptFinished (cRunningScript *script, int returnValue);
  /** one script has been killed */
  void scriptKilled (cRunningScript *script);
  /** one script couldn't be started */
  void scriptFailed (cRunningScript *script);
  /** cRunningScript has accepted the text (but not sent to script yet) */
  void scriptTextAccepted ();
  /** cRunningScript has received the sent text */
  void scriptTextSent ();
};

#endif
