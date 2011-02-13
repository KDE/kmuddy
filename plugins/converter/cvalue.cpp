//
// C++ Implementation: cValue, cValueList
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
#include "cvalue.h"

#include <kconfiggroup.h>

#include <QXmlStreamWriter>

#include <map>
#include <set>

using namespace std;

class cValueData {
 private:
  cValueData ();
  ~cValueData ();
  /** copy the value */
  void copyTo (cValueData *data);
  /** clear data */
  void clear ();

  enum ValueType { ValueNone, ValueString, ValueInt, ValueDouble, ValueArray, ValueList, ValueMarker };
  
  /** return type of currently held value */
  ValueType valueType () const { return valType; };
  bool isEmpty() const { return (valType==ValueNone); };
  bool isString() const { return (valType==ValueString); };
  bool isInteger() const { return (valType==ValueInt); };
  bool isDouble() const { return (valType==ValueDouble); };
  bool isArray() const { return (valType==ValueArray); };
  bool isList() const { return (valType==ValueList); };
  bool isMarker() const { return (valType==ValueMarker); };

  QString asString () const;
  int asInteger () const;
  double asDouble () const;

  QString listJoin (const QString &sep) const;

  int usage;
  ValueType valType;
  union {
    int int_val;
    double dbl_val;
  };
  // these cannot be in the union:
  map<int, QString> array_val;
  set<QString> list_val;
  QString str_val;
  
  /** no implementation here - we don't want to create copies */
  cValueData (const cValueData &val);
  /** no implementation here - we don't want to create copies */
  cValueData &operator= (const cValueData &val);
  
  friend class cValue;
};

cValueData::cValueData ()
{
  usage = 1;
  valType = ValueNone;
}

cValueData::~cValueData ()
{
  clear ();
}

void cValueData::clear ()
{
  valType = ValueNone;
  str_val = QString();
  list_val.clear ();
  array_val.clear ();
}


void cValueData::copyTo (cValueData *data)
{
  if (data == this) return;
  
  data->clear ();
  data->valType = valType;
  switch (valType) {
    case ValueNone:
    case ValueMarker:
    break;
    case ValueString:
      data->str_val = str_val;
    break;
    case ValueInt:
      data->int_val = int_val;
    break;
    case ValueDouble:
      data->dbl_val = dbl_val;
    break;
    case ValueArray:
      data->array_val = array_val;
    break;
    case ValueList:
      data->list_val = list_val;
    break;
  };
}

QString cValueData::asString () const
{
  QString ret = QString();
  map<int, QString>::const_iterator ita;
  set<QString>::const_iterator itl;
  switch (valType) {
    case ValueNone:
    case ValueMarker:
      ret = QString();
    break;
    case ValueString:
      ret = str_val;
    break;
    case ValueInt:
      ret.setNum (int_val);
    break;
    case ValueDouble:
      ret.setNum (dbl_val);
    break;
    case ValueArray:
    case ValueList:
      ret = listJoin ("|");
    break;
  }
  return ret;
}

int cValueData::asInteger () const
{
  bool ok = false;
  int ret = 0;
  switch (valType) {
    case ValueNone:
    case ValueMarker:
      ret = 0;
      break;
    case ValueString:
      ret = str_val.toInt (&ok);
      if (!ok) ret = 0;
      break;
    case ValueInt:
      ret = int_val;
      break;
    case ValueDouble:
      ret = (int) dbl_val;  //should auto-trim digits after decimal separator
      break;
    case ValueArray:
      ret = array_val.size();
      break;
    case ValueList:
      ret = list_val.size();
      break;
  }
  return ret;
}

double cValueData::asDouble () const
{
  double ret = 0.0;
  bool ok;
  switch (valType) {
    case ValueNone:
    case ValueMarker:
      ret = 0.0;
      break;
    case ValueString:
      ret = str_val.toDouble (&ok);
      if (!ok) ret = 0.0;
      break;
    case ValueInt:
      ret = (double) int_val;
      break;
    case ValueDouble:
      ret = dbl_val;
      break;
    case ValueArray:
      ret = array_val.size();
      break;
    case ValueList:
      ret = list_val.size();
      break;
  }
  return ret;
}

QString cValueData::listJoin (const QString &sep) const
{
  QString ret;
  map<int, QString>::const_iterator ita;
  set<QString>::const_iterator itl;
  if (valType == ValueArray) {
    ita = array_val.begin();
    if (ita == array_val.end())  // empty array ?
      return ret;;
    ret = ita->second;
    ++ita;
    for (; ita != array_val.end(); ++ita) ret += sep + ita->second;
  }
  if (valType == ValueList) {
    itl = list_val.begin();
    if (itl == list_val.end())  // empty list ?
      return ret;
    ret = (*itl);
    ++itl;
    for (; itl != list_val.end(); ++itl) ret += sep + (*itl);
  }
  return ret;
}


// *********************************
// cValue implementation starts here
// *********************************

cValue cValue::_empty;

cValue::cValue ()
{
  d = 0;
}

cValue::cValue (const cValue &val)
{
  d = val.d;
  if (d) d->usage++;
}

cValue::cValue (const QString &val)
{
  d = 0;
  setValue (val);
}

cValue::cValue (int val)
{
  d = 0;
  setValue (val);
}

cValue::cValue (double val)
{
  d = 0;
  setValue (val);
}

cValue::cValue (bool val)
{
  d = 0;
  setValue (val ? 1 : 0);
}

cValue::~cValue ()
{
  // detach from value, deleting it if needed
  detachValue ();
}

cValue *cValue::load (KConfigGroup *g)
{
  cValue *val = 0;
  int type = g->readEntry ("Type", 0);
  int cnt;
  switch (type) {
    case 0: // string (or old-style value)
      val = new cValue (g->readEntry ("Value", QString()));
    break;
    case 1: // integer
      val = new cValue (g->readEntry ("Integer value", 0));
    break;
    case 2: // double
      val = new cValue (g->readEntry ("Double value", 0.0));
    break;
    case 3: // array
      val = new cValue;
      cnt = g->readEntry ("Size", 0);
      for (int i = 1; i <= cnt; ++i) {
        int index = g->readEntry ("Array index " + QString::number (i), 0);
        QString v = g->readEntry ("Array value " + QString::number (i));
        val->setItem (index, v);
      }
    break;
    case 4: // list
      val = new cValue;
      cnt = g->readEntry ("Size", 0);
      for (int i = 1; i <= cnt; ++i) {
        QString v = g->readEntry ("List value " + QString::number (i));
        val->addToList (v);
      }
    break;
  }
  return val;
}

void cValue::save (QXmlStreamWriter *writer, const QString &name)
{
  writer->writeStartElement ("variable");
  if (!name.isEmpty()) writer->writeAttribute ("name", name);
  
  int type = 0;
  if (d) switch (d->valType) {
    case cValueData::ValueNone: type = 0; break;
    // markers are only used in function evaluation, no need to save them
    case cValueData::ValueMarker: type = 0; break;
    case cValueData::ValueString: type = 0; break;
    case cValueData::ValueInt: type = 1; break;
    case cValueData::ValueDouble: type = 2; break;
    case cValueData::ValueArray: type = 3; break;
    case cValueData::ValueList: type = 4; break;
    default: type = 0; break;  // should never happen
  };
  writer->writeAttribute ("type", QString::number (type));
  map<int, QString>::iterator it1;
  set<QString>::iterator it2;
  switch (type) {
    case 0: {  // string
      writer->writeAttribute ("value", asString());
    } break;
    case 1: {  // integer
      writer->writeAttribute ("value", QString::number (asInteger()));
    } break;
    case 2: {  // double
      writer->writeAttribute ("value", QString::number (asDouble(), 'g', 15));
    } break;
    case 3: {  // array
      for (it1 = d->array_val.begin(); it1 != d->array_val.end(); ++it1) {
        writer->writeStartElement ("element");
        writer->writeAttribute ("index", QString::number (it1->first));
        writer->writeAttribute ("value", it1->second);
        writer->writeEndElement ();
      }
    } break;
    case 4: {  // list
      for (it2 = d->list_val.begin(); it2 != d->list_val.end(); ++it2) {
        writer->writeStartElement ("element");
        writer->writeAttribute ("value", *it2);
        writer->writeEndElement ();
      }
    } break;
  }
  writer->writeEndElement ();
}


cValue &cValue::operator= (const cValue &a)
{
  // handle the a=a; type of assignments correctly
  if (a.d == d) return *this;
  
  detachValue ();
  d = a.d;
  if (d) d->usage++;
  return *this;
}

void cValue::setValue (const cValue &val)
{
  operator= (val);
}

void cValue::setValue ()
{
  detachValue ();
  d = 0;
}

void cValue::setValue (const QString &val)
{
  // first of all, try to convert the string to a number ...
  bool ok = false;
  double value = val.toDouble (&ok);
  if (ok) {
    // success - set the value as a number
    setValue (value);
    return;
  }  
  detachValue ();
  d = new cValueData;
  d->valType = cValueData::ValueString;
  d->str_val = val;
}

void cValue::setValue (int val)
{
  detachValue ();
  d = new cValueData;
  d->valType = cValueData::ValueInt;
  d->int_val = val;
}

void cValue::setValue (double val)
{
  detachValue ();
  d = new cValueData;
  d->valType = cValueData::ValueDouble;
  d->dbl_val = val;
}

void cValue::setAsMarker ()
{
  detachValue ();
  d = new cValueData;
  d->valType = cValueData::ValueMarker;
}

bool cValue::isEmpty() const
{
  if (d) return d->isEmpty();
  return true;
}

bool cValue::isString() const
{
  if (d) return d->isString();
  return false;
}

bool cValue::isInteger() const
{
  if (d) return d->isInteger();
  return false;
}

bool cValue::isDouble() const
{
  if (d) return d->isDouble();
  return false;
}

bool cValue::isArray() const
{
  if (d) return d->isArray();
  return false;
}

bool cValue::isList() const
{
  if (d) return d->isList();
  return false;
}

bool cValue::isMarker() const
{
  if (d) return d->isMarker();
  return false;
}

cValue cValue::operator[] (int index) const
{
  return item (index);
}

void cValue::setItem (int index, const QString &value)
{
  if (!isArray()) {
    detachValue ();  // not an array - get rid of the old value
    d = new cValueData;
    d->valType = cValueData::ValueArray;
  } else
    removeItem (index);
  d->array_val[index] = value;
}

void cValue::removeItem (int index)
{
  if (!isArray()) return;
  if (d->array_val.count (index) != 0)
    d->array_val.erase (index);
}

void cValue::addToList (const QString &item)
{
  if (!isList()) {
    detachValue ();  // not a list - get rid of the old value
    d = new cValueData;
    d->valType = cValueData::ValueList;
  }
  d->list_val.insert (item);
}

void cValue::removeFromList (const QString &item)
{
  if (!isList()) return;
  d->list_val.erase (item);
}

bool cValue::listContains (const QString &item)
{
  if (!isList()) return false;
  return (d->list_val.count(item) != 0);
}


QString cValue::asString () const
{
  if (d) return d->asString ();
  return QString();
}

int cValue::asInteger () const
{
  if (d) return d->asInteger ();
  return 0;
}

double cValue::asDouble () const
{
  if (d) return d->asDouble ();
  return 0.0;
}

QString cValue::item (int index) const
{
  if (!isArray()) return QString();
  if (d->array_val.count (index) != 0)
    return d->array_val[index];
  return QString();
}

bool cValue::contains (const QString &item) const
{
  if (!isList()) return false;
  return (d->list_val.count (item) != 0);
}

int cValue::size () const
{
  if (isArray ())
    return d->array_val.size();
  if (isList ())
    return d->list_val.size();
  if (isEmpty ())
    return 0;
  return 1;
}

QString cValue::listJoin (const QString &sep) const
{
  if ((!isList()) && (!isArray())) return QString();
  return d->listJoin (sep);
}

cValue cValue::toList (const QString &sep) const
{
  QStringList list = asString().split (sep);
  QStringList::iterator it;
  cValue val;
  for (it = list.begin(); it != list.end(); ++it)
    val.addToList (*it);
  return val;
}

void cValue::detachValue ()
{
  if (!d) return;
  if (d->usage > 0) d->usage--;
  if (d->usage == 0) delete d;
  d = 0;
}

void cValue::unique ()
{
  if (!d) return;
  if (d->usage <= 1) return;
  cValueData *vd = new cValueData;
  d->copyTo (vd);
  detachValue ();
  d = vd;
}

cValue operator+ (const cValue &a, const cValue &b)
{
  if (a.isInteger() && b.isInteger())
    return cValue (a.asInteger() + b.asInteger());
  return cValue (a.asDouble() + b.asDouble());
}

cValue operator- (const cValue &a, const cValue &b)
{
  if (a.isInteger() && b.isInteger())
    return cValue (a.asInteger() - b.asInteger());
  return cValue (a.asDouble() - b.asDouble());
}

cValue operator* (const cValue &a, const cValue &b)
{
  return cValue (a.asDouble() * b.asDouble());
}

cValue operator/ (const cValue &a, const cValue &b)
{
  double bb = b.asDouble ();
  if (bb != 0)
    return cValue (a.asDouble() / bb);
  return cValue (0);
}

