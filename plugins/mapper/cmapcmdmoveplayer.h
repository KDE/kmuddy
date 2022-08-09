/***************************************************************************
                             cmapcmdmoveplayer.h
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

#ifndef CMAPCMDMOVEPLAYEAR_H
#define CMAPCMDMOVEPLAYEAR_H

#include "cmapcommand.h"
#include "cmapelement.h"

class CMapRoom;
class CMapPath;
class CMapLevel;
class CMapManager;

class CMapCmdMovePlayer : public CMapCommand
{
public:
  CMapCmdMovePlayer(CMapManager *mapManager, directionTyp direction, QString specialCmd, bool create);
  ~CMapCmdMovePlayer() override;
  void redo() override;
  void undo() override;

private:
  CMapManager *m_manager;
  bool m_create;
  QString m_special;
  directionTyp m_direction;

  CMapRoom *m_origroom, *m_newroom;
  CMapPath *m_newpath;
  CMapLevel *m_createdlevel;
};


#endif
