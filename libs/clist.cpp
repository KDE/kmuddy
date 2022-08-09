//
// C++ Implementation: clist
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

#include "clist.h"

#include "clistgroup.h"
#include "clistmanager.h"

#include <KLocalizedString>

#include <QAbstractItemModel>
#include <QFont>
#include <QIcon>
#include <QMimeData>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <map>

using namespace std;

/** cListModel - the model providing access to a single list. */
class cListModel : public QAbstractItemModel {

  friend class cList;

  cListModel (cList *l) : QAbstractItemModel(), lst(l) {
  }

  QModelIndex index (int row, int column, const QModelIndex &parent = QModelIndex()) const override
  {
    if (!hasIndex(row, column, parent))
      return QModelIndex();

    cListGroup *group = parent.isValid() ? static_cast<cListGroup *>(parent.internalPointer()) : lst->rootGroup();

    cListObject *obj = group->objectAt (row);
    if (!obj) return QModelIndex();
    return createIndex (row, column, (void *) obj);
  }

  QModelIndex indexOf (const cListObject *obj) const
  {
    if (obj == lst->rootGroup()) return QModelIndex();
    if (!obj) return QModelIndex();
    return createIndex (obj->positionInGroup(), 0, (void *) obj);
  }

  QModelIndex parent (const QModelIndex &index) const override
  {
    if (!index.isValid()) return QModelIndex();
    cListObject *obj = static_cast<cListObject *>(index.internalPointer());
    cListGroup *group = obj->parentGroup ();
    if ((!group) || (group == lst->rootGroup()))  // root or top-level item
      return QModelIndex();
    return createIndex (group->positionInGroup(), 0, (void *) group);
  }

  int columnCount (const QModelIndex &) const override
  {
    return 1;  // we have one column
  }

  int rowCount (const QModelIndex &parent = QModelIndex()) const override
  {
    if (parent.column() > 0) return 0;  // we only have a single column

    cListObject *obj = parent.isValid() ? static_cast<cListObject *>(parent.internalPointer()) : lst->rootGroup();
    if (!obj) return 0;
    if (!obj->isGroup()) return 0;
    return static_cast<cListGroup *>(obj)->objectCount();
  }

  QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole) const override
  {
    cListObject *obj = index.isValid() ? static_cast<cListObject *>(index.internalPointer()) : lst->rootGroup();
    
    // DisplayRole - the visible name
    if (role == Qt::DisplayRole)
      return obj->visibleName();

    if (role == Qt::UserRole) {
      // here we return the object/group
      return qVariantFromValue (obj);
    }

    if (role == Qt::DecorationRole) {
      return obj->enabled() ? QIcon() : QIcon::fromTheme("dialog-cancel");
    }

    if (role == Qt::FontRole) {
      QFont bold;
      bold.setBold (true);
      return obj->isGroup() ? bold : QVariant();
    }

    return QVariant();
  }

  Qt::ItemFlags flags (const QModelIndex &index) const override
  {
    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    // anything can be dragged, only groups accept drops
    res |= Qt::ItemIsDragEnabled;
    cListObject *obj = index.isValid() ? static_cast<cListObject *>(index.internalPointer()) : lst->rootGroup();
    if (obj->isGroup()) res |= Qt::ItemIsDropEnabled;
    return res;
  }

  void startAddRows (const QModelIndex &parent, int from, int to)
  {
    //emit layoutAboutToBeChanged();
    beginInsertRows (parent, from, to);
  }

  void addedRows ()
  {
    endInsertRows ();
    //emit layoutChanged();
  }

  void startRemoveRows (const QModelIndex &parent, int from, int to)
  {
    emit layoutAboutToBeChanged();
    beginRemoveRows (parent, from, to);
  }

  void removedRows ()
  {
    endRemoveRows ();
    emit layoutChanged();
  }

  void notifyChanged (const QModelIndex &parent, int from, int to)
  {
    emit dataChanged (index (from, 0, parent), index (to, 0, parent));
  }

  // drag and drop
  Qt::DropActions supportedDropActions () const override
  {
    return Qt::MoveAction;
  }

  QStringList mimeTypes () const override
  {
    QStringList types;
    types << "application/kmuddy.object.info";
    return types;
  }

  QMimeData *mimeData (const QModelIndexList &indexes) const override
  {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream (&encodedData, QIODevice::WriteOnly);

    cListManager *lm = cListManager::self();
    for (QModelIndexList::const_iterator it = indexes.begin(); it != indexes.end(); ++it) {
      if (!(*it).isValid()) continue;
      cListObject *obj = static_cast<cListObject *>((*it).internalPointer());
      int id = lm->objectId (obj);
      if (!id) continue;
      stream << id;
    }
    
    mimeData->setData ("application/kmuddy.object.info", encodedData);
    return mimeData;
  }

  bool dropMimeData (const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override
  {
    if (action == Qt::IgnoreAction) return true;
    if (!data->hasFormat("application/kmuddy.object.info")) return false;
    if (column > 0) return false;

    cListObject *grp = parent.isValid() ? static_cast<cListObject *>(parent.internalPointer()) : lst->rootGroup();
    if (!grp->isGroup()) {  // we dropped onto an end-object - can this ever happen ?
      // will insert after this item
      row = grp->positionInGroup() + 1;
      grp = grp->parentGroup ();
    }
    cListGroup *group = (cListGroup *) grp;
    QByteArray encodedData = data->data ("application/kmuddy.object.info");
    QDataStream stream (&encodedData, QIODevice::ReadOnly);
    
    cListManager *lm = cListManager::self();
    while (!stream.atEnd()) {
      // fetch an object ID from the dropped data
      int id;
      stream >> id;
      cListObject *obj = lm->object (id);
      // ensure that the object is valid and belongs to the same list
      if (!obj) continue;
      if (obj->list() != lst) continue;

      // alright, we need to move this object to the designated position
      // also adjust the designated position if needed, so that the next object is placed correctly
      bool sameGroup = (group == obj->parentGroup ());
      if (!sameGroup) lst->addToGroup (group, obj);
      // adjust row number so that everything works well when moving within the same group
      if (sameGroup && obj->positionInGroup() < row) row--;
      if (row >= 0)
        group->moveObjectToPosition (obj, row++);
    }
    return true;
  }

 private:
  cList *lst;

};

struct cList::Private
{
  bool enabled;
  QString name;
  int sess;
  cListModel *model;
  map<QString, cListProperty> propertyList;

  cListGroup *rootGroup;
  map<QString, cListGroup *> groups;
  map<QString, cListObject *> namedObjects;

  QString lastError;
  bool hasError;
};

cList::cList (const QString &name)
{
  d = new Private;
  d->enabled = true;
  d->name = name;
  d->model = new cListModel (this);
  d->rootGroup = nullptr;
  d->sess = 0;
  d->hasError = false;
}

// initialize the root group
// this cannot be in the constructor, because the cListObject constructor
// is calling our pure virtual method, which can only be done when the object
// has been fully constructed
void cList::initRootGroup ()
{
  if (d->rootGroup) return;
  d->rootGroup = new cListGroup (this);
  d->rootGroup->setName ("Root");
  d->groups["Root"] = d->rootGroup;
}

cList::~cList ()
{
  // clear ();  // not called here, so that we can still use the inherited list in cListObject-derived class destructors
  delete d->rootGroup;
  delete d->model;
  delete d;
}

void cList::setSession (int sess)
{
  d->sess = sess;
}

int cList::session ()
{
  return d->sess;
}

cListGroup *cList::newGroup ()
{
  return new cListGroup (this);
}

QString cList::name ()
{
  return d->name;
}

const std::map<QString, cListProperty> &cList::getPropertyList ()
{
  return d->propertyList;
}

int cList::defaultIntValue (const QString &name)
{
  if (!d->propertyList.count (name)) return 0;
  if (d->propertyList[name].type != Int) return 0;
  return d->propertyList[name].defIntValue;
}

QString cList::defaultStrValue (const QString &name)
{
  if (!d->propertyList.count (name)) return QString();
  if (d->propertyList[name].type != String) return QString();
  return d->propertyList[name].defStrValue;
}

bool cList::defaultBoolValue (const QString &name)
{
  if (!d->propertyList.count (name)) return false;
  if (d->propertyList[name].type != Bool) return false;
  return d->propertyList[name].defBoolValue;
}

bool cList::enabled ()
{
  return d->enabled;
}

void cList::setEnabled (bool en)
{
  d->enabled = en;
}

cListGroup *cList::rootGroup ()
{
  return d->rootGroup;
}

cListGroup *cList::group (const QString &name)
{
  if (d->groups.count (name))
    return d->groups[name];
  return nullptr;
}

cListGroup *cList::addGroup (cListGroup *parent, const QString &name)
{
  cListGroup *g = group (name);
  if (g) return g;  // group already exists
  g = newGroup ();
  g->setName (name);
  g->setParentGroup (parent);
  d->groups[name] = g;
  return g;
}

bool cList::renameGroup (cListGroup *group, const QString &newName)
{
  if (d->groups.count (newName)) return false;
  if (group == d->rootGroup) return false;
  d->groups.erase (group->name());
  group->setName (newName);
  d->groups[newName] = group;
  return true;
}

void cList::removeGroup (cListGroup *group)
{
  if (group == d->rootGroup) return;  // the root group cannot be removed

  // reparent all childs to the parent group
  cListGroup *parent = group->parentGroup ();
  const std::list<cListObject *> *objects = group->objectList ();
  std::list<cListObject *> moveList = *objects;  // make a copy of the list
  // the copy is made so that the iterator doesn't get invalidated
  std::list<cListObject *>::iterator it;
  for (it = moveList.begin(); it != moveList.end(); ++it)
    (*it)->setParentGroup (parent);

  // the group is empty now - remove
  d->groups.erase (group->name());
  delete group;
}

void cList::addToGroup (cListGroup *group, cListObject *item)
{
  if (item == d->rootGroup) return;
  item->setParentGroup (group);
}

bool cList::setObjectName (cListObject *obj, const QString &name)
{
  if (obj->list() != this) return false;
  if (d->namedObjects.count (name)) return false;  // name already exists
  if (obj->isGroup()) return false;  // not to be used on groups
  if (!obj->name().isEmpty())
    d->namedObjects.erase (obj->name());
  obj->setName (name);
  if (!name.isEmpty())
    d->namedObjects[name] = obj;
  return true;
}

cListObject *cList::getObject (const QString &name)
{
  if (d->namedObjects.count (name))
    return d->namedObjects[name];
  return nullptr;
}

void cList::deleteObject (cListObject *obj)
{
  if (obj->list() != this) return;  // must be one of ours
  if (obj->isGroup()) return;  // must not be a group
  delete obj;
}

void cList::clear ()
{
  // first, remove all the groups; this moves all the objects to the root group
  // list copying is done to prevent iterator invalidation
  list<cListGroup *> g;
  std::map<QString, cListGroup *>::iterator it;
  for (it = d->groups.begin(); it != d->groups.end(); ++it)
    g.push_back (it->second);
  std::list<cListGroup *>::iterator itl;
  for (itl = g.begin(); itl != g.end(); ++itl)
    removeGroup (*itl);
  
  // second, delete all the objects
  const std::list<cListObject *> *objects = d->rootGroup->objectList ();
  std::list<cListObject *> moveList = *objects;
  std::list<cListObject *>::iterator ito;
  for (ito = moveList.begin(); ito != moveList.end(); ++ito)
    deleteObject (*ito);

  // finally, clear the list of named objects
  d->namedObjects.clear ();
}

void cList::traverse (int traversalType)
{
  if (!enabled()) return;  // list must be enabled
  d->rootGroup->traverse (traversalType);
}

void cList::load (QXmlStreamReader *reader)
{
  // remove all existing elements
  clear ();
  d->hasError = false;

  reader->readNext ();  // read the document start
  reader->readNext ();
  if (reader->isStartElement ())
    if (reader->name() == "list")
      if (reader->attributes().value ("version") == "1.0") {
        // all is well, we can start loading the list
        // so read the root group
        do {
          reader->readNext ();
        } while (!(reader->isStartElement () || reader->atEnd()));
        if (reader->isStartElement () && (reader->name() == "group"))
          d->rootGroup->load (reader);
        else
          reader->raiseError (i18n ("This file does not contain the root group, and therefore cannot be loaded."));
      }
      else
        reader->raiseError (i18n ("This file was created by a newer version of KMuddy, and this version is unable to open it."));
    else
      reader->raiseError (i18n ("This is not a KMuddy object file."));
  else if (!reader->hasError())
    reader->raiseError (i18n ("This file is corrupted."));

  if (reader->hasError()) {
    d->hasError = true;
    d->lastError = i18n ("Error at line %1, column %2: %3",
                            QString::number (reader->lineNumber()),
                            QString::number (reader->columnNumber()),
                            reader->errorString());
  }

  listLoaded ();  // the list is loaded now
}

void cList::save (QXmlStreamWriter *writer)
{
  writer->setAutoFormatting (true);  // make the generated XML more readable
  writer->writeStartDocument ();

  writer->writeStartElement ("list");
  writer->writeAttribute ("version", "1.0");

  d->rootGroup->save (writer);

  writer->writeEndElement ();  // end the list element
  writer->writeEndDocument ();
}

bool cList::hasError ()
{
  return d->hasError;
}

void cList::clearError ()
{
  d->hasError = false;
}

const QString cList::lastError ()
{
  return d->lastError;
}

void cList::addProperty (const cListProperty &prop)
{
  d->propertyList[prop.name] = prop;
}

void cList::addIntProperty (const QString &name, const QString &desc, int defaultValue)
{
  cListProperty p;
  p.name = name;
  p.desc = desc;
  p.type = Int;
  p.defIntValue = defaultValue;
  addProperty (p);
}

void cList::addStringProperty (const QString &name, const QString &desc, QString defaultValue)
{
  cListProperty p;
  p.name = name;
  p.desc = desc;
  p.type = String;
  p.defStrValue = defaultValue;
  addProperty (p);
}

void cList::addBoolProperty (const QString &name, const QString &desc, bool defaultValue)
{
  cListProperty p;
  p.name = name;
  p.desc = desc;
  p.type = Bool;
  p.defBoolValue = defaultValue;
  addProperty (p);
}

void cList::addObject (cListObject *obj)
{
  obj->updateVisibleName ();
}

void cList::removeObject (cListObject *obj)
{
  if (!obj->name().isEmpty())
    d->namedObjects.erase (obj->name());
}

QAbstractItemModel *cList::model ()
{
  return d->model;
}

cListObject *cList::objectAt (const QModelIndex &index)
{
  QVariant data = d->model->data (index, Qt::UserRole);
  cListObject *obj = data.value<cListObject *>();
  return obj;
}

QModelIndex cList::indexOf (const cListObject *obj)
{
  return d->model->indexOf (obj);
}

void cList::notifyAdding (cListGroup *group, int pos)
{
  d->model->startAddRows (d->model->indexOf (group), pos, pos);
}

void cList::addDone ()
{
  d->model->addedRows();
}

void cList::notifyRemoving (cListObject *obj)
{
  cListGroup *group = obj->parentGroup();
  int pos = obj->positionInGroup();
  d->model->startRemoveRows (d->model->indexOf (group), pos, pos);
}

void cList::removeDone ()
{
  d->model->removedRows();
}

void cList::notifyChanged (cListObject *obj)
{
  cListGroup *group = obj->parentGroup();
  int pos = obj->positionInGroup();
  d->model->notifyChanged (d->model->indexOf (group), pos, pos);
}

