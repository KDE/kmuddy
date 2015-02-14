/***************************************************************************
                               cmapcmdgroup.h
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

#ifndef CMAPCMDGROUP_H
#define CMAPCMDGROUP_H

#include <QList>

#include "cmapcommand.h"

class CMapManager;

class CMapCmdGroup : public CMapCommand
{
public:
	CMapCmdGroup(CMapManager *mapManager,QString name);
	~CMapCmdGroup();
	virtual void redo();
	virtual void undo();
	void addCommand(CMapCommand *command);
	CMapCmdGroup *getPreviousGroup(void);
	void setPreviousGroup(CMapCmdGroup *group);

private:
	CMapManager *m_mapManager;
	/** A list of commands in the group */
	QList<CMapCommand *> commands;
	/** A pointer to the group above this, NULL if it's the main history */
	CMapCmdGroup *previousGroup;
};


#endif
