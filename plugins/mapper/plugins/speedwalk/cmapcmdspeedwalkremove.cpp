/***************************************************************************
                               cmapcmdspeedwalkremove.cpp
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

#include "cmapcmdspeedwalkremove.h"

#include "cmappluginspeedwalk.h"

#include "../../cmapmanager.h"
#include "../../cmaproom.h"
#include "../../cmaplevel.h"

#include <klocale.h>

CMapCmdSpeedwalkRemove::CMapCmdSpeedwalkRemove(CMapPluginSpeedwalk *plugin)  : CMapCommand (i18n("Remove Room(s) from speedwalk list"))
{
	m_plugin = plugin;
	m_levelList.clear();
	m_roomList.clear();
}

CMapCmdSpeedwalkRemove::~CMapCmdSpeedwalkRemove()
{
}

void CMapCmdSpeedwalkRemove::addRoom(CMapRoom *room)
{
	m_levelList.append(room->getLevel()->getLevelID());
	m_roomList.append(room->getRoomID());
}

void CMapCmdSpeedwalkRemove::execute()
{
	for (unsigned int i=0;i<m_levelList.count();i++)
	{	
		CMapLevel *level = m_plugin->getManager()->findLevel(*m_levelList.at(i));
		if (level)
		{
			CMapRoom *room = level->findRoom(*m_roomList.at(i));
	    	if (room)
			{
				m_plugin->delSpeedwalkRoomNoCmd(room,false);
			}
		}	
	}

	m_plugin->updateSpeedwalkList();
}

void CMapCmdSpeedwalkRemove::unexecute()
{
	for (unsigned int i=0;i<m_levelList.count();i++)
	{	
		CMapLevel *level = m_plugin->getManager()->findLevel(*m_levelList.at(i));
		if (level)
		{
			CMapRoom *room = level->findRoom(*m_roomList.at(i));
	    	if (room)
			{
				m_plugin->addSpeedwalkRoomNoCmd(room,false);
			}
		}	
	}

	m_plugin->updateSpeedwalkList();
}
