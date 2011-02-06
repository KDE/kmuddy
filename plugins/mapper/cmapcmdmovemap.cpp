/***************************************************************************
                               cmapcmdmovemap.cpp
                             -------------------
    begin                : Thu Feb 28 2002
    copyright            : (C) 2002 by Kmud Developer Team
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

#include "cmapcmdmovemap.h"

#include "cmapzone.h"
#include "cmaplevel.h"
#include "cmaproom.h"
#include "cmaptext.h"
#include "cmappath.h"
#include "cmapmanager.h"

CMapCmdMoveMap::CMapCmdMoveMap(CMapManager *manager,QPoint offset,CMapZone *zone,QString name) : CMapCommand(name)
{
	m_posOffset = offset;
	m_negOffset = QPoint (0,0) - offset;
	m_zoneId = zone->getZoneID();
	m_manager = manager;
}

CMapCmdMoveMap::~CMapCmdMoveMap()
{
}


void CMapCmdMoveMap::execute()
{
	CMapZone *zone = m_manager->findZone(m_zoneId);
	moveMap(m_posOffset,zone);
}

void CMapCmdMoveMap::unexecute()
{
	CMapZone *zone = m_manager->findZone(m_zoneId);
	moveMap(m_negOffset,zone);
}

/** This method is used to move the elements in a zone by the given vector */
void CMapCmdMoveMap::moveMap(QPoint inc,CMapZone *zone)
{
	// Move the rooms
	for (CMapLevel *level=zone->getLevels()->first(); level !=0;level = zone->getLevels()->next())
	{
		for (CMapRoom *room=level->getRoomList()->first();room !=0 ; room =level->getRoomList()->next())
		{
			room->moveBy(inc);

			for (CMapPath *path=room->getPathList()->first();path!=0; path=room->getPathList()->next())
			{
				path->moveBy(inc);
			}
		}

		for (CMapZone *zone=level->getZoneList()->first();zone !=0 ; zone = level->getZoneList()->next())
		{
			zone->moveBy(inc);
		}

		for (CMapText *text=level->getTextList()->first();text!=0; text = level->getTextList()->next())
		{
			text->moveBy(inc);
		}
	}
}
