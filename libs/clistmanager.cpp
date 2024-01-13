//
// C++ Implementation: clistmanager
//
// Description: 
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "clistmanager.h"

#include "clist.h"
#include "cprofilemanager.h"

#include <map>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>


using namespace std;

struct cListManager::Private {
  map<QString, cList *(*)()> listTypes;
  map<QString, QStandardItem *> modelItems;
  map<int, map<QString, cList *> > lists;
  QStandardItemModel *model;
  map<int, cListObject *> objects;
  map<cListObject *, int> objectIDs;
  int maxObjectID;
};

cListManager *cListManager::_self = nullptr;

cListManager *cListManager::self()
{
  if (!_self)
    _self = new cListManager;
  return _self;
}

cListManager::cListManager () : cActionBase ("listmanager", 0)
{
  d = new Private;
  d->model = new QStandardItemModel;
  d->maxObjectID = 0;
 
  // properly react on session connect/disconnect
  addEventHandler ("profile-init", 5, PT_STRING);
  addEventHandler ("profile-load", 5, PT_STRING);
  addEventHandler ("disconnected", 5, PT_NOTHING);
  addEventHandler ("save", 20, PT_NOTHING);
}

cListManager::~cListManager ()
{
  removeEventHandler ("profile-init");
  removeEventHandler ("profile-load");
  removeEventHandler ("disconnected");
  removeEventHandler ("save");

  delete d->model;
  delete d;
}

void cListManager::eventNothingHandler (QString event, int session)
{
  if (event == "disconnected") {
    // these will get called for both profile and non-profile sessions -
    // for the latter, they'll simply do nothing
    cProfileManager::self()->unassignSession (sess());
    removeSession (session);
  }
  if (event == "save") {
    saveAll (session);
  }
}


void cListManager::eventStringHandler (QString event, int session, QString &par1, const QString &)
{
  if (event == "profile-init") {
    // tie the session to the profile
    cProfileManager::self()->assignSession (session, par1);
  }
  if (event == "profile-load") {
    // load all the lists
    addSession (session);
  }
}

void cListManager::registerType (const QString &name, const QString &visibleName, cList *(*factory)())
{
  // type already registered - do nothing
  if (d->listTypes.count (name))
    return;
  d->listTypes[name] = factory;

  // add the type to the model
  QStandardItem *item = new QStandardItem (visibleName);
  item->setData (name);  // remember real name
  d->modelItems[name] = item;
  d->model->invisibleRootItem()->appendRow (item);
  d->model->sort (0);

  // create the object in every existing session
  // this will ensure that data will be loaded, events hooked, and so on
  map<int, map<QString, cList *> >::iterator it;
  for (it = d->lists.begin(); it != d->lists.end(); ++it)
    getList (it->first, name);
}

void cListManager::unregisterType (const QString &name)
{
  // delete all lists of this type
  map<int, map<QString, cList *> >::iterator it;
  for (it = d->lists.begin(); it != d->lists.end(); ++it) {
    if (it->second.count (name)) {
      saveList (it->first, it->second[name]);
      delete it->second[name];
      it->second.erase (name);
    }
  }

  // unregister the type
  d->listTypes.erase (name);
  if (d->modelItems.count (name))
    delete d->modelItems[name];
  d->modelItems.erase (name);
}
  
QStandardItemModel *cListManager::typeModel () const
{
  return d->model;
}

cList *cListManager::getList (int sessId, const QString &name)
{
  if (!d->lists.count (sessId)) return nullptr;  // session must exist
  if (!d->listTypes.count (name)) return nullptr;  // type must be registered
  if (d->lists[sessId].count (name))
    return d->lists[sessId][name];  // list exists - return it

  // list doesn't exist yet - create it
  cList *list = d->listTypes[name]();  // call the list factory
  list->initRootGroup ();
  d->lists[sessId][name] = list;
  list->setSession (sessId);  // set list's session ID

  // also load list contents
  loadList (sessId, list);

  return list;
}

void cListManager::addSession (int sessId)
{
  if (d->lists.count (sessId)) return;  // already exists
  map<QString, cList *> empty;
  d->lists[sessId] = empty;

  // create every registered list in this session
  // this will ensure that data will be loaded, events hooked, and so on
  map<QString, cList *(*)()>::iterator it;
  for (it = d->listTypes.begin(); it != d->listTypes.end(); ++it)
    getList (sessId, it->first);
}

void cListManager::removeSession (int sessId)
{
  if (!d->lists.count (sessId)) return;
  map<QString, cList *>::iterator it;
  for (it = d->lists[sessId].begin(); it != d->lists[sessId].end(); ++it) {
    saveList (sessId, it->second);
    it->second->clear ();  // remove all elements from the list
    delete it->second;
  }
  d->lists.erase (sessId);
}

int cListManager::registerObject (cListObject *obj)
{
  int id = objectId (obj);
  if (id) return id;  // already registered
  id = ++d->maxObjectID;  // generate new object ID
  d->objectIDs[obj] = id;
  d->objects[id] = obj;
  return id;
}

void cListManager::unregisterObject (cListObject *obj)
{
  int id = objectId (obj);
  if (!id) return;  // not registered
  d->objectIDs.erase (obj);
  d->objects.erase (id);
}

int cListManager::objectId (cListObject *obj) const
{
  if (d->objectIDs.count (obj))
    return d->objectIDs[obj];
  return 0;
}

cListObject *cListManager::object (int id) const
{
  if (d->objects.count (id))
    return d->objects[id];
  return nullptr;
}

void cListManager::loadList (int sessId, cList *list)
{
  // load the list contents
  cProfileManager *pm = cProfileManager::self();
  
  QString path = pm->profilePath (sessId);
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  QString fName = list->name() + ".xml";

  QFile f (path + "/" + fName);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "No " << fName << " file - nothing to do.";
    list->listLoaded ();  // mark the list as loaded
    return;  // no profiles - nothing to do
  }

  QXmlStreamReader *reader = new QXmlStreamReader (&f);
  list->load (reader);

  f.close ();
  delete reader;
}

void cListManager::saveList (int sessId, cList *list)
{
  if (!list) return;

  // save the list contents
  cProfileManager *pm = cProfileManager::self();

  QString path = pm->profilePath (sessId);
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  dir.remove (list->name() + ".backup");
  QString fName = list->name() + ".xml";
  if (!QFile(path + "/" + fName).copy (path + "/" + list->name() + ".backup")) {
    qDebug() << "Unable to backup " << fName;  // not fatal, may simply not exist
  }

  QFile f (path + "/" + fName);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    qDebug() << "Unable to open " << (path + "/" + fName) << " for writing.";
    return;  // problem
  }

  QXmlStreamWriter *writer = new QXmlStreamWriter (&f);
  list->save (writer);

  f.close ();
  delete writer;
}

void cListManager::saveAll (int sessId)
{
  map<QString, cList *(*)()>::iterator it;
  for (it = d->listTypes.begin(); it != d->listTypes.end(); ++it)
    saveList (sessId, getList (sessId, it->first));
}



