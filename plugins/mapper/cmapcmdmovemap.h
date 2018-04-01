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

class CMapManager;

/**This command is used to move all elements in the current zone
  *@author Kmud Developer Team
  */
class CMapCmdMoveMap : public CMapCommand
{
public:
	CMapCmdMoveMap(CMapManager *manager,QPoint offset,QString name);
	~CMapCmdMoveMap();

	virtual void redo() override;
	virtual void undo() override;

private:
	/** This method is used to move the elements in a zone by the given vector */
	void moveMap(QPoint inc);

private:
	/** The map manager */
	CMapManager *m_manager;
	/** The offset to move the elements */
	QPoint m_Offset;

};

#endif
