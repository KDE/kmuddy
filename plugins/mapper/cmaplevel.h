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

#include <QList>
#include <QPoint>

#include <kmuddy_export.h>

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
	CMapLevel(CMapManager *mapManager, int pos);
	~CMapLevel();

	/** Get the list of rooms */
	QList<CMapRoom *> *getRoomList() { return &m_roomList; }
	/** Get the list of text elements */
	QList<CMapText *> *getTextList() { return &m_textList; }

	/** Used to get the pointer to the previous level */
	CMapLevel *getPrevLevel(void);
	/** Used to get the pointer to the next level */
	CMapLevel *getNextLevel(void);
	/** Used to get the zone that the level is in */
	CMapZone *getZone(void);
	/** Used to get the number of the level */
	int getNumber(void);

        /** Retrieve all elements in the level */
        QList<CMapElement *> getAllElements();
	/** This is used to get a unique ID for the level */
	uint getLevelID(void) const;
	/** This is used to set the level ID for the level */
	void setLevelID(unsigned int id);
	/** Used to find a room with the ID */
	CMapRoom *findRoom(unsigned int id);
	/** Used to find a room with the ID */
	CMapText *findText(unsigned int id);

	QList<CMapElement *> elementsUnderMouse(QPoint mousePos);
	CMapElement *findElementAt(QPoint pos, int type = -1);
	CMapRoom *findRoomAt(QPoint pos);

private:
	unsigned int m_ID;
	CMapManager *m_mapManager;

	/** An array of the elements in the level */
	QList<CMapRoom *> m_roomList;
	QList<CMapText *> m_textList;
};

#endif
