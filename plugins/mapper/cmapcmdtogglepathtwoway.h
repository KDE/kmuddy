/***************************************************************************
                               cmapcmdtogglepathtwoway.h
                             -------------------
    begin                : Thu Mar 7 2002
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

#ifndef CMAPCMDTOGGLEPATHTWOWAY_H
#define CMAPCMDTOGGLEPATHTWOWAY_H

#include <qstring.h>

#include "cmapelementutil.h"

#include "cmapcommand.h"

class CMapPath;
class CMapManager;
class CMapLevel;
class CMapManager;

/**
  *@author Kmud Developer Team
  */
class CMapCmdTogglePathTwoWay : public CMapCommand,CMapElementUtil
{
public:
	CMapCmdTogglePathTwoWay(CMapManager *mapManager,QString name,CMapPath *path);
	~CMapCmdTogglePathTwoWay();
	
	virtual void redo();
	virtual void undo();

private:
	void togglePath(CMapPath *path);

	CMapManager *m_mapManager;

	int m_srcRoom;
	int m_srcLevel;
	directionTyp m_srcDir;
	QString m_specialCmd;
};

#endif
