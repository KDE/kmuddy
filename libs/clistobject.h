//
// C++ Interface: clistobject
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

#ifndef CLISTOBJECT_H
#define CLISTOBJECT_H

#include <qstring.h>
#include <QModelIndex>
#include <QObject>
#include <list>
#include <map>
#include <kmuddy_export.h>

// needed for cList::TraverseAction
#include <clist.h>

class cListGroup;

class QXmlStreamReader;
class QXmlStreamWriter;

#define DEFAULT_OBJECT_PRIORITY 50

/** cListObjectData - data of the object. */
struct KMUDDY_EXPORT cListObjectData {
  QString name;
  int priority;
  bool enabled;
  std::map<QString, int> intValues;
  std::map<QString, QString> strValues;
  std::map<QString, bool> boolValues;
  int intValue (const QString &name) const {
    if (intValues.count (name))
      return intValues.find (name)->second;
    return 0;
  }
  QString strValue (const QString &name) const {
    if (strValues.count (name))
      return strValues.find (name)->second;
    return QString();
  }
  bool boolValue (const QString &name) const {
    if (boolValues.count (name))
      return boolValues.find (name)->second;
    return false;
  }
};

/**
cListObject - Base class for all objects. Holds all data. Can load/save itself.

Child classes (other than cListGroup) only need to add their own things,
the only thing that they may want to override is attribChanged().

@author Tomas Mecir <kmuddy@kmuddy.com>
*/

class KMUDDY_EXPORT cListObject : public QObject {
Q_OBJECT
public:
  /** Returns parent group. */
  cListGroup *parentGroup ();

  /** Rreturns list where this object belongs to. */
  cList *list ();

  /** Returns object priority. */
  int priority ();
  /** Sets object priority. */
  void setPriority (int p);

  /** Returns name visible in the object list. */
  QString visibleName ();
  /** Sets the visible name. */
  void setVisibleName (const QString &name);

  /** Returns the object name. */
  QString name ();

  /** Returns the value of an integer property. If it's not specified
  and the object specifies a default value, then that gets returned.
  If no default value is provided, 0 is returned. */
  int intVal (const QString &name);
  /** Sets an integer property value. */
  void setInt (const QString &name, int value);
  /** Clears an integer property value back to default. */
  void clearInt (const QString &name);

  /** Returns the value of a string property. If it's not specified
  and the object specifies a default value, then that gets returned.
  If no default value is provided, empty string is returned. */
  QString strVal (const QString &name);
  /** Sets a string property value. */
  void setStr (const QString &name, const QString &value);
  /** Clears a string property value back to default. */
  void clearStr (const QString &name);

  /** Returns the value of  boolean property. If it's not specified
  and the object specifies a default value, then that gets returned.
  If no default value is provided, false is returned. */
  bool boolVal (const QString &name);
  /** Sets a boolean property value. */
  void setBool (const QString &name, bool value);
  /** Clears  boolean property value back to default. */
  void clearBool (const QString &name);

  // list of strings - convenience wrappers around the string routines
  int strListCount (const QString &name);
  void setStrListCount (const QString &name, int count);
  QString strListValue (const QString &name, int which);
  void setStrListValue (const QString &name, int which, const QString &value);
  void clearStrList (const QString &name);

  /** Returns a -copy- of the object data. */
  cListObjectData data ();

  /** Is this object enabled ? */
  bool enabled ();
  /** Enabled or disables an object. */
  virtual void setEnabled (bool en = true);

  virtual bool isGroup () { return false; }

  int positionInGroup () const;
  int priorityInGroup () const;

  /** Returns index of this object within the model. */
  QModelIndex itemIndex ();
Q_SIGNALS:
  /** Emitted whenever this object is changed. */
  void changed (cListObject *);
protected:
  /** constructor - only cList can call it */
  cListObject (cList *list);
  /** destructor - only cList can call it */
  ~cListObject () override;

  /** Sets the object name. Functions and macros can access the object by it. Used by cList. */
  virtual void setName (const QString &n);
  /** Generate the visible name. */
  virtual void updateVisibleName ();

  /** Change parent group. Used by cList. */
  void setParentGroup (cListGroup *group);
  /** Change position in group. Used by cListGroup. */
  virtual void setPositionInGroup (int pos);
  /** Change priority position in group. Used by cListGroup. */
  virtual void setPriorityInGroup (int pos);
  /** Load the object from the XML reader. */
  virtual void load (QXmlStreamReader *reader);
  /** Save the object into a XML writer. */
  virtual void save (QXmlStreamWriter *writer);

  /** React on attribute change. Does nothing, child classes can override. */
  virtual void attribChanged (const QString &name);
  /** React on the fact that the object has moved. */
  virtual void objectMoved ();
  /** React on the object being enabled. */
  virtual void objectEnabled () {};
  /** React on the object being disabled. */
  virtual void objectDisabled () {};

  /** Recursive traversal of the list, called by cList::traverse.
  Redefine with actual implementation. The parameter can be used
  to distinguish if traversal is used for multiple purposes within
  the same object. */
  virtual cList::TraverseAction traverse (int traversalType);

  friend class cList;
  friend class cListGroup;  // needed to allow cListGroup to call setPositionInGroup and load/save
  struct Private;
  Private *d;
};
// Qt metatype, needed so the model can return these in data()
Q_DECLARE_METATYPE(cListObject *)

#endif
