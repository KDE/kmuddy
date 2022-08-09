/***************************************************************************
                          csaveablelist.h  -  ancestor of alias/trigger handler
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne sep 8 2002
    copyright            : (C) 2002 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CSAVEABLELIST_H
#define CSAVEABLELIST_H

#include <QString>

class KConfig;

#include "csaveablefield.h"

/**
Abstract class that can load/save itself.
Its descendands will handle aliases/triggers.
  *@author Tomas Mecir
  */

class cSaveableList {
public: 
  /** constructor */
  cSaveableList (QString name, QString _objName, cSaveableField *proto);
  /** destructor */
  ~cSaveableList ();

  /** go back to the first item */
  void reset () { cur = first; };
  /** advance list one item forwards */
  void operator++ (int) { cur = ((cur == nullptr) ? nullptr : cur->next); };
  /** advance list one item backwards */
  void operator-- (int) { cur = ((cur == nullptr) ? nullptr : cur->prev); };
  /** return current field */
  cSaveableField * operator* () { return cur; };
  /** Is the current item valid? False if we have run away from the list */
  operator bool () { return (cur != nullptr); };

  /** is the list empty? */
  bool isEmpty () { return (_count == 0); };
  /** are we on the first item? */
  bool isFirst () { return (cur == first); };
  /** are we on the last item? */
  bool isLast () { return (cur == last); };
  /** returns count of items in the list */
  int count () { return _count; };

  /** sets marker to the current item */
  void setMarker () { marker = cur; };
  /** removes marker */
  void unsetMarker () { marker = nullptr; };

  /** Adds a new item to the begin of the list. Returns true if successful. */
  bool addToBegin (cSaveableField *newitem);
  /** Adds a new item to the end of the list. Returns true if successful. */
  bool addToEnd (cSaveableField *newitem);
  /** Adds a new item after the current item. Returns true if successful. */
  bool addAfterCurrent (cSaveableField *newitem);
  /** Adds a new item after the marked item. Returns true if successful. */
  bool addAfterMarker (cSaveableField *newitem);
  /** Replaces current item with a new one. Returns true if successful. */
  bool replaceCurrent (cSaveableField *newitem);
  /** Replaces marked item with a new one. Returns true if successful. */
  bool replaceMarker (cSaveableField *newitem);
  /** moves current item by one item towards the begin of list */
  bool moveCurrentToFront ();
  /** moves current item by one item towards the end of list */
  bool moveCurrentToBack ();
  /** Removes first item from the list. Returns true if successful. */
  bool removeFirst (bool dontDelete = false);
  /** Removes last item from the list. Returns true if successful. */
  bool removeLast (bool dontDelete = false);
  /** Removes current item from the list. Returns true if successful. */
  bool removeCurrent (bool dontDelete = false);
  /** Removes marked item from the list. Returns true if successful. */
  bool removeMarked (bool dontDelete = false);
  /** clears the whole list */
  void clear (bool dontDelete = false);
  
protected:
  void load ();

  QString objName;
  cSaveableField *prototype;

  /** config object must be created before calling load/save */
  KConfig *config;
  
  /** required pointers to items */
  cSaveableField *first, *last, *cur, *marker;
  /** count of items is stored here */
  int _count;

};

#endif
