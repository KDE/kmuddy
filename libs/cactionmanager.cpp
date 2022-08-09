//
// C++ Implementation: cActionManager
//
// Description: action manager
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

#include "cactionmanager.h"

#include "cactionbase.h"

#include <kactioncollection.h>
#include <kdebug.h>
#include <kmainwindow.h>
#include <QWidget>

#include <list>
#include <map>

struct AMprivAction {
  cActionBase *ab;
  QString name;
  ParamType pt;
};

struct AMprivEvent {
  std::multimap<int, AMprivAction> handlers;
};

struct AMprivSessionInfo {
  // objects, accessible by names
  std::map<QString, cActionBase *> objects;  
  // event handlers, accessible by event names
  std::map<QString, AMprivEvent> events;
  // attributes
  std::map<QString, int> attribs;
  // string attributes
  std::map<QString, QString> strattribs;
};

struct cActionManagerPrivate {
  std::map<int, AMprivSessionInfo *> sessions;
  std::list<int> sessionList;
  int activesess;

  KMainWindow *mainWindow;

  // Shortcuts - only here because I have no better place for them :D
  // and I need them in lib/
  KActionCollection *acol;
};
  
cActionManager *cActionManager::_self = nullptr;

cActionManager::cActionManager ()
{
  d = new cActionManagerPrivate;
  d->activesess = 0;
  registerSession (0);
  d->acol = nullptr;
  d->mainWindow = nullptr;
}

cActionManager::~cActionManager ()
{
  unregisterSession (0);
  delete d;
  _self = nullptr;
  if (d->acol) delete d->acol;
}

cActionManager *cActionManager::self ()
{
  if (!_self)
    _self = new cActionManager;
  return _self;
}

void cActionManager::registerSession (int id)
{
  if (sessionExists (id)) return;
  if (id < 0) return;
  d->sessions[id] = new AMprivSessionInfo;
  if (id) d->sessionList.push_back (id);
}

void cActionManager::unregisterSession (int id)
{
  if (!sessionExists (id)) return;

  // delete all objects within the session
  // we need to use a temporary list, because deleting cActionBase would
  // invalidate the iterator ...
  std::map<QString, cActionBase *>::iterator it;
  std::list<cActionBase *> delList;
  std::list<cActionBase *>::iterator itl;
  
  for (it = d->sessions[id]->objects.begin(); it != d->sessions[id]->objects.end(); ++it) {
    kWarning() << "Object " << it->first << " in session " << id << " was not deleted properly.";
    delList.push_back (it->second);
  }
  for (itl = delList.begin(); itl != delList.end(); ++itl)
    delete *itl;

  // delete the session
  delete d->sessions[id];
  d->sessions.erase (id);
  if (id) d->sessionList.remove (id);
}

bool cActionManager::sessionExists (int id)
{
  return (d->sessions.count (id) != 0);
}

list<int> cActionManager::sessionList ()
{
  return d->sessionList;
}

int cActionManager::sessions ()
{
  return d->sessionList.size();
}

int cActionManager::activeSession ()
{
  return d->activesess;
}

void cActionManager::setActiveSession (int sess)
{
  d->activesess = sess;
}

void cActionManager::setSessionAttrib (int sess, const QString &name, int value)
{
  if (!sessionExists (sess)) return;
  d->sessions[sess]->attribs[name] = value;
}

void cActionManager::setSessionStrAttrib (int sess, const QString &name, const QString &value)
{
  if (!sessionExists (sess)) return;
  d->sessions[sess]->strattribs[name] = value;
}

int cActionManager::sessionAttrib (int sess, const QString &name)
{
  if (!sessionExists (sess)) return 0;
  if (d->sessions[sess]->attribs.count (name))
    return d->sessions[sess]->attribs[name];
  return 0;
}

QString cActionManager::sessionStrAttrib (int sess, const QString &name)
{
  if (!sessionExists (sess)) return QString();
  if (d->sessions[sess]->strattribs.count (name))
    return d->sessions[sess]->strattribs[name];
  return QString();
}

void cActionManager::registerObject (cActionBase *ab, int session)
{
  if (!sessionExists (session)) return;
  d->sessions[session]->objects[ab->objName()] = ab;
}

void cActionManager::unregisterObject (cActionBase *ab, int session)
{
  if (!sessionExists (session)) return;
  d->sessions[session]->objects.erase (ab->objName());

  // TODO: remove all event handlers for this object ...
}

#include <iostream>
cActionBase *cActionManager::object (const QString &name, int session)
{
  if (!sessionExists (session)) {
    std::cerr << "Requested object " << name.toLatin1().data() << " from non-existant session " << session << std::endl;
    return nullptr;
  }
  if (d->sessions[session]->objects.count (name))
  {
    cActionBase *ab = d->sessions[session]->objects[name];
    return ab;
  }
  std::cerr << "Object " << name.toLatin1().data() << " from session " << session << " not found." << std::endl;
  return nullptr;
}

void cActionManager::addEventHandler (cActionBase *ab, int session, QString name,
    int priority, ParamType pt)
{
  if (!sessionExists (session)) return;
  AMprivAction a;
  a.ab = ab;
  a.name = name;
  a.pt = pt;
  d->sessions[session]->events[name].handlers.insert (
      std::pair<int, AMprivAction> (priority, a));
}

void cActionManager::removeEventHandler (cActionBase *ab, int session, QString name)
{
  if (!sessionExists (session)) return;
  std::multimap<int, AMprivAction>::iterator it;
  for (it = d->sessions[session]->events[name].handlers.begin();
      it != d->sessions[session]->events[name].handlers.end(); ++it)
  {
    if ((it->second.name == name) && (it->second.ab == ab))
    {
      d->sessions[session]->events[name].handlers.erase (it);
      return;
    }
  }
}

QString cActionManager::callAction (QString objectName, QString action, int session,
    int par1, int par2)
{
  if (!sessionExists (session)) return QString();
  
  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionIntHandler (action, session, par1, par2);
  
  return QString();
}

QString cActionManager::callAction (QString objectName, QString action, int session,
    QString &par1, const QString &par2)
{
  if (!sessionExists (session)) return QString();

  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionStringHandler (action, session, par1, par2);
  return QString();
}

QString cActionManager::callAction (QString objectName, QString action, int session,
    const QString &par1, const QString &par2)
{
  if (!sessionExists (session)) return QString();

  QString p = par1;
  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionStringHandler (action, session, p, par2);
  return QString();
}

QString cActionManager::callAction (QString objectName, QString action, int session)
{
  if (!sessionExists (session)) return QString();

  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionNothingHandler (action, session);
  return QString();
}

QString cActionManager::callAction (QString objectName, QString action, int session,
    cTextChunk *par)
{
  if (!sessionExists (session)) return QString();

  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionChunkHandler (action, session, par);
  return QString();
}

QString cActionManager::callAction (QString objectName, QString action, int session,
    void *par)
{
  if (!sessionExists (session)) return QString();

  cActionBase *ab = object (objectName, session);
  if (ab)
    return ab->actionVoidHandler (action, session, par);
  return QString();
}


void cActionManager::invokeEvent (QString event, int session, int par1, int par2)
{
  if (!sessionExists (session)) return;

  std::multimap<int, AMprivAction>::iterator it;  
  // events for session 0 go to event handlers of all sessions
  if (session == 0) {
    std::map<int, AMprivSessionInfo *>::iterator it2;
    for (it2 = d->sessions.begin(); it2 != d->sessions.end(); ++it2)
      for (it = it2->second->events[event].handlers.begin();
           it != it2->second->events[event].handlers.end(); ++it)
        if (it->second.pt == PT_INT)
          it->second.ab->eventIntHandler (event, session, par1, par2);
    
  }
  else
  {
    // send the event to the target session handlers
    for (it = d->sessions[session]->events[event].handlers.begin();
            it != d->sessions[session]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_INT)
        it->second.ab->eventIntHandler (event, session, par1, par2);
    
    // objects with session number 0 receive all events
    for (it = d->sessions[0]->events[event].handlers.begin();
         it != d->sessions[0]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_INT)
        it->second.ab->eventIntHandler (event, session, par1, par2);
  }
}

void cActionManager::invokeEvent (QString event, int session,
    QString &par1, const QString &par2)
{
  if (!sessionExists (session)) return;

  std::multimap<int, AMprivAction>::iterator it;  
  // events for session 0 go to event handlers of all session
  if (session == 0) {
    std::map<int, AMprivSessionInfo *>::iterator it2;
    for (it2 = d->sessions.begin(); it2 != d->sessions.end(); ++it2)
      for (it = it2->second->events[event].handlers.begin();
           it != it2->second->events[event].handlers.end(); ++it)
        if (it->second.pt == PT_STRING)
          it->second.ab->eventStringHandler (event, session, par1, par2);
    
  }
  else
  {
    // send the event to the target session handlers
    for (it = d->sessions[session]->events[event].handlers.begin();
            it != d->sessions[session]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_STRING)
        it->second.ab->eventStringHandler (event, session, par1, par2);
    
    // objects with session number 0 receive all events
    for (it = d->sessions[0]->events[event].handlers.begin();
         it != d->sessions[0]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_STRING)
        it->second.ab->eventStringHandler (event, session, par1, par2);
  }
  
}

void cActionManager::invokeEvent (QString event, int session,
    const QString &par1, const QString &par2)
{
  QString p = par1;
  invokeEvent (event, session, p, par2);
}

void cActionManager::invokeEvent (QString event, int session)
{
  if (!sessionExists (session)) return;

  std::multimap<int, AMprivAction>::iterator it;  
  // events for session 0 go to event handlers of all session
  if (session == 0) {
    std::map<int, AMprivSessionInfo *>::iterator it2;
    for (it2 = d->sessions.begin(); it2 != d->sessions.end(); ++it2)
      for (it = it2->second->events[event].handlers.begin();
           it != it2->second->events[event].handlers.end(); ++it)
        if (it->second.pt == PT_NOTHING)
          it->second.ab->eventNothingHandler (event, session);
    
  }
  else
  {
    // send the event to the target session handlers
    for (it = d->sessions[session]->events[event].handlers.begin();
            it != d->sessions[session]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_NOTHING)
        it->second.ab->eventNothingHandler (event, session);  
    
    // objects with session number 0 receive all events
    for (it = d->sessions[0]->events[event].handlers.begin();
         it != d->sessions[0]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_NOTHING)
        it->second.ab->eventNothingHandler (event, session);
  }
}

void cActionManager::invokeEvent (QString event, int session, cTextChunk *par)
{
  if (!sessionExists (session)) return;

  std::multimap<int, AMprivAction>::iterator it;  
  // events for session 0 go to event handlers of all session
  if (session == 0) {
    std::map<int, AMprivSessionInfo *>::iterator it2;
    for (it2 = d->sessions.begin(); it2 != d->sessions.end(); ++it2)
      for (it = it2->second->events[event].handlers.begin();
           it != it2->second->events[event].handlers.end(); ++it)
        if (it->second.pt == PT_TEXTCHUNK)
          it->second.ab->eventChunkHandler (event, session, par);
    
  }
  else
  {
    // send the event to the target session handlers
    for (it = d->sessions[session]->events[event].handlers.begin();
            it != d->sessions[session]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_TEXTCHUNK)
        it->second.ab->eventChunkHandler (event, session, par);
    
    // objects with session number 0 receive all events
    for (it = d->sessions[0]->events[event].handlers.begin();
         it != d->sessions[0]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_TEXTCHUNK)
        it->second.ab->eventChunkHandler (event, session, par);
  }
}

void cActionManager::invokeEvent (QString event, int session, void *par)
{
  if (!sessionExists (session)) return;

  std::multimap<int, AMprivAction>::iterator it;  
  // events for session 0 go to event handlers of all session
  if (session == 0) {
    std::map<int, AMprivSessionInfo *>::iterator it2;
    for (it2 = d->sessions.begin(); it2 != d->sessions.end(); ++it2)
      for (it = it2->second->events[event].handlers.begin();
           it != it2->second->events[event].handlers.end(); ++it)
        if (it->second.pt == PT_TEXTCHUNK)
          it->second.ab->eventVoidHandler (event, session, par);
    
  }
  else
  {
    // send the event to the target session handlers
    for (it = d->sessions[session]->events[event].handlers.begin();
         it != d->sessions[session]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_POINTER)
        it->second.ab->eventVoidHandler (event, session, par);
    
    // objects with session number 0 receive all events
    for (it = d->sessions[0]->events[event].handlers.begin();
         it != d->sessions[0]->events[event].handlers.end(); ++it)
      if (it->second.pt == PT_POINTER)
        it->second.ab->eventVoidHandler (event, session, par);
  }
}

void cActionManager::setMainWindow (KMainWindow *window)
{
  if (d->mainWindow) return;   // already set - do not override
  d->mainWindow = window;
}

QWidget *cActionManager::mainWidget ()
{
  return d->mainWindow;
}

KMainWindow *cActionManager::mainWindow ()
{
  return d->mainWindow;
}

void cActionManager::createACol (QWidget *parent)
{
  if (!d->acol)
    d->acol = new KActionCollection (parent);
  //create acol object if needed, but nothing more... readShortcutSettings()
  //is called in kmuddy->prepareObjects ();
}

KActionCollection *cActionManager::getACol ()
{
  return d->acol;
}

