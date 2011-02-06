//
// C++ Interface: clist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CLIST_H
#define CLIST_H

#include <qstring.h>
#include <map>

#include <kmuddy_export.h>

class cListEditor;
class cListGroup;
class cListObject;

class QModelIndex;
class QAbstractItemModel;
class QWidget;
class QXmlStreamReader;
class QXmlStreamWriter;

enum cListPropertyType {
  Int = 0,
  String,
  Bool
};

/** cListProperty - definition of one object property. */
struct KMUDDY_EXPORT cListProperty {
  QString name;
  QString desc;      // description of the property
  cListPropertyType type;
  int defIntValue;  // default int value, only used if type is Int
  QString defStrValue;  // default string value, only used if type is String
  bool defBoolValue;  // default bool value, only used if type is Bool
};


/**
One list of objects. Each existing list should subclass this to provide the desired functionality.

	@author Tomas Mecir <kmuddy@kmuddy.com>
*/
class KMUDDY_EXPORT cList {
public:
  /** constructor */
  cList (const QString &name);
  /** destructor */
  virtual ~cList ();

  /** Create a new instance of this class. */
  static cList *newList () { return 0; };

  /** Set session ID of this list. */
  void setSession (int sess);
  /** Return session ID of this list. */
  int session ();

  /** Create a new object belonging to this list. */
  virtual cListObject *newObject () = 0;
  /** Create a new group belonging to this list. */
  virtual cListGroup *newGroup ();
  void deleteObject (cListObject *obj);

  /** Returns list name. */
  QString name ();
  /** Returns the name of objects of this type. */
  virtual QString objName () = 0;

  /** Returns a list of all object properties indexed by name. */
  const std::map<QString, cListProperty> &getPropertyList ();
  int defaultIntValue (const QString &name);
  QString defaultStrValue (const QString &name);
  bool defaultBoolValue (const QString &name);

  /** Is this list enabled ? */
  bool enabled ();
  /** Enabled or disables this list. */
  virtual void setEnabled (bool en = true);

  /** main group of the list */
  cListGroup *rootGroup ();
  /** returns a group with the given name */
  cListGroup *group (const QString &name);
  /** creates a new group with the given name, or returns an existing one, if a group with such a name exists */
  cListGroup *addGroup (cListGroup *parent, const QString &name);
  /** Rename a group. Returns true if successful, false if the group with the new name already exists. */
  bool renameGroup (cListGroup *group, const QString &newName);
  /** Remove a group */
  void removeGroup (cListGroup *group);
  /** Adds an item to the group. Removes it from its existing group, if any. This can also be used to create nested groups - the item
  can be a group. */
  void addToGroup (cListGroup *group, cListObject *item);

  /** Changes the object name, */
  bool setObjectName (cListObject *obj, const QString &name);
  /** Returns the object of a given name. */
  cListObject *getObject (const QString &name);

  /** Remove everything from the list. */
  void clear ();

  /** Create and return a new object editor widget with the given parent. */
  virtual cListEditor *editor (QWidget *parent) = 0;

  enum TraverseAction {
    Continue=0,  // continue traversing
    Stop,        // stop traversing
    LeaveGroup   // leave the current group and continue with the next one
  };

  /** Traverse the list and call the traverse method on each object. Descends into groups and honors the returnes TraverseAction */
  void traverse (int traversalType);

  /** Load the structure from the XML reader. */
  void load (QXmlStreamReader *reader);
  /** Save the list into a XML writer, including all nested groups. */
  void save (QXmlStreamWriter *writer);
  /** Did error occur durig load/save? */
  bool hasError ();
  /** Return the last error that occured. */
  const QString lastError ();
  /** Clear the last error. */
  void clearError ();

  /** interaction with the associated model */
  QAbstractItemModel *model ();
  cListObject *objectAt (const QModelIndex &index);
  QModelIndex indexOf (const cListObject *obj);
protected:
  void initRootGroup ();

  /** Adds a new property. The constructors of derived lists should use this. */
  void addProperty (const cListProperty &prop);
  /** Convenience wrapper around addProperty for int properties. */
  void addIntProperty (const QString &name, const QString &desc, int defaultValue = 0);
  /** Convenience wrapper around addProperty for string properties. */
  void addStringProperty (const QString &name, const QString &desc, QString defaultValue = QString());
  /** Convenience wrapper around addProperty for boolean properties. */
  void addBoolProperty (const QString &name, const QString &desc, bool defaultValue = false);

  /** Adds a new object to the list. Used by cListObject constructor. */
  void addObject (cListObject *obj);
  /** Removes an object from the list. Used by cListObject destructor. */
  void removeObject (cListObject *obj);

  /** Called when a group is adding a new item at the given position. */
  void notifyAdding (cListGroup *group, int pos);
  /** Called when the adding is done. */
  void addDone ();
  /** Called when an item is being removed from its group. */
  void notifyRemoving (cListObject *obj);
  /** Called when the removal is done. */
  void removeDone ();
  /** Called when the object  has changed. */
  void notifyChanged (cListObject *obj);

  /** Called when the list is fully loaded. Useful to perform initialisation that
   requires the objects. */
  virtual void listLoaded () {};
  /** Called when the list is saved. */
  virtual void listSaved () {};
  struct Private;
  Private *d;

  friend class cListObject;
  friend class cListGroup;
  friend class cListManager;
};

#endif
