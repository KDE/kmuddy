/***************************************************************************
                               cmaptoolroom.cpp
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

#include "cmaptoolroom.h"

#include <qcolor.h>

#include <KLocalizedString>

#include "../../../cmapmanager.h"
#include "../../../cmapview.h"

CMapToolRoom::CMapToolRoom(KActionCollection *actionCollection,CMapManager *manager)
	: CMapToolBase(actionCollection,
                   i18n("Create Room"),
                   QIcon::fromTheme("kmud_room.png"),
                   manager,"toolsRoom",nullptr)

{
}

CMapToolRoom::~CMapToolRoom()
{
}

void CMapToolRoom::paint(QPainter *p)
{
  if ((lastPos.x() < 0) || (lastPos.y() < 0)) return;

  p->setPen(QColor(255, 255, 255, 128));
  p->setBrush(QColor(0, 255, 255, 64));

  QSize gSize = mapManager->getMapData()->gridSize;
  p->drawRect(lastPos.x()+1,lastPos.y()+1, gSize.width()-2,gSize.height()-2);
}

/** Called when the tool recives a mouse release event */
void CMapToolRoom::mouseReleaseEvent(QPoint mousePos, QMouseEvent * /*e*/, CMapLevel *currentLevel)
{
	//FIXME_jp : Check to see if the room was placed at the edge of the map and move map if nescerary

	//Create the room
	mapManager->createRoom(mapManager->cordsSnapGrid(mousePos),currentLevel);

	lastPos=QPoint(-100,-100);
        mapManager->getActiveView()->requestPaint();
}

/** Called when the tool recives a mouse move event */
void CMapToolRoom::mouseMoveEvent(QPoint mousePos, Qt::KeyboardModifiers, Qt::MouseButtons, CMapLevel *)
{
	lastPos = mapManager->cordsSnapGrid(mousePos);
        mapManager->getActiveView()->requestPaint();
}

void CMapToolRoom::mouseEnterEvent()
{
}

void CMapToolRoom::mouseLeaveEvent()
{
	lastPos=QPoint(-100,-100);
        mapManager->getActiveView()->requestPaint();
}

/** This function called when a tool is selected */
void CMapToolRoom::toolSelected(void)
{
	CMapView *view = mapManager->getActiveView();
	if (view)
	{
		view->setMouseTracking(true);
		view->setCursor(Qt::ArrowCursor);	
	}
	lastPos=QPoint(-100,-100);
}

#include "moc_cmaptoolroom.cpp"
