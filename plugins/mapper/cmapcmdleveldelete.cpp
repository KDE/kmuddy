/***************************************************************************
                               cmapcmdleveldelete.cpp
                             -------------------
    begin                : Mon Mar 4 2002
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

#include "cmapcmdleveldelete.h"

#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmapzone.h"

CMapCmdLevelDelete::CMapCmdLevelDelete(CMapManager *manager,QString name, CMapLevel *level) : CMapCommand(name),CMapLevelUtil(manager)
{
	m_mapManager = manager;
	m_levelID = level->getLevelID();
	m_zoneIntoID = level->getZone()->getZoneID();
	m_index = -1;
}

CMapCmdLevelDelete::~CMapCmdLevelDelete()
{
}

void CMapCmdLevelDelete::execute()
{
	CMapLevel *level = m_mapManager->findLevel(m_levelID);
	m_index = deleteLevel(level);
}

void CMapCmdLevelDelete::unexecute()
{
	CMapZone *zone = m_mapManager->findZone(m_zoneIntoID);
	CMapLevel *level = createLevel(m_index,zone);
	level->setLevelID(m_levelID);
}
