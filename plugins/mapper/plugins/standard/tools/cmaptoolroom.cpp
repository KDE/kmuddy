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

#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

#include <qcursor.h>

#include "../../../cmapmanager.h"
#include "../../../cmapviewbase.h"

CMapToolRoom::CMapToolRoom(KActionCollection *actionCollection,CMapManager *manager,QObject *parent)
	: CMapToolBase(actionCollection,
                   i18n("Create Room"),
                   BarIcon("kmud_room.png"),
                   manager,"toolsRoom",0,parent)

{
}

CMapToolRoom::~CMapToolRoom()
{
}

/** Called when the tool recives a mouse release event */
void CMapToolRoom::mouseReleaseEvent(QPoint mousePos,CMapLevel *currentLevel)
{
	//FIXME_jp : Check to see if the room was placed at the edge of the map and move map if nescerary

	//Create the room
	mapManager->createRoom(mapManager->cordsSnapGrid(mousePos),currentLevel);

	lastPos=QPoint(-100,-100);
}

/** Called when the tool recives a mouse move event */
void CMapToolRoom::mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *)
{
	int width = mapManager->getMapData()->gridSize.width();
	int height = mapManager->getMapData()->gridSize.height();

	QPoint pos = mapManager->cordsSnapGrid(mousePos);

// TODO: this must be done by calling a repaint event, not like this
#if 0
	p->setPen(Qt::black);
	p->setBrush(QColor(Qt::black));

	// p->setRasterOp(Qt::NotROP);  //TODO FIXME

	if (pos.x()>=0 && pos.y()>=0)
	{
		// Erase old one
		p->drawRect(lastPos.x()+1,lastPos.y()+1,width-2,height-2);

		// Draw new one
		p->drawRect(pos.x()+1,pos.y()+1,width-2,height-2);
   	}
#endif
	lastPos=pos;
}

void CMapToolRoom::mouseEnterEvent()
{

	kDebug() << "CMapToolRoom: mouseEnterEvent";
}

void CMapToolRoom::mouseLeaveEvent()
{
	kDebug() << "CMapToolRoom: mouseLeaveEvent";
}

/** This function called when a tool is selected */
void CMapToolRoom::toolSelected(void)
{
	CMapViewBase *view = mapManager->getActiveView();
	if (view)
	{
		view->setMouseTracking(true);
		view->setCursor(Qt::ArrowCursor);	
	}
	lastPos=QPoint(-100,-100);
}

/** This function is called when a tool is unselected */
void CMapToolRoom::toolUnselected(void)
{
	if (mapManager->getActiveView())
		mapManager->getActiveView()->redraw();
}

