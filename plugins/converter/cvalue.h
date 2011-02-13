//
// C++ Interface: cValue, cValueList
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
#ifndef CVALUE_H
#define CVALUE_H

class cValueData;
class KConfigGroup;
class QXmlStreamWriter;

#include <qstring.h>

/**
Class cValue holds one value, used in variables and in function evaluator.

@author Tomas Mecir
*/
class cValue {
 public:
  /** constructor */
  cValue ();
  cValue (const cValue &val);
  cValue (const QString &val);
  cValue (int val);
  cValue (double val);
  cValue (bool val);

  /** destructor */
  ~cValue ();

  static cValue *load (KConfigGroup *g);
  void save (QXmlStreamWriter *writer, const QString &name);

  /** assignment operator */
  cValue &operator= (const cValue &a);

  /** set value to nothing */
  void setValue ();
  /** set value to a string */
  void setValue (const QString &val);
  /** set value to an integer */
  void setValue (int val);
  /** set value to a number */
  void setValue (double val);
  /** set value to a copy of a given value */
  void setValue (const cValue &val);
  void setAsMarker ();
  /** set one particular item in an array */
  void setItem (int index, const QString &value);
  /** remove an item from an array */
  void removeItem (int index);
  /** add an item to the list */
  void addToList (const QString &item);
  /** remove an item from the list */
  void removeFromList (const QString &item);
  /** does the list contain a given string ? */
  bool listContains (const QString &item);

  QString asString () const;
  int asInteger () const;
  double asDouble () const;
  /** Return array item with the given index, if any. Arrays only. */
  QString item (int index) const;
  /** Does the list contain a given value ? Lists only.*/
  bool contains (const QString &item) const;
  /** Number of entries in a list/array. */
  int size () const;
 
  /** join a list into a string, using the given separator */
  QString listJoin (const QString &sep) const;
  /** convert a string to a list, using the given separator */
  cValue toList (const QString &sep) const;

  bool isEmpty() const;
  bool isString() const;
  bool isInteger() const;
  bool isDouble() const;
  bool isArray() const;
  bool isList() const;
  bool isMarker() const;

  /// some operators ...
  cValue operator[] (int index) const;
  
  static cValue empty () { return _empty; };
 protected:

  /** detach from current value, deleting it if no one else uses it */
  void detachValue ();
  /** ensure that no one else uses our value */
  void unique ();
    
  cValueData *d;
  
  static cValue _empty;
};

/// some external operators ...
cValue operator+ (const cValue &a, const cValue &b);
cValue operator- (const cValue &a, const cValue &b);
cValue operator* (const cValue &a, const cValue &b);
cValue operator/ (const cValue &a, const cValue &b);


#endif
