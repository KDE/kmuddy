//
// C++ Interface: cValue, cValueList
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CVALUE_H
#define CVALUE_H

class cValueData;
class QXmlStreamWriter;
class QXmlStreamReader;

#include <qstring.h>
#include <kmuddy_export.h>

/**
Class cValue holds one value, used in variables and in function evaluator.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cValue {
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

  /** Load data into a variable. Returns variable name, if given. */
  QString load (QXmlStreamReader *reader);
  void save (QXmlStreamWriter *writer, const QString &name = QString());

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
