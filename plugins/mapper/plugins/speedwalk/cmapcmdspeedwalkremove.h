/***************************************************************************
                               cmapcmdspeedwalkremove.h
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

#ifndef CMAPCMDSPEEDWALKREMOVE_H
#define CMAPCMDSPEEDWALKREMOVE_H

#include "../../cmapcommand.h"
#include <q3valuelist.h>

class CMapRoom;
class CMapManager;
class CMapPluginSpeedwalk;

/**
  *@author Kmud Developer Team
  */

class CMapCmdSpeedwalkRemove : public CMapCommand
{
public: 
	CMapCmdSpeedwalkRemove(CMapPluginSpeedwalk *plugin);
	~CMapCmdSpeedwalkRemove();

	virtual void redo();
	virtual void undo();

	void addRoom(CMapRoom *room);

private:
	Q3ValueList<int> m_levelList;
	Q3ValueList<int> m_roomList;

	CMapPluginSpeedwalk *m_plugin;
};

#endif
