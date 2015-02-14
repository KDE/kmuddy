/***************************************************************************
                               cmapcmdmovemap.h
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

#ifndef CMAPCMDMOVEMAP_H
#define CMAPCMDMOVEMAP_H

#include <qpoint.h>

#include "cmapcommand.h"

class CMapZone;
class CMapManager;

/**This command is used to move all elements in the current zone
  *@author Kmud Developer Team
  */
class CMapCmdMoveMap : public CMapCommand
{
public:
	CMapCmdMoveMap(CMapManager *manager,QPoint offset,CMapZone *zone,QString name);
	~CMapCmdMoveMap();

	virtual void redo();
	virtual void undo();

private:
	/** This method is used to move the elements in a zone by the given vector */
	void moveMap(QPoint inc,CMapZone *zone);

private:
	/** The map manager */
	CMapManager *m_manager;
	/** The zones elements which are to be moved */
	int m_zoneId;
	/** The offset to move the elements */
	QPoint m_Offset;

};

#endif
