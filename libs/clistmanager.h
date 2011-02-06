//
// C++ Interface: clistmanager
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CLISTMANAGER_H
#define CLISTMANAGER_H

#include <kmuddy_export.h>

#include <cactionbase.h>

class cList;
class cListObject;
class QStandardItemModel;

/** cListManager - manages all the existing lists */
class KMUDDY_EXPORT cListManager: public cActionBase {
 public:
  static cListManager *self();
  virtual ~cListManager ();

  /** Register a new list type. Factory should point to the newList method of a cList-inherited class. */
  void registerType (const QString &name, const QString &visibleName, cList *(*factory)());
  /** Unregister a list type. Also remove all lists of this type. */
  void unregisterType (const QString &name);

  /** A model containing all the registered types. */
  QStandardItemModel *typeModel () const;

  /** Returns the given list, creating it if needed. */
  cList *getList (int sessId, const QString &name);
  void addSession (int sessId);
  /** Remove all lists belonging to the given session. */
  void removeSession (int sessId);

  int registerObject (cListObject *obj);
  void unregisterObject (cListObject *obj);
  int objectId (cListObject *obj) const;
  cListObject *object (int id) const;

  void saveList (int sessId, cList *list);
  void saveAll (int sessId);
 protected:
  cListManager ();
  static cListManager *_self;

  void loadList (int sessId, cList *list);
  virtual void eventNothingHandler (QString event, int session);
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &);

  struct Private;
  Private *d;
};

#endif  // CLISTMANAGER_H
