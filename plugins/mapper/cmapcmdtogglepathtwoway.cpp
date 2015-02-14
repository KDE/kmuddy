/***************************************************************************
                               cmapcmdtogglepathtwoway.cpp
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

#include "cmapcmdtogglepathtwoway.h"

#include "cmappath.h"
#include "cmaplevel.h"
#include "cmapmanager.h"
#include "cmaproom.h"

CMapCmdTogglePathTwoWay::CMapCmdTogglePathTwoWay(CMapManager *mapManager,QString name,CMapPath *path) : CMapCommand(name),CMapElementUtil(mapManager)
{
	m_srcRoom    = path->getSrcRoom()->getRoomID();
	m_srcLevel   = path->getSrcRoom()->getLevel()->getLevelID();
	m_srcDir     = path->getSrcDir();
	m_specialCmd = path->getSpecialCmd();
	
	m_mapManager = mapManager;	
}

CMapCmdTogglePathTwoWay::~CMapCmdTogglePathTwoWay()
{
}

void CMapCmdTogglePathTwoWay::redo()
{
	CMapLevel *srcLevel = m_mapManager->findLevel(m_srcLevel);
	CMapRoom *srcRoom = srcLevel->findRoom(m_srcRoom);
	CMapPath *path = srcRoom->getPathDirection(m_srcDir,m_specialCmd);
	togglePath (path);
}

void CMapCmdTogglePathTwoWay::undo()
{
	CMapLevel *srcLevel = m_mapManager->findLevel(m_srcLevel);
	CMapRoom *srcRoom = srcLevel->findRoom(m_srcRoom);
	CMapPath *path = srcRoom->getPathDirection(m_srcDir,m_specialCmd);
	togglePath (path);
}

void CMapCmdTogglePathTwoWay::togglePath(CMapPath *path)
{
  if (path->getOpsitePath())
  {
    delete path->getOpsitePath();
  }
  else
  {
    CMapPath *newPath = new CMapPath(m_mapManager, path->getDestRoom(),path->getDestDir(),path->getSrcRoom(),path->getSrcDir());
    if (path->getSpecialExit())
    {
      newPath->setSpecialCmd(path->getSpecialCmd());
      path->setOpsitePath(newPath);
      newPath->setOpsitePath(path);
    }
  }
}
