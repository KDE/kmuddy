/***************************************************************************
                               cmapcmdlevelcreate.cpp
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

#include "cmapcmdlevelcreate.h"

#include "cmapmanager.h"
#include "cmaplevel.h"

CMapCmdLevelCreate::CMapCmdLevelCreate(CMapManager *manager,QString name,int index)  : CMapCommand(name)
{
	m_mapManager = manager;
	m_index = index;
	m_levelID = -1;
	m_level = NULL;
}

CMapCmdLevelCreate::~CMapCmdLevelCreate()
{
}

void CMapCmdLevelCreate::redo()
{
  m_level = new CMapLevel(m_mapManager, m_index);

  if (m_levelID == -1)
  {
    m_levelID = m_level->getLevelID();
  }
  else
  {
    m_level->setLevelID(m_levelID);
  }
}

void CMapCmdLevelCreate::undo()
{
	CMapLevel *level = m_mapManager->findLevel(m_levelID);
	delete level;
}

CMapLevel *CMapCmdLevelCreate::getLevel(void)
{
  return m_level;
}
