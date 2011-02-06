/***************************************************************************
                               cmapcmdspeedwalkadd.cpp
                             -------------------
    begin                : Thu Mar 14 2002
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

#include "cmapcmdspeedwalkadd.h"

#include "cmappluginspeedwalk.h"

#include "../../cmapmanager.h"
#include "../../cmaproom.h"
#include "../../cmaplevel.h"

#include <klocale.h>

CMapCmdSpeedwalkAdd::CMapCmdSpeedwalkAdd(CMapPluginSpeedwalk *plugin,CMapRoom *room) : CMapCommand (i18n("Add Room to speedwalk list"))
{
	m_plugin = plugin;
	m_levelID = room->getLevel()->getLevelID();
	m_roomID = room->getRoomID();
}

CMapCmdSpeedwalkAdd::~CMapCmdSpeedwalkAdd()
{
}

void CMapCmdSpeedwalkAdd::execute()
{
	CMapLevel *level = m_plugin->getManager()->findLevel(m_levelID);
	if (level)
	{
		CMapRoom *room = level->findRoom(m_roomID);
    	if (room)
		{
			m_plugin->addSpeedwalkRoomNoCmd(room);
		}
	}
}

void CMapCmdSpeedwalkAdd::unexecute()
{
	CMapLevel *level = m_plugin->getManager()->findLevel(m_levelID);
	if (level)
	{
		CMapRoom *room = level->findRoom(m_roomID);
    	if (room)
		{
			m_plugin->delSpeedwalkRoomNoCmd(room);
		}
	}
}
