//
// C++ Interface: clistmanager
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
  ~cListManager () override;

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
  void eventNothingHandler (QString event, int session) override;
  void eventStringHandler (QString event, int session, QString &par1, const QString &) override;

  struct Private;
  Private *d;
};

#endif  // CLISTMANAGER_H
