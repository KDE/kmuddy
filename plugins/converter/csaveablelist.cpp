/***************************************************************************
                        csaveablelist.cpp  -  ancestor of alias/trigger handler
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

#include "csaveablelist.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cSaveableList::cSaveableList (QString file, QString _objName, cSaveableField *proto)
{
  _count = 0;
  first = NULL;
  last = NULL;
  cur = NULL;
  marker = NULL;
  objName = _objName;
  prototype = proto;

  config = new KConfig (file);
  load ();
}

cSaveableList::~cSaveableList ()
{
  clear ();
  if (config != NULL)
    delete config;
}

bool cSaveableList::addToBegin (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (first == NULL)  //list is still empty
  {
    first = newitem;
    last = newitem;
    newitem->prev = NULL;
    newitem->next = NULL;
    cur = NULL;
    _count = 1;
  }
  else
  {
    first->prev = newitem;
    newitem->next = first;
    newitem->prev = NULL;
    first = newitem;
    _count++;
  }
  return true;
}

bool cSaveableList::addToEnd (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (last == NULL)  //list is still empty
  {
    first = newitem;
    last = newitem;
    newitem->prev = NULL;
    newitem->next = NULL;
    cur = NULL;
    _count = 1;
  }
  else
  {
    last->next = newitem;
    newitem->next = NULL;
    newitem->prev = last;
    last = newitem;
    _count++;
  }
  return true;
}

bool cSaveableList::addAfterCurrent (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (cur == NULL)  //there is no current entry!!!
    return false;
  else
  {
    newitem->next = cur->next;
    newitem->prev = cur;
    if (cur->next != NULL)
      cur->next->prev = newitem;
    cur->next = newitem;
    _count++;
  }
  return true;
}

bool cSaveableList::addAfterMarker (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (marker == NULL)  //there is no marked entry!!!
    return false;
  else
  {
    newitem->next = marker->next;
    newitem->prev = marker;
    if (marker->next != NULL)
      marker->next->prev = newitem;
    marker->next = newitem;
    _count++;
  }
  return true;
}

bool cSaveableList::replaceCurrent (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (cur == NULL)  //there is no current entry!!!
    return false;
  else
  {
    //bind new item
    newitem->next = cur->next;
    newitem->prev = cur->prev;
    if (cur->next != NULL)
      cur->next->prev = newitem;
    if (cur->prev != NULL)
      cur->prev->next = newitem;
    if (first == cur)
      first = newitem;
    if (last == cur)
      last = newitem;
    if (marker == cur)
      marker = newitem;   //marked item gets updated as well...7
    //get rid of the old one
    delete cur;
    cur = newitem;
    //_count is NOT modified
  }
  return true;
}

bool cSaveableList::replaceMarker (cSaveableField *newitem)
{
  if (newitem == NULL)
    return false;
  if (marker == NULL)  //there is no marked entry!!!
    return false;
  else
  {
    //bind new item
    newitem->next = marker->next;
    newitem->prev = marker->prev;
    if (marker->next != NULL)
      marker->next->prev = newitem;
    if (marker->prev != NULL)
      marker->prev->next = newitem;
    if (first == marker)
      first = newitem;
    if (last == marker)
      last = newitem;
    if (cur == marker)
      cur = newitem;   //current item gets updated as well...
    //get rid of the old one
    delete marker;
    marker = newitem;
    //_count is NOT modified
  }
  return true;
}

bool cSaveableList::moveCurrentToFront ()
//only moves by one item
{
  if (cur == NULL)
    return false;
  if (cur->prev == NULL)
    return true;    //this is the begin - there's nothing to do

  //store pointers
  cSaveableField *prev = cur->prev;
  cSaveableField *prev2 = cur->prev->prev;
  cSaveableField *next = cur->next;

  //bind cur
  cur->next = prev;
  cur->prev = prev2;
  //bind next
  if (next != NULL)
    next->prev = prev;
  //bind prev
  prev->next = next;
  prev->prev = cur;
  //bind prev2
  if (prev2 != NULL)
    prev2->next = cur;

  //update first/last if needed
  if (first == prev)
    first = cur;
  if (last == cur)
    last = prev;
    
  return true;
}

bool cSaveableList::moveCurrentToBack ()
//only moves by one item
{
  if (cur == NULL)
    return false;
  if (cur->next == NULL)
    return true;    //this is the end - there's nothing to do
  
  //store pointers
  cSaveableField *prev = cur->prev;
  cSaveableField *next = cur->next;
  cSaveableField *next2 = cur->next->next;

  //bind cur          
  cur->next = next2;  
  cur->prev = next;
  //bind prev
  if (prev != NULL)
    prev->next = next;
  //bind next
  next->prev = prev;
  next->next = cur;
  //bind next2
  if (next2 != NULL)
    next2->prev = cur;

  //update first/last if needed
  if (first == cur)
    first = next;
  if (last == next)
    last = cur;

  return true;
}

bool cSaveableList::removeFirst (bool dontDelete)
{
  if (first == NULL)    //there's nothing to remove!
    return false;
  if (first->next != NULL)
    first->next->prev = NULL;
  cSaveableField *newfirst = first->next;
  if (cur == first)
    cur = newfirst;
  if (last == first)
    last = NULL;     //newfirst==NULL in this case, but I want to be sure...
  if (marker == first)
    marker = newfirst;
  if (!dontDelete)
    delete first;
  first = newfirst;
  _count--;
  return true;
}

bool cSaveableList::removeLast (bool dontDelete)
{
  if (last == NULL)    //there's nothing to remove!
    return false;
  if (last->prev != NULL)
    last->prev->next = NULL;
  cSaveableField *newlast = last->prev;
  if (cur == last)
    cur = newlast;
  if (first == last)
    first = NULL;     //newlast==NULL in this case, but I want to be sure...
  if (marker == last)
    marker = newlast;
  if (!dontDelete)
    delete last;
  last = newlast;
  _count--;
  return true;
}

bool cSaveableList::removeCurrent (bool dontDelete)
{
  if (cur == NULL)    //there's nothing to remove!
    return false;

  if (cur->prev != NULL)
    cur->prev->next = cur->next;
  if (cur->next != NULL)
    cur->next->prev = cur->prev;
  if (marker == cur)
    marker = NULL;
  if (first == cur)
    first = cur->next;
  if (last == cur)
    last = cur->prev;
  if (!dontDelete)
    delete cur;
  cur = NULL;
  _count--;
  return true;
}

bool cSaveableList::removeMarked (bool dontDelete)
{
  if (marker == NULL)    //there's nothing to remove!
    return false;

  if (marker->prev != NULL)
    marker->prev->next = marker->next;
  if (marker->next != NULL)
    marker->next->prev = marker->prev;
  if (cur == marker)
    cur = NULL;
  if (first == marker)
    first = marker->next;
  if (last == marker)
    last = marker->prev;
  if (!dontDelete)
    delete marker;
  marker = NULL;
  _count--;
  return true;
}

void cSaveableList::clear (bool dontDelete)
{
  while (last != NULL)
    removeFirst (dontDelete);
  _count = 0;
}

void cSaveableList::load ()
{
  if (config == NULL)
    return;
  clear ();
  KConfigGroup g = config->group ("General");
  int num = g.readEntry ("Count", 0);
  for (int i = 1; i <= num; i++)
  {
    QString groupname = objName + " " + QString::number (i);
    cSaveableField *field = prototype->newInstance ();
    field->load (config, groupname);
    addToEnd (field);
  }
}


