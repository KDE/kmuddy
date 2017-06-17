/***************************************************************************
                               cmapcmdmoveelements.h
                             -------------------
    begin                : Mon Jun 17 2002
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

#ifndef CMAPCMDMOVEELEMENTS_H
#define CMAPCMDMOVEELEMENTS_H

#include "../../cmapcommand.h"

#include <QLinkedList>
#include <QPoint>


class CMapManager;
class CMapElement;

/**
  *@author Kmud Developer Team
  */

class CMapCmdMoveElements : public CMapCommand
{
public: 
	CMapCmdMoveElements(CMapManager *mapManager,QPoint offset);
	~CMapCmdMoveElements();

	void addElement(CMapElement *element);

	void redo();
	void undo();

private:
	struct elemProp
	{
		int level;
		QPoint pos;
		int labelPos;
	};
	typedef QLinkedList<struct elemProp> PropList;

	PropList elements;
	CMapManager *m_mapManager;
	QPoint m_offset;
};

#endif
