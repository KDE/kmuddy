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
	commands.setAutoDelete(true);
}

CMapCmdGroup::~CMapCmdGroup()
{
}

void CMapCmdGroup::execute()
{
	for (K3Command *c = commands.first(); c!=NULL; c = commands.next())
	{
		c->execute();
	}
}

void CMapCmdGroup::unexecute()
{
	m_mapManager->setUndoActive(false);

	for (K3Command *c = commands.last(); c!=NULL; c = commands.prev())
	{
		c->unexecute();
	}

	m_mapManager->setUndoActive(true);
}


void CMapCmdGroup::addCommand(K3Command *command,bool execute)
{
	commands.append(command);
	if (execute)
		command->execute();
}

void CMapCmdGroup::setPreviousGroup(CMapCmdGroup *group)
{
	previousGroup = group;
}

CMapCmdGroup *CMapCmdGroup::getPreviousGroup(void)
{
	return previousGroup;
}

