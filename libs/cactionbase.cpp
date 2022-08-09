//
// C++ Implementation: cActionBase
//
// Description: action base
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

#include "cactionbase.h"
#include "cactionmanager.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"

#include <map>

struct cActionBasePrivate {
  std::map<QString, int> attribs;
  std::map<QString, QString> strattribs;
  int sess;
};

cActionBase::cActionBase (const QString &name, int session)
{
  d = new cActionBasePrivate;
  _name = name;
  d->sess = session;
  if (session >= 0)
    cActionManager::self()->registerObject (this, session);  
}

cActionBase::~cActionBase ()
{
  if (d->sess >= 0)
    cActionManager::self()->unregisterObject (this, d->sess);
  delete d;
  d = nullptr;
}

int cActionBase::sess ()
{
  return d->sess;
}

int cActionBase::attrib (const QString &name)
{
  if (d->attribs.count (name))
    return d->attribs[name];
  return 0;
}

void cActionBase::setAttrib (const QString &name, int value)
{
  d->attribs[name] = value;
}

QString cActionBase::strAttrib (const QString &name)
{
  if (d->strattribs.count (name))
    return d->strattribs[name];
  return QString();
}

void cActionBase::setStrAttrib (const QString &name, const QString &value)
{
  d->strattribs[name] = value;
}

cProfileSettings *cActionBase::settings ()
{
  return cProfileManager::self()->settings (d->sess);
}

void cActionBase::addEventHandler (QString name, int priority, ParamType pt)
{
  cActionManager::self()->addEventHandler (this, d->sess, name, priority, pt);
}

void cActionBase::removeEventHandler (QString name)
{
  cActionManager::self()->removeEventHandler (this, d->sess, name);
}

void cActionBase::addGlobalEventHandler (QString name, int priority, ParamType pt)
{
  cActionManager::self()->addEventHandler (this, 0, name, priority, pt);
}

void cActionBase::removeGlobalEventHandler (QString name)
{
  cActionManager::self()->removeEventHandler (this, 0, name);
}

cActionBase *cActionBase::object (const QString &name, int session)
{
  return cActionManager::self()->object (name, (session == -1) ? d->sess : session);
}

QString cActionBase::callAction (QString objectName, QString action, int session, int par1,
    int par2)
{
  return cActionManager::self()->callAction (objectName, action, session, par1, par2);
}

QString cActionBase::callAction (QString objectName, QString action, int session,
    QString &par1, const QString &par2)
{
  return cActionManager::self()->callAction (objectName, action, session, par1, par2);
}

QString cActionBase::callAction (QString objectName, QString action, int session,
    const QString &par1, const QString &par2)
{
  QString p = par1;
  return cActionManager::self()->callAction (objectName, action, session, p, par2);
}

QString cActionBase::callAction (QString objectName, QString action, int session)
{
  return cActionManager::self()->callAction (objectName, action, session);
}

QString cActionBase::callAction (QString objectName, QString action, int session, cTextChunk *par)
{
  return cActionManager::self()->callAction (objectName, action, session, par);
}

QString cActionBase::callAction (QString objectName, QString action, int session, void *par)
{
  return cActionManager::self()->callAction (objectName, action, session, par);
}

void cActionBase::invokeEvent (QString event, int session, int par1, int par2)
{
  cActionManager::self()->invokeEvent (event, session, par1, par2);
}

void cActionBase::invokeEvent (QString event, int session, QString &par1,
    const QString &par2)
{
  cActionManager::self()->invokeEvent (event, session, par1, par2);
}

void cActionBase::invokeEvent (QString event, int session, const QString &par1,
    const QString &par2)
{
  QString p = par1;
  cActionManager::self()->invokeEvent (event, session, p, par2);
}

void cActionBase::invokeEvent (QString event, int session)
{
  cActionManager::self()->invokeEvent (event, session);
}

void cActionBase::invokeEvent (QString event, int session, cTextChunk *par)
{
  cActionManager::self()->invokeEvent (event, session, par);
}

void cActionBase::invokeEvent (QString event, int session, void *par)
{
  cActionManager::self()->invokeEvent (event, session, par);
}

void cActionBase::eventNothingHandler (QString /*event*/, int /*session*/) {}
void cActionBase::eventIntHandler (QString /*event*/, int /*session*/, int /*par1*/, int /*par2*/){}
void cActionBase::eventStringHandler (QString /*event*/, int /*session*/,
      QString &/*par1*/, const QString &/*par2*/){}
void cActionBase::eventChunkHandler (QString /*event*/, int /*session*/, cTextChunk * /*par*/){}
void cActionBase::eventVoidHandler (QString /*event*/, int /*session*/, void * /*par*/){}

