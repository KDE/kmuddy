//
// C++ Implementation: clistobject
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

#include "clistobject.h"

#include "clistgroup.h"
#include "clist.h"
#include "clistmanager.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

struct cListObject::Private {
  cList *list;
  cListGroup *parent;

  cListObjectData data;
  int groupPos, groupPriorityPos;
  QString visibleName;
};

cListObject::cListObject (cList *list)
{
  d = new Private;
  d->list = list;
  d->parent = nullptr;
  d->data.enabled = true;
  d->groupPos = 0;
  d->groupPriorityPos = 0;
  d->data.priority = DEFAULT_OBJECT_PRIORITY;

  d->visibleName = list->objName ();
  
  d->list->addObject (this);
  cListManager::self()->registerObject (this);
}

cListObject::~cListObject ()
{
  d->data.intValues.clear ();
  d->data.strValues.clear ();
  d->data.boolValues.clear ();
  if (d->parent) d->parent->removeObject (this);
  d->list->removeObject (this);
  cListManager::self()->unregisterObject (this);

  delete d;
}

cList *cListObject::list ()
{
  return d->list;
}

cListGroup *cListObject::parentGroup ()
{
  return d->parent;
}

int cListObject::priority ()
{
  return d->data.priority;
}

void cListObject::setPriority (int p)
{
  if (p < 1) p = 1;
  if (p > 1000) p = 1000;
  d->data.priority = p;
  if (d->parent)
    d->parent->objectChanged (this);
}

void cListObject::setName (const QString &n)
{
  d->data.name = n;
  updateVisibleName ();
}

QString cListObject::name ()
{
  return d->data.name;
}

QString cListObject::visibleName ()
{
  return d->visibleName;
}

void cListObject::setVisibleName (const QString &name)
{
  d->visibleName = name;
  emit changed (this);
  list()->notifyChanged (this);
}

void cListObject::updateVisibleName ()
{
  // default implementation simply uses object type name
  setVisibleName (list()->objName());
}

int cListObject::intVal (const QString &name)
{
  if (d->data.intValues.count (name))
    return d->data.intValues[name];
  return d->list->defaultIntValue (name);
}

void cListObject::setInt (const QString &name, int value)
{
  if (value == d->list->defaultIntValue (name))
    clearInt (name);
  else
    d->data.intValues[name] = value;
  emit changed (this);
  attribChanged (name);
}

void cListObject::clearInt (const QString &name)
{
  d->data.intValues.erase (name);
}

QString cListObject::strVal (const QString &name)
{
  if (d->data.strValues.count (name))
    return d->data.strValues[name];
  return d->list->defaultStrValue (name);
}

void cListObject::setStr (const QString &name, const QString &value)
{
  if (value == d->list->defaultStrValue (name))
    clearStr (name);
  else
    d->data.strValues[name] = value;
  emit changed (this);
  attribChanged (name);
}

void cListObject::clearStr (const QString &name)
{
  d->data.strValues.erase (name);
}

bool cListObject::boolVal (const QString &name)
{
  if (d->data.boolValues.count (name))
    return d->data.boolValues[name];
  return d->list->defaultBoolValue (name);
}

void cListObject::setBool (const QString &name, bool value)
{
  if (value == d->list->defaultBoolValue (name))
    clearBool (name);
  else
    d->data.boolValues[name] = value;
  emit changed (this);
  attribChanged (name);
}

void cListObject::clearBool (const QString &name)
{
  d->data.boolValues.erase (name);
}

int cListObject::strListCount (const QString &name)
{
  return intVal (name + "-count");
}

void cListObject::setStrListCount (const QString &name, int count)
{
  int oldCount = strListCount (name);
  // clear data that's removed, if any
  for (int i = count + 1; i <= oldCount; ++i)
    clearStr (name + "-" + QString::number(i));
  setInt (name + "-count", count);
}

QString cListObject::strListValue (const QString &name, int which)
{
  return strVal (name + "-" + QString::number (which));
}

void cListObject::setStrListValue (const QString &name, int which, const QString &value)
{
  if (which <= 0) return;
  if (which > strListCount (name)) setStrListCount (name, which);

  setStr (name + "-" + QString::number(which), value);
}

void cListObject::clearStrList (const QString &name)
{
  setStrListCount (name, 0);
}

void cListObject::attribChanged (const QString &)
{
  // nothing here - childs may override
}

cListObjectData cListObject::data ()
{
  return d->data;
}

bool cListObject::enabled ()
{
  return d->data.enabled;
}

void cListObject::setEnabled (bool en)
{
  bool e = d->data.enabled;
  d->data.enabled = en;
  
  if (e != en) {
    en ? objectEnabled () : objectDisabled ();
    emit changed (this);
    list()->notifyChanged (this);
  }
}

void cListObject::setParentGroup (cListGroup *group)
{
  if (d->parent == group) return;  // already there ? do nothing
  if (d->parent)
    d->parent->removeObject (this);
  d->parent = group;
  d->parent->addObject (this);
  objectMoved ();
}

void cListObject::setPositionInGroup (int pos)
{
  int oldpos = d->groupPos;
  d->groupPos = pos;
  if (oldpos != pos) objectMoved ();
}

int cListObject::positionInGroup () const

{
  return d->groupPos;
}

void cListObject::setPriorityInGroup (int pos)
{
  int oldpos = d->groupPriorityPos;
  d->groupPriorityPos = pos;
  if (oldpos != pos) objectMoved ();
}

int cListObject::priorityInGroup () const
{
  return d->groupPriorityPos;
}

void cListObject::objectMoved ()
{
  // nothing here - subclasses may override
}

cList::TraverseAction cListObject::traverse (int)
{
  return cList::Stop;
}

void cListObject::load (QXmlStreamReader *reader)
{
  setEnabled (true);
  setPriority (DEFAULT_OBJECT_PRIORITY);  // revert priority to default

  QString en = reader->attributes().value ("enabled").toString();
  if ((!en.isEmpty()) && (en.toLower() == QString("false")))
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
    if (reader->isEndElement() && (reader->name() == QString("object"))) break;
    if (!reader->isStartElement()) continue;  // anything else than start of element - ignore it and continue with the next
    if (reader->name() == QString("attrib")) {
      QStringView type = reader->attributes().value ("type");
      QString name = reader->attributes().value ("name").toString();
      QString value;
      while (!reader->isEndElement()) {
        // read the data
        // the data may be split into parts, if it contains entities
        reader->readNext ();
        if (reader->isCharacters()) value += reader->text().toString();
      }
      if (type == QString("bool"))
        setBool (name, (value.toLower() == "true"));
      else if (type == QString("int"))
        setInt (name, value.toInt());
      else
        setStr (name, value);
    }
  }
}

void cListObject::save (QXmlStreamWriter *writer)
{
  writer->writeStartElement ("object");

  if (!name().isEmpty()) writer->writeAttribute ("name", name());
  if (!enabled())
    writer->writeAttribute ("enabled", "false");
  if (priority() != DEFAULT_OBJECT_PRIORITY)
    writer->writeAttribute ("priority", QString::number (priority()));

  // save the attributes
  std::map<QString, int>::iterator it1;
  std::map<QString, QString>::iterator it2;
  std::map<QString, bool>::iterator it3;
  for (it1 = d->data.intValues.begin(); it1 != d->data.intValues.end(); ++it1)
  {
    writer->writeStartElement ("attrib");
    writer->writeAttribute ("type", "int");
    writer->writeAttribute ("name", it1->first);
    writer->writeCharacters (QString::number (it1->second));
    writer->writeEndElement ();
  }
  for (it2 = d->data.strValues.begin(); it2 != d->data.strValues.end(); ++it2)
  {
    writer->writeStartElement ("attrib");
    writer->writeAttribute ("type", "string");
    writer->writeAttribute ("name", it2->first);
    writer->writeCharacters (it2->second);
    writer->writeEndElement ();
  }
  for (it3 = d->data.boolValues.begin(); it3 != d->data.boolValues.end(); ++it3)
  {
    writer->writeStartElement ("attrib");
    writer->writeAttribute ("type", "bool");
    writer->writeAttribute ("name", it3->first);
    writer->writeCharacters (it3->second ? "true" : "false");
    writer->writeEndElement ();
  }

  writer->writeEndElement ();  // end the object element
}

#include "moc_clistobject.cpp"
