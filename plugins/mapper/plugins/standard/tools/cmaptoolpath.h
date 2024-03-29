/***************************************************************************
                               cmaptoolpath.h
                             -------------------
    begin                : Mon Aug 20 2001
    copyright            : (C) 2001 by Kmud Developer Team
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

#ifndef CMAPTOOLPATH_H
#define CMAPTOOLPATH_H

#include <qobject.h>
#include <qpoint.h>
#include <qcursor.h>

#include "../../../cmaptoolbase.h"

class CMapRoom;




/**This tool is used to create paths between rooms
  *@author Kmud Developer Team
  */

class CMapToolPath : public CMapToolBase
{
public: 
	CMapToolPath(KActionCollection *actionCollection,CMapManager *manager);
	~CMapToolPath() override;

	/** Called when the tool recives a mouse release event */
	void mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;

	/** This function called when a tool is selected */
	void toolSelected(void) override;

private:
	QCursor *currentCursor;
	QCursor *pathStartCursor;
	QCursor *pathEndCursor;
	CMapRoom *pathStartRoom;
	int pathToolMode;

};

#endif
