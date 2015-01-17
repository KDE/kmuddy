/***************************************************************************
                               cmaptoolzone.h
                             -------------------
    begin                : Fri Aug 10 2001
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

#ifndef CMAPTOOLZONE_H
#define CMAPTOOLZONE_H

#include <qobject.h>
#include <qpoint.h>

#include "../../../cmaptoolbase.h"

/**This tool is used to create zones
  *@author Kmud Developer Team
  */

class CMapToolZone : public CMapToolBase
{
   Q_OBJECT
public: 
	CMapToolZone(KActionCollection *actionCollection,CMapManager *manager,QObject *parent=0);
	~CMapToolZone();

	/** Called when the tool recives a mouse release event */
	virtual void mouseReleaseEvent(QPoint mousePos,CMapLevel *currentLevel);
	/** Called when the tool recives a mouse move event */
	virtual void mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *currentLevel);

	/** This method called when a tool is selected */
	virtual void toolSelected(void);
	/** This method is called when a tool is unselected */
	virtual void toolUnselected(void);

private:
	QPoint lastPos;
};

#endif
