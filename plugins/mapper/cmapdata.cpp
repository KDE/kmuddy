/***************************************************************************
                               cmapdata.cpp
                             -------------------
    begin                : Sat Mar 10 2001
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

#include "cmapdata.h"

#include <kapplication.h>

#include "cmaplevel.h"

CMapData::CMapData()
{
	//zoneList.setAutoDelete(true);

	gridSize.setWidth(20);
	gridSize.setHeight(20);

	// FIXME_jp: This needs loading/saving
	defaultPathTwoWay = true;
	showUpperLevel = true;
	showLowerLevel = true;
	createModeActive = true;
	gridVisable = true;

	speedwalkAbortActive = false;
	speedwalkAbortLimit = 100;
	speedwalkDelay = 5;

	validRoomCheck = false;

	defaultTextColor = Qt::black;
	defaultTextFont = kapp->font();
	defaultTextFont.setPointSize(8);

	failedMoveMsg.clear();

	rootZone = NULL;
	
	//initDirections();
}

CMapData::~CMapData()
{
}

/** This is used to get the first zone in a list of zones */
CMapZone *CMapData::getFirstZone(void)
{
	currentZone = rootZone;

	return currentZone;
}

/** This is used to get the next zone is a list of zones */
CMapZone *CMapData::getNextZone(void)
{
	// Find next zone in current zone
	CMapZone *zone = findFirstSubZone(currentZone);
	if (zone)
	{
		currentZone = zone;
		return zone;
	}

	// Find next zone in at the same level
    // if no zones in the current zone
	m_foundCurrentZone = false;
	currentZone = getNextSameLevelZone(currentZone);

	return currentZone;
}


/** This is used to get the current zone in a list of zones */
CMapZone *CMapData::getCurrentZone(void)
{
	return currentZone;
}

signed int CMapData::getZoneNumber(CMapZone *findZone)
{
	int result = -1;
	int count = 0;

	for (CMapZone *zone = getFirstZone();zone!=0;zone = getNextZone())
	{
		if (zone  == findZone)
		{
			result = count;
			break;
		}

		count++;
	}

	return result;
}

void CMapData::initDirections(void)
{
	directions[NORTH]     = "north";
	directions[SOUTH]     = "south";
	directions[WEST]      = "west";
	directions[EAST]      = "east";
	directions[NORTHWEST] = "northwest";
	directions[NORTHEAST] = "northeast";
	directions[SOUTHWEST] = "southwest";
	directions[SOUTHEAST] = "southeast";
	directions[UP]        = "up";
	directions[DOWN]      = "down";

	directions[NORTH+(NUM_DIRECTIONS/2)]     = "n";
	directions[SOUTH+(NUM_DIRECTIONS/2)]     = "s";
	directions[WEST+(NUM_DIRECTIONS/2)]      = "w";
	directions[EAST+(NUM_DIRECTIONS/2)]      = "e";
	directions[NORTHWEST+(NUM_DIRECTIONS/2)] = "nw";
	directions[NORTHEAST+(NUM_DIRECTIONS/2)] = "ne";
	directions[SOUTHWEST+(NUM_DIRECTIONS/2)] = "sw";
	directions[SOUTHEAST+(NUM_DIRECTIONS/2)] = "se";
	directions[UP+(NUM_DIRECTIONS/2)]        = "u";
	directions[DOWN+(NUM_DIRECTIONS/2)]      = "d";

}

/** This is used to get a zone and the given index as if they
  * were in a list. The current Zone is set to the one at the
  * index */
CMapZone *CMapData::getZoneAt(int index)
{
	int i = 0;
	for (CMapZone *zone = getFirstZone(); zone !=0; zone=getNextZone())
	{
		if (i == index)
			return zone;

		i++;
	}

	currentZone = NULL;
	return NULL;
}

/** This method is used to get the first sub zone of a parent zone
  * @param parent The parent zone, it is the fist sub zone of the parent that is returned
  * @return The first sub zone of the parent zone */
CMapZone *CMapData::findFirstSubZone(CMapZone *parent)
{
	CMapZone *result = NULL;

	m_foundCurrentZone = false;

	// Check all the levels of the parent zone for a sub zone, make result equal
    // the first zone that is found.
	for (CMapLevel *level = parent->getLevels()->first();level!=0;level = parent->getLevels()->next())
	{
		CMapZone *zone = level->getZoneList()->first();
		if (zone)
		{
			result = zone;
			break;
		}
	}

	return result;
}

CMapZone *CMapData::getNextSameLevelZone(CMapZone *current)
{
	CMapZone *parent = current->getZone();
	CMapZone *result = NULL;

	if (parent)
	{
		for (CMapLevel *level = parent->getLevels()->first();level!=0; level = parent->getLevels()->next())
		{
			for (CMapZone *zone = level->getZoneList()->first();zone!=0;zone = level->getZoneList()->next())
			{
				if (zone==current)
				{
					m_foundCurrentZone = true;
					continue;
				}

				if (m_foundCurrentZone)
				{
					result = zone;
					return zone;
					break;
				}
			}
		}

		// Go up a zone and try again because a zone was not found at this level
		if (result==NULL)
		{
			m_foundCurrentZone = false;
			result = getNextSameLevelZone(parent);
		}
	}

	return result;
}


