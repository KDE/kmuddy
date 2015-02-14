/***************************************************************************
                               cmapcmdgroup.cpp
                             -------------------
    begin                : Wed Feb 27 2002
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

#include "cmapcmdgroup.h"

#include "cmapmanager.h"

CMapCmdGroup::CMapCmdGroup(CMapManager *mapManager,QString name) : CMapCommand(name)
{
m_mapManager = mapManager;
}

CMapCmdGroup::~CMapCmdGroup()
{
  foreach (CMapCommand *cmd, commands)
    delete cmd;
}

void CMapCmdGroup::redo()
{
  foreach (CMapCommand *c, commands)
    c->redo();
}

void CMapCmdGroup::undo()
{
  m_mapManager->setUndoActive(false);

  foreach (CMapCommand *c, commands)
    c->undo();

  m_mapManager->setUndoActive(true);
}


void CMapCmdGroup::addCommand(CMapCommand *command)
{
  commands.append(command);
}

void CMapCmdGroup::setPreviousGroup(CMapCmdGroup *group)
{
  previousGroup = group;
}

CMapCmdGroup *CMapCmdGroup::getPreviousGroup(void)
{
  return previousGroup;
}

