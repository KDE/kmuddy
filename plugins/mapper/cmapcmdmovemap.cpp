/***************************************************************************
                               cmapcmdmovemap.cpp
                             -------------------
    begin                : Thu Feb 28 2002
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

#include "cmapcmdmovemap.h"

#include "cmaplevel.h"
#include "cmaproom.h"
#include "cmappath.h"
#include "cmapmanager.h"

CMapCmdMoveMap::CMapCmdMoveMap(CMapManager *manager,QPoint offset,QString name) : CMapCommand(name)
{
  m_Offset = offset;
  m_manager = manager;
}

CMapCmdMoveMap::~CMapCmdMoveMap()
{
}


void CMapCmdMoveMap::redo()
{
  moveMap(m_Offset);
}

void CMapCmdMoveMap::undo()
{
  moveMap(QPoint(0, 0) - m_Offset);
}

/** This method is used to move the elements in a zone by the given vector */
void CMapCmdMoveMap::moveMap(QPoint inc)
{
  foreach (CMapLevel *level, *m_manager->getZone()->getLevels())
  {
    foreach (CMapElement *el, level->getAllElements())
    {
      el->moveBy(inc);
      CMapRoom *room = dynamic_cast<CMapRoom *>(el);
      if (room) {
        foreach (CMapPath *path, *room->getPathList())
          path->moveBy(inc);
      }
    }
  }
}
