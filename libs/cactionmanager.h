//
// C++ Interface: cActionManager
//
// Description: action manager
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, 2005-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CACTIONMANAGER_H
#define CACTIONMANAGER_H

class cTextChunk;
struct cActionManagerPrivate;

class QWidget;
class KActionCollection;
class KMainWindow;

#include <cactionbase.h>

#include <list>

#include <kmuddy_export.h>

using namespace std;

/**
This is the action manager, it registers objects, actions and events, everyone who wants to use it must register with it. Registration is done automatically in cActionBase.

It remembers who wants to receive which event and routes them as needed, also remembers the session-specific objects belonging to each session and returns pointers to them if needed.


@author Tomas Mecir
*/
class KMUDDY_EXPORT cActionManager {
 public:
  static cActionManager *self ();
  /** destructor */
  ~cActionManager ();
  
  void registerSession (int id);
  void unregisterSession (int id);
  bool sessionExists (int id);
  list<int> sessionList ();
  /** Session count. */
  int sessions ();
  
  void setSessionAttrib (int sess, const QString &name, int value);
  void setSessionStrAttrib (int sess, const QString &name, const QString &value);
  int sessionAttrib (int sess, const QString &name);
  QString sessionStrAttrib (int sess, const QString &name);
  
  int activeSession ();
  void setActiveSession (int sess);

  void registerObject (cActionBase *ab, int session = -1);
  void unregisterObject (cActionBase *ab, int session = -1);

  cActionBase *object (const QString &name, int session = 0);  

  /** add action of an object - name must be unique (per object) */
  void addEventHandler (cActionBase *ab, int session, QString name, int priority, ParamType pt);
  void removeEventHandler (cActionBase *ab, int session, QString name);
  
  /** call an object's action - PT_INT parameter */
  QString callAction (QString objectName, QString action, int session, int par1, int par2=0);
  /** call an object's action - PT_STRING parameter */
  QString callAction (QString objectName, QString action, int session,
      QString &par1, const QString &par2=QString());
  /** as above, but with const string */
  QString callAction (QString objectName, QString action, int session,
      const QString &par1, const QString &par2=QString());
  /** call an object's action - PT_NOTHING parameter */
  QString callAction (QString objectName, QString action, int session);
  /** call an object's action - PT_TEXTCHUNK parameter */
  QString callAction (QString objectName, QString action, int session, cTextChunk *par);
  /** call an object's action - PT_POINTER parameter */
  QString callAction (QString objectName, QString action, int session, void *par);

  /** invoke an event - PT_INT parameter */
  void invokeEvent (QString event, int session, int par1, int par2 = 0);
  /** invoke an event - PT_STRING parameter */
  void invokeEvent (QString event, int session, QString &par1,
      const QString &par2 = QString());
  /** as above, but const string */
  void invokeEvent (QString event, int session, const QString &par1,
      const QString &par2 = QString());
  /** invoke an event - PT_NOTHING parameter */
  void invokeEvent (QString event, int session);
  /** invoke an event - PT_TEXTCHUNK parameter */
  void invokeEvent (QString event, int session, cTextChunk *par);
  /** invoke an event - PT_POINTER parameter */
  void invokeEvent (QString event, int session, void *par);

  // Shortcuts
  void createACol (QWidget *widget);
  KActionCollection *getACol ();

  /** Set pointer to the main window. */
  void setMainWindow (KMainWindow *window);
  /** Pointer to the main window. */
  KMainWindow *mainWindow ();
  /** Pointer to the main window as a QWidget. */
  QWidget *mainWidget ();
 private:
  /** constructor */
  cActionManager ();
  static cActionManager *_self;
  cActionManagerPrivate *d;
};

#endif
