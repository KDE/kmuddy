/***************************************************************************
                               cmapcmddeletelevel.h
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

#ifndef CMAPCMDLEVELDELETE_H
#define CMAPCMDLEVELDELETE_H

#include <qstring.h>

#include <k3command.h>
#include <kvbox.h>

#include "cmaplevelutil.h"

#include "cmapcommand.h"

class CMapManager;
class CMapLevel;

/**This class is the command used to delete a level
  *@author Kmud Developer Team
  */

class CMapCmdLevelDelete : public CMapCommand,CMapLevelUtil
{
public: 
	CMapCmdLevelDelete(CMapManager *manager,QString name, CMapLevel *level);
	~CMapCmdLevelDelete();

	virtual void execute();
	virtual void unexecute();

private:
	int m_index;
	int m_levelID;
	int m_zoneIntoID;
	CMapManager *m_mapManager;
};

#endif
