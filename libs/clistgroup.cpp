//
// C++ Implementation: clistgroup
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

#include "clistgroup.h"

#include "clist.h"
#include "clistmanager.h"

#include <klocale.h>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

struct cListGroup::Private {
  QString tag;
  int objectCount;
  std::list<cListObject *> objects;
  std::list<cListObject *> priorityList;
};

cListGroup::cListGroup (cList *list) : cListObject (list)
{
  d = new Private;
  d->objectCount = 0;
}

cListGroup::~cListGroup()
{
  delete d;
}

void cListGroup::addObject (cListObject *obj)
{
  list()->notifyAdding (this, objectCount());
  d->objects.push_back (obj);
  generatePriorityList ();
  list()->addDone();
}

void cListGroup::removeObject (cListObject *obj)
{
  list()->notifyRemoving (obj);
  d->objects.remove (obj);
  generatePriorityList ();
  list()->removeDone ();
}

void cListGroup::objectChanged (cListObject *)
{
  generatePriorityList ();
}

const std::list<cListObject *> *cListGroup::objectList ()
{
  return &d->objects;
}

const std::list<cListObject *> *cListGroup::priorityList ()
{
  return &d->priorityList;
}

void cListGroup::updateVisibleName ()
{
  setVisibleName (name());
}

void cListGroup::setTag (const QString &tag)
{
  d->tag = tag;
}

QString cListGroup::tag ()
{
  return d->tag;
}

cListObject *cListGroup::objectAt (int pos)
{
  std::list<cListObject *>::iterator it = d->objects.begin();
  for (int p = 0; p < pos; ++p) ++it;
  if (it == d->objects.end()) return 0;   // hit end of list - no such object
  return *it;
}

int cListGroup::objectPosition (cListObject *obj)
{
  int pos = 0;
  std::list<cListObject *>::iterator it;
  for (it = d->objects.begin(); it != d->objects.end(); ++it) {
    if (*it == obj) return pos;
    pos++;
  }
  return -1;  // not found
}

void cListGroup::moveObjectUp (int pos)
{
  cListObject *obj = objectAt (pos);
  if (!obj) return;
  moveObjectToPosition (obj, pos - 1);
}

void cListGroup::moveObjectDown (int pos)
{
  cListObject *obj = objectAt (pos);
  if (!obj) return;
  moveObjectToPosition (obj, pos + 1);
}

void cListGroup::moveObjectToPosition (cListObject *obj, int pos)
{
  if (pos < 0) return;
  if (pos >= objectCount()) return;

  removeObject (obj); // need full removal with resorting, so that the model stays valid

  list()->notifyAdding (this, pos);
  std::list<cListObject *>::iterator it = d->objects.begin(), it2;
  for (int p = 0; p < pos; ++p) ++it;   // move to position
  d->objects.insert (it, obj);   // and insert the object there
  generatePriorityList ();
  list()->addDone();
  list()->notifyChanged (obj);
}

int cListGroup::objectCount () const
{
  return d->objectCount;
}

bool cListGroup::compareObjects (cListObject *a, cListObject *b)
{
  if (a->priority() != b->priority())
    return (a->priority() > b->priority());  // higher priority goes first
  return (a->positionInGroup() < b->positionInGroup());   // same priority - base ordering on position
}

void cListGroup::generatePriorityList ()
{
  // recompute object positions
  std::list<cListObject *>::iterator it;
  int pos = 0;
  for (it = d->objects.begin(), pos = 0; it != d->objects.end(); ++it, ++pos)
    (*it)->setPositionInGroup (pos);

  // also fix object count
  d->objectCount = d->objects.size();

  // recompute priorities
  d->priorityList.clear ();
  d->priorityList = d->objects;
  d->priorityList.sort (compareObjects);

  for (it = d->priorityList.begin(), pos = 0; it != d->priorityList.end(); ++it, ++pos)
    (*it)->setPriorityInGroup (pos);
}

cList::TraverseAction cListGroup::traverse (int traversalType)
{
  if (!enabled()) return cList::Continue;

  // create a copy of the priority list
  // this makes things slower, but it's necessary, as the callback could alter the list
  // TODO: implement some flag that would disable this behaviour for callbacks that do not alter the list ?
  // TODO: or perhaps simply prohibit such callbacks ?
  std::list<cListObject *> pl = d->priorityList;
  std::list<cListObject *>::iterator it;
  cListManager *lm = cListManager::self();
  for (it = pl.begin(); it != pl.end(); ++it) {
    cListObject *obj = *it;
    if (!lm->objectId (obj)) continue;  // object doesn't exist anymore ? skip it
    if (!obj->enabled()) continue; // object not enabled - continue
    cList::TraverseAction act = obj->traverse (traversalType);
    // we must stop - so stop
    if (act == cList::Stop) return cList::Stop;
    // we must leave the group - do so    
    if (act == cList::LeaveGroup) break;
  }
  // we're done here, tell the parent to continue
  return cList::Continue;
}

void cListGroup::load (QXmlStreamReader *reader)
{
  setEnabled (true);
  setPriority (DEFAULT_OBJECT_PRIORITY);  // revert priority to default

  QString en = reader->attributes().value ("enabled").toString();
  if ((!en.isEmpty()) && (en.toLower() == "false"))
    setEnabled (false);
  QString pri = reader->attributes().value ("priority").toString();
  if (!pri.isEmpty()) {
    int p = pri.toInt();
    if (p) setPriority (p);
  }

  // okay, now traverse child tags, if any
  while (!reader->atEnd()) {
    reader->readNext ();
    // Error ? Break out.
    if (reader->hasError()) break;
    // we're done with the tag
    if (reader->isEndElement() && (reader->name() == "group")) break;
    if (!reader->isStartElement()) continue;  // anything else than start of element - ignore it and continue with the next
    if (reader->name() == "group") {
      // a new group
      QString name = reader->attributes().value ("name").toString();
      if (name.isEmpty()) {
        reader->raiseError (i18n ("Error in file - group with no name."));
        return;
      }
      if (list()->group (name)) {
        reader->raiseError (i18n ("Error in file - duplicate group name."));
        return;
      }
      cListGroup *g = list()->addGroup (this, name);
      g->load (reader);
    } else if (reader->name() == "object") {
      // a new object
      cListObject *obj = list()->newObject();
      list()->addToGroup (this, obj);
      // set object name, if any
      QString name = reader->attributes().value ("name").toString();
      if (!name.isEmpty())
        list()->setObjectName (obj, name);
      // and load other parameters
      obj->load (reader);
    }
  }
}

void cListGroup::save (QXmlStreamWriter *writer)
{
  writer->writeStartElement ("group");

  writer->writeAttribute ("name", name());
  if (!enabled())
    writer->writeAttribute ("enabled", "false");
  if (priority() != DEFAULT_OBJECT_PRIORITY)
    writer->writeAttribute ("priority", QString::number (priority()));

  // save child elements
  std::list<cListObject *>::iterator it;
  for (it = d->objects.begin(); it != d->objects.end(); ++it)
    (*it)->save (writer);

  writer->writeEndElement ();  // end the group element
}


