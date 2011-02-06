/***************************************************************************
                               cmaplevel.h
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


#ifndef CMAPLEVEL_H
#define CMAPLEVEL_H

#include <q3ptrlist.h>

#include <kmuddy_export.h>

#define ROOM_LIST 0
#define TEXT_LIST 1
#define ZONE_LIST 2

#define NUM_LISTS 3

class CMapManager;
class CMapRoom;
class CMapText;
class CMapZone;
class CMapElement;

/**The map level class used to store the map elements
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapLevel
{
public:
	CMapLevel(CMapManager *mapManager);
	~CMapLevel();

	/** Get the list of rooms */
	Q3PtrList<CMapRoom> *getRoomList(void)      { return (Q3PtrList<CMapRoom> *)&m_elementList[ROOM_LIST]; }
	/** Get the list of text elements */
	Q3PtrList<CMapText> *getTextList(void)      { return (Q3PtrList<CMapText> *)&m_elementList[TEXT_LIST]; }
	/** Get the list of zones */
	Q3PtrList<CMapZone> *getZoneList(void)      { return (Q3PtrList<CMapZone> *)&m_elementList[ZONE_LIST]; }

	/** Used to set the pointer to the previous level */
	void setPrevLevel(CMapLevel *level);
	/** Used to get the pointer to the previous level */
	CMapLevel *getPrevLevel(void);
	/** Used to set the pointer to the next level */
	void setNextLevel(CMapLevel *level);
	/** Used to get the pointer to the next level */
	CMapLevel *getNextLevel(void);
	/** Used to get the zone that the level is in */
	CMapZone *getZone(void);
	/** Used to set the zone that the level is in */
	void setZone(CMapZone *zone);
	/** Used to get the number of the level */
	int getNumber(void);


	/** Used to get the first element in a level */
	CMapElement *getFirstElement(void);
	/** Used to get the element after the current element in the level */
	CMapElement *getNextElement(void);
	/** Used to get the current element in the level */
	CMapElement *getCurrentElement(void);
	/** This is used to get a unique ID for the level */
	uint getLevelID(void);
	/** This is used to set the level ID for the level */
	void setLevelID(unsigned int id);
	/** Used to find a room with the ID */
	CMapRoom *findRoom(unsigned int id);
	/** Used to find a room with the ID */
	CMapText *findText(unsigned int id);

	void resetCount(void);

private:
	unsigned int m_ID;
	CMapManager *m_mapManager;
	CMapZone *m_mapZone;
	CMapLevel *m_nextLevel;
	CMapLevel *m_prevLevel;

	/** An array of the elements in the level */
	Q3PtrList<CMapElement> m_elementList[NUM_LISTS];

	/** A pointer to the current element */
	CMapElement *m_currentElement;

	int m_currentList;
};

#endif
