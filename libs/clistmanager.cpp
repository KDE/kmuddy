//
// C++ Implementation: clistmanager
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "clistmanager.h"

#include "clist.h"
#include "cprofilemanager.h"

#include <map>

#include <QDir>
#include <QFile>
#include <QStandardItemModel>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <kdebug.h>

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

cListManager *cListManager::_self = 0;

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
  if (!d->lists.count (sessId)) return 0;  // session must exist
  if (!d->listTypes.count (name)) return 0;  // type must be registered
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
  return 0;
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
    kDebug() << "No " << fName << " file - nothing to do." << endl;
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
    kDebug() << "Unable to backup " << fName << endl;  // not fatal, may simply not exist
  }

  QFile f (path + "/" + fName);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    kDebug() << "Unable to open " << (path + "/" + fName) << " for writing." << endl;
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



