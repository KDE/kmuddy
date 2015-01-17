/***************************************************************************
                               cmaplevel.cpp
                             -------------------
    begin                : Tue Mar 20 2001
    copyright            : (C) 2001 by Kmud Developer Team
    email                : kmud-devel@kmud.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cmaplevel.h"

#include "cmapmanager.h"
#include "cmapzone.h"
#include "cmaptext.h"
#include "cmaproom.h"
#include "cmapviewbase.h"

CMapLevel::CMapLevel(CMapManager *mapManager, CMapZone *zone, int pos): m_mapManager(mapManager), m_mapZone(zone)
{
  m_mapManager->m_levelCount++;
  setLevelID(mapManager->m_levelCount);

  // insert the level at the requested location
  QList<CMapLevel *> *lvls = m_mapZone->getLevels();
  if ((pos < 0) || (pos > lvls->size()))
    lvls->append(this);
  else
    lvls->insert(pos, this);
}

CMapLevel::~CMapLevel()
{
  CMapViewBase *view = m_mapManager->getActiveView();
  if (view->getCurrentlyViewedLevel() == this)
  {
    if (getPrevLevel())
    {
      view->showPosition(getPrevLevel(),true);
    }
    else
    {
      if (getNextLevel())
      {
        view->showPosition(getNextLevel(),true);
      }
      else
      {
        view->showPosition(m_mapManager->getMapData()->rootZone->getLevels()->first(),true);
      }
    }
  }
  getZone()->getLevels()->removeAll(this);

  QList<CMapElement *> lst = getAllElements();
  foreach (CMapElement *el, lst)
    delete el;
}

/** Used to find a room with the ID */
CMapRoom *CMapLevel::findRoom(unsigned int id)
{
  foreach (CMapRoom *room, m_roomList)
    if (room->getRoomID() == id)
      return room;
  return 0;
}

/** Used to find a room with the ID */
CMapText *CMapLevel::findText(unsigned int id)
{
  foreach (CMapText *text, m_textList)
    if (text->getTextID() == id)
      return text;
  return 0;
}

/** Used to get the number of the level */
int CMapLevel::getNumber(void)
{
  return getZone()->getLevels()->indexOf(this);
}

unsigned int CMapLevel::getLevelID(void) const
{
	return m_ID;
}

void CMapLevel::setLevelID(unsigned int id)
{
  if (id > m_mapManager->m_levelCount)
    m_mapManager->m_zoneCount = id;

  m_ID = id;
}

QList<CMapElement *> CMapLevel::getAllElements()
{
  QList<CMapElement *> lst;

  foreach (CMapRoom *room, m_roomList)
    lst.push_back(room);
  foreach (CMapText *text, m_textList)
    lst.push_back(text);
  foreach (CMapZone *zone, m_zoneList)
    lst.push_back(zone);

  return lst;
}

/** Used to get the pointer to the previous level */
CMapLevel *CMapLevel::getPrevLevel(void)
{
  QList<CMapLevel *> *lvls = m_mapZone->getLevels();
  int idx = lvls->indexOf(this);
  if (idx <= 0) return 0;
  if (idx > lvls->count() - 1) return 0;
  return lvls->at(idx - 1);
}

/** Used to get the pointer to the next level */
CMapLevel *CMapLevel::getNextLevel(void)
{
  QList<CMapLevel *> *lvls = m_mapZone->getLevels();
  int idx = lvls->indexOf(this);
  if (idx < 0) return 0;
  if (idx >= lvls->count() - 1) return 0;
  return lvls->at(idx + 1);
}

/** Used to get the zone that the level is in */
CMapZone *CMapLevel::getZone(void)
{
  return m_mapZone;
}

QList<CMapElement *> CMapLevel::elementsUnderMouse(QPoint mousePos)
{
  QList<CMapElement *> opts = getAllElements();
  QList<CMapElement *> res;
  foreach (CMapElement *el, opts)
    if (el->mouseInElement(mousePos, m_mapZone))
      res.push_back(el);

  return res;
}

CMapElement *CMapLevel::findElementAt(QPoint pos)
{
  QList<CMapElement *> lst = elementsUnderMouse(pos);
  if (lst.empty()) return NULL;
  return lst.first();
}

CMapRoom *CMapLevel::findRoomAt(QPoint pos)
{
  foreach (CMapRoom *room, m_roomList)
    if (room->mouseInElement(pos, m_mapZone))
      return room;

  return NULL;
}


