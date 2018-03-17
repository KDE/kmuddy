//
// C++ Interface: clistgroup
//
// Description: List group.
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

#ifndef CLISTGROUP_H
#define CLISTGROUP_H

#include <clistobject.h>
#include <kmuddy_export.h>
#include <list>

/**
A group in the list, base class for all types of groups (most probably won't need to subclass).
The constructor is protected - only cList will want to create these objects.

	@author Tomas Mecir <kmuddy@kmuddy.com>
*/
class KMUDDY_EXPORT cListGroup : public cListObject
{
public:
  void addObject (cListObject *obj);
  void removeObject (cListObject *obj);
  void objectChanged (cListObject *obj);

  virtual bool isGroup () override { return true; }

  /** Returns the list of objects in this group. */
  const std::list<cListObject *> *objectList ();

  /** Returns the list of objects in this group, sorted by priority. */
  const std::list<cListObject *> *priorityList ();

  /** Assign a tag to this group. */
  void setTag (const QString &tag);
  /** Return tag assigned to this group. */
  QString tag ();

  cListObject *objectAt (int pos);
  int objectPosition (cListObject *obj);

  /** Move object upwards. */
  void moveObjectUp (int pos);
  /** Move object downwards. */
  void moveObjectDown (int pos);
  /** Move object to a specified position. */
  void moveObjectToPosition (cListObject *obj, int pos);
  /** Number of objects within this group. */
  int objectCount () const;
protected:
  /** constructor */
  cListGroup (cList *list);
  /** destructor */
  ~cListGroup ();
  friend class cList;

  virtual void updateVisibleName () override;

  /** Load the group from the XML reader. */
  virtual void load (QXmlStreamReader *reader) override;
  /** Save the group into a XML writer, including all nested groups. */
  virtual void save (QXmlStreamWriter *writer) override;

  /** regenerate the list of objects stored by priority */
  void generatePriorityList ();
  
  /** Recursive traversal of the list, called by cList::traverse */
  virtual cList::TraverseAction traverse (int traversalType) override;
  
  /** helper function used by generatePriorityList */
  static bool compareObjects (cListObject *a, cListObject *b);

  struct Private;
  Private *d;
};

#endif
