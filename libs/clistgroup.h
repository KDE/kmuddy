//
// C++ Interface: clistgroup
//
// Description: List group.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

  virtual bool isGroup () { return true; }

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

  virtual void updateVisibleName ();

  /** Load the group from the XML reader. */
  virtual void load (QXmlStreamReader *reader);
  /** Save the group into a XML writer, including all nested groups. */
  virtual void save (QXmlStreamWriter *writer);

  /** regenerate the list of objects stored by priority */
  void generatePriorityList ();
  
  /** Recursive traversal of the list, called by cList::traverse */
  virtual cList::TraverseAction traverse (int traversalType);
  
  /** helper function used by generatePriorityList */
  static bool compareObjects (cListObject *a, cListObject *b);

  struct Private;
  Private *d;
};

#endif
