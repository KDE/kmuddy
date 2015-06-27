/***************************************************************************
                            cmapcmdmoveplayer.cpp
                             -------------------
    begin                : Sun Mar 29 2015
    copyright            : (C) 2015 by Tomas Mecir
    email                : mecirt@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cmapcmdmoveplayer.h"

#include "cmapelement.h"
#include "cmapelementutil.h"
#include "cmaplevel.h"
#include "cmappath.h"
#include "cmaproom.h"
#include "cmapview.h"
#include "cmapmanager.h"

CMapCmdMovePlayer::CMapCmdMovePlayer(CMapManager *mapManager, directionTyp direction, QString specialCmd, bool create) :
  CMapCommand("Move Player"),
  m_manager(mapManager),
  m_create(create),
  m_special(specialCmd),
  m_direction(direction),
  m_newroom(0),
  m_newpath(0),
  m_createdlevel(0)
{
}

CMapCmdMovePlayer::~CMapCmdMovePlayer()
{
}

void CMapCmdMovePlayer::redo()
{
  m_createdlevel = 0;
  m_newpath = 0;
  m_newroom = 0;

  m_origroom = m_manager->getCurrentRoom();
  if (!m_origroom) return;
  CMapRoom *tgroom = m_origroom->getPathTarget(m_direction, m_special);
  if (tgroom) {
    m_manager->setCurrentRoom(tgroom);
    return;
  }

  // Nothing suitable -- if we can't create, we are done
  if (!m_create) return;

  CMapRoom *srcRoom = m_origroom;
  CMapPath *oppositePath = NULL;

  CMapLevel *destLevel = m_origroom->getLevel();

  int x = 0, y = 0;
  // Check to see if there is a path in the opposite direction that we should be using
  foreach (CMapPath *path2, *srcRoom->getConnectingPathList())
  {
    if (path2->getDestDir() == m_direction)
    {
      tgroom = path2->getSrcRoom();
      destLevel = tgroom->getLevel();
      x = tgroom->getX();
      y = tgroom->getY();
      oppositePath = path2;
      break;
    }
  }

  if (!tgroom) {
    QSize grid = m_manager->getMapData()->gridSize;
    // Nothing found, so let's see if there is already a room where we are going
    QPoint inc;
    m_manager->directionToCord(m_direction,QSize(grid.width()*2, grid.height()*2),&inc);
    x = srcRoom->getX() + inc.x();
    y = srcRoom->getY() + inc.y();

    if ((m_direction == UP) || (m_direction == DOWN))
    {
      int pos = (m_direction == UP) ? destLevel->getZone()->getLevels()->count() : 0;
      destLevel = (m_direction == UP) ? destLevel->getNextLevel() : destLevel->getPrevLevel();
      if (!destLevel) {
        destLevel = new CMapLevel (m_manager, pos);
        m_createdlevel = destLevel;
      }
    }

    // Check to see if the map needs to be moved
    // and calulate the offset to move if it needs moving.
    if (x < grid.width()*3 || y < grid.height()*2)
    {
      int movex, movey;

      if (x < grid.width()*3)
      {
        movex = grid.width()*3 - x;
        x+=movex;
      }
      else
        movex = 0;

      if (y < grid.height()*3)
      {
        movey = grid.height() * 3- y;
        y+=movey;
      }
      else
        movey = 0;

      m_manager->moveMap (QPoint(movex,movey),m_origroom->getZone());
    }

    // Check to see if the room already exists
    tgroom = (CMapRoom *) destLevel->findElementAt(QPoint (x,y), ROOM);

    // if it doesn't, we need to create one
    if (!tgroom) {
      tgroom = CMapElementUtil::createRoom(m_manager, QPoint (x,y),destLevel);
      m_newroom = tgroom;
    }
  }

  directionTyp destDir = m_manager->getOpsiteDirection(m_direction);
  // Create the new path to the room
  // TODO: move path creation to utils!
  bool undo = m_manager->getUndoActive();
  m_manager->setUndoActive(false);
  CMapPath *newPath = m_manager->createPath(srcRoom, m_direction, tgroom, destDir);
  m_newpath = newPath;

  // Make the path two way if the default path type is two way
  if (m_manager->getMapData()->defaultPathTwoWay && (!oppositePath))
    newPath->makeTwoWay();

  m_manager->setCurrentRoom(tgroom);
  m_manager->setUndoActive(undo);
}

void CMapCmdMovePlayer::undo()
{
  // move player back to the original location
  if (m_origroom) m_manager->setCurrentRoom(m_origroom);
  // delete the created room/exit if needed
  if (m_create) {
    delete m_newpath;
    delete m_newroom;
    delete m_createdlevel;
    m_newroom = 0;
    m_newpath = 0;
    m_createdlevel = 0;
  }
}

