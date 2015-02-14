/***************************************************************************
                               cmapcmdspeedwalkadd.h
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

#ifndef CMAPCMDSPEEDWALKADD_H
#define CMAPCMDSPEEDWALKADD_H

#include "../../cmapcommand.h"

class CMapRoom;
class CMapManager;
class CMapPluginSpeedwalk;

/**
  *@author Kmud Developer Team
  */

class CMapCmdSpeedwalkAdd : public CMapCommand
{
public: 
	CMapCmdSpeedwalkAdd(CMapPluginSpeedwalk *plugin,CMapRoom *room);
	~CMapCmdSpeedwalkAdd();

	virtual void redo();
	virtual void undo();

private:

	int m_roomID;
	int m_levelID;
	CMapPluginSpeedwalk *m_plugin;
};

#endif
