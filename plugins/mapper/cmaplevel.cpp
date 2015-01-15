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
//Added by qt3to4:
#include <Q3PtrList>

CMapLevel::CMapLevel(CMapManager *mapManager)
{	
	m_mapManager = mapManager;
	m_mapManager->m_levelCount++;
	setLevelID(mapManager->m_levelCount);

	m_nextLevel = NULL;
	m_prevLevel = NULL;
	m_elementList[ROOM_LIST].setAutoDelete(false);
	m_elementList[TEXT_LIST].setAutoDelete(false);
	m_elementList[ZONE_LIST].setAutoDelete(false);
	m_currentList = ROOM_LIST;
	m_currentElement = NULL;
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
  getZone()->getLevels()->remove(this);

  QList<CMapElement *> lst;

  for (CMapRoom *room = (CMapRoom *)m_elementList[ROOM_LIST].first(); room != NULL ; room = (CMapRoom *)m_elementList[ROOM_LIST].next())
    lst.push_back(room);
  for (CMapText *text = (CMapText *)m_elementList[TEXT_LIST].first(); text != NULL ; text = (CMapText *)m_elementList[TEXT_LIST].next())
    lst.push_back(text);
  for (CMapZone *zone = (CMapZone *)m_elementList[ZONE_LIST].first(); zone != NULL ; zone = (CMapZone *)m_elementList[ZONE_LIST].next())
    lst.push_back(zone);

  foreach (CMapElement *el, lst)
    delete el;
}

/** Used to find a room with the ID */
CMapRoom *CMapLevel::findRoom(unsigned int id)
{
	CMapRoom *result = NULL;

	for (CMapRoom *room = (CMapRoom *)m_elementList[ROOM_LIST].first(); room != NULL ; room = (CMapRoom *)m_elementList[ROOM_LIST].next())
	{
		if (room->getRoomID()==id)
		{
			result = room;
			break;
		}
	}

	return result;
}

/** Used to find a room with the ID */
CMapText *CMapLevel::findText(unsigned int id)
{
	CMapText *result = NULL;

	for (CMapText *text = (CMapText *)m_elementList[TEXT_LIST].first(); text != NULL ; text = (CMapText *)m_elementList[TEXT_LIST].next())
	{
		if (text->getTextID()==id)
		{
			result = text;
			break;
		}
	}

	return result;
}


/** Used to get the first element in a level */
CMapElement *CMapLevel::getFirstElement(void)
{
	for (int i = 0; i<NUM_LISTS; i++)
	{
		if (m_elementList[i].first())
		{
			m_currentElement = m_elementList[i].first();
			m_currentList = i;
			return m_elementList[i].first();
		}
	}

    m_currentList = ROOM_LIST;
	m_currentElement = NULL;

	return NULL;
}

/** Used to get the element after the current element in the level */
CMapElement *CMapLevel::getNextElement(void)
{
	CMapElement *tmpElm = m_currentElement;

	// Process the lists that are not paths
	if (m_currentList<NUM_LISTS)
	{
		tmpElm = m_elementList[m_currentList].next();

		// Get the next element by going through all the lists of different elements
		for (int i = m_currentList; i<NUM_LISTS; i++)
		{
			if (tmpElm)
			{
				m_currentList = i;
				m_currentElement = tmpElm;
				return tmpElm;
			}
			else
			{
				// No element found in current list so go to next list or break if
				// that was the last list
				if (i!=NUM_LISTS-1)
				{
					tmpElm = m_elementList[i+1].first();
					continue;
				}
				else
				{
					tmpElm = NULL;
					break;
				}
			}

			tmpElm = m_elementList[i].next();
		}
	}

	// Process the paths
    Q3PtrList<CMapRoom> *roomList = getRoomList();

	for (CMapRoom *room = roomList->first();room!=0;room = roomList->next())
	{
		for (CMapPath *path = room->getPathList()->first();path!=0; path = room->getPathList()->next())
		{
			if (tmpElm)
			{
				if (((CMapElement*)path) == m_currentElement)
					tmpElm = NULL;
			}
			else
			{
				m_currentElement = (CMapElement *)path;
				m_currentList = NUM_LISTS;
				return m_currentElement;
			}

		}
	}

	m_currentElement = NULL;
	m_currentList = ROOM_LIST;
	return NULL;
}

/** Used to get the current element in the level */
CMapElement *CMapLevel::getCurrentElement(void)
{
	return m_currentElement;
}

/** Used to get the number of the level */
int CMapLevel::getNumber(void)
{
	return  getZone()->getLevels()->find(this);
}

unsigned int CMapLevel::getLevelID(void)
{
	return m_ID;
}

void CMapLevel::setLevelID(unsigned int id)
{
	if (id > m_mapManager->m_levelCount)
	  m_mapManager->m_zoneCount = id;

	m_ID = id;
}

void CMapLevel::resetCount(void)
{
	m_mapManager->m_levelCount = 0;
}

/** Used to set the pointer to the previous level */
void CMapLevel::setPrevLevel(CMapLevel *level)
{
	m_prevLevel = level;
}

/** Used to get the pointer to the previous level */
CMapLevel *CMapLevel::getPrevLevel(void)
{
	return m_prevLevel;
}

/** Used to set the pointer to the next level */
void CMapLevel::setNextLevel(CMapLevel *level)
{
	m_nextLevel = level;
}

/** Used to get the pointer to the next level */
CMapLevel *CMapLevel::getNextLevel(void)
{
	return m_nextLevel;
}

/** Used to get the zone that the level is in */
CMapZone *CMapLevel::getZone(void)
{
	return m_mapZone;
}

/** Used to set the zone that the level is in */
void CMapLevel::setZone(CMapZone *zone)
{	
	m_mapZone = zone;
}
