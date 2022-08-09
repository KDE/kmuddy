/***************************************************************************
                               cmaptoolroom.h
                             -------------------
    begin                : Wed Aug 8 2001
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

#ifndef CMAPTOOLROOM_H
#define CMAPTOOLROOM_H

#include <qobject.h>
#include <qpoint.h>

#include "../../../cmaptoolbase.h"

/**This tool is used to create rooms
  *@author Kmud Developer Team
  */

class CMapToolRoom : public CMapToolBase
{
   Q_OBJECT
public: 
	CMapToolRoom(KActionCollection *actionCollection,CMapManager *manager);
	~CMapToolRoom() override;

	/** Called when the tool recives a mouse release event */
	void mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;
	/** Called when the tool recives a mouse move event */
	void mouseMoveEvent(QPoint mousePos, Qt::KeyboardModifiers, Qt::MouseButtons, CMapLevel *currentLevel) override;
	void mouseLeaveEvent() override;
	void mouseEnterEvent() override;

	/** This function called when a tool is selected */
	void toolSelected(void) override;

        void paint(QPainter *) override;

private:
	QPoint lastPos;
};

#endif
