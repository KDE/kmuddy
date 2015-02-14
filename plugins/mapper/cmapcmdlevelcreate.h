/***************************************************************************
                               cmapcmdlevelcreate.h
                             -------------------
    begin                : Wed Mar 6 2002
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

#ifndef CMAPCMDLEVELCREATE_H
#define CMAPCMDLEVELCREATE_H

#include "cmapcommand.h"

class CMapManager;
class CMapLevel;
class CMapZone;

/**This class is the command used to create a level
  *@author Kmud Developer Team
  */

class CMapCmdLevelCreate : public CMapCommand
{
public: 
	CMapCmdLevelCreate(CMapManager *manager,QString name,CMapZone *intoZone,int index);
	~CMapCmdLevelCreate();

	virtual void redo();
	virtual void undo();

	CMapLevel *getLevel(void);

private:
	CMapManager *m_mapManager;
	CMapLevel *m_level;
	int m_levelID;
	int m_index;
	int m_intoZone;
};

#endif
