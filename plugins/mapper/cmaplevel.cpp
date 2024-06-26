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
#include "cmapview.h"

#include <KLocalizedString>

CMapLevel::CMapLevel(CMapManager *mapManager, int pos): m_mapManager(mapManager)
{
  m_mapManager->m_levelCount++;
  setLevelID(m_mapManager->m_levelCount);
  name = i18n("Level %1").arg(pos+1);

  // insert the level at the requested location
  getZone()->insertLevel(this, pos);
}

CMapLevel::~CMapLevel()
{
  CMapView *view = m_mapManager->getActiveView();
  if (view->getCurrentlyViewedLevel() == this)
  {
    CMapLevel *show = getPrevLevel();
    if (!show) show = getNextLevel();
    if (!show) show = getZone()->firstLevel();
    view->showPosition(show,true);
  }
  getZone()->removeLevel(this);

  QList<CMapElement *> lst = getAllElements();
  for (CMapElement *el : lst)
    delete el;
}

/** Used to find a room with the ID */
CMapRoom *CMapLevel::findRoom(unsigned int id)
{
  for (CMapRoom *room : m_roomList)
    if (room->getRoomID() == id)
      return room;
  return nullptr;
}

/** Used to find a room with the ID */
CMapText *CMapLevel::findText(unsigned int id)
{
  for (CMapText *text : m_textList)
    if (text->getTextID() == id)
      return text;
  return nullptr;
}

/** Used to get the number of the level */
int CMapLevel::getNumber(void) const
{
  return getZone()->levelIndex(this);
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

QString CMapLevel::getName() const
{
  if (name.length()) return name;
  return i18n("Level %1").arg(getLevelID());
}

void CMapLevel::setName(const QString &name)
{
  this->name = name;
  getZone()->setLevelName(this, getName());
}

QList<CMapElement *> CMapLevel::getAllElements()
{
  QList<CMapElement *> lst;

  for (CMapRoom *room : m_roomList)
    lst.push_back(room);
  for (CMapText *text : m_textList)
    lst.push_back(text);

  return lst;
}

/** Used to get the pointer to the previous level */
CMapLevel *CMapLevel::getPrevLevel(void)
{
  int idx = getZone()->levelIndex(this);
  if (idx <= 0) return nullptr;
  return getZone()->getLevel(idx - 1);
}

/** Used to get the pointer to the next level */
CMapLevel *CMapLevel::getNextLevel(void)
{
  int idx = getZone()->levelIndex(this);
  if (idx < 0) return nullptr;
  return getZone()->getLevel(idx + 1);
}

/** Used to get the zone that the level is in */
CMapZone *CMapLevel::getZone(void) const
{
  return m_mapManager->getZone();
}

QList<CMapElement *> CMapLevel::elementsUnderMouse(QPoint mousePos)
{
  QList<CMapElement *> opts = getAllElements();
  QList<CMapElement *> res;
  for (CMapElement *el : opts)
    if (el->mouseInElement(mousePos))
      res.push_back(el);

  return res;
}

CMapElement *CMapLevel::findElementAt(QPoint pos, int type)
{
  QList<CMapElement *> lst = elementsUnderMouse(pos);
  if (lst.empty()) return nullptr;
  for (CMapElement *el : lst)
  {
    if ((type >= 0) && (el->getElementType() != type)) continue;
    return el;
  }
  return nullptr;
}

CMapRoom *CMapLevel::findRoomAt(QPoint pos)
{
  for (CMapRoom *room : m_roomList)
    if (room->mouseInElement(pos))
      return room;

  return nullptr;
}


