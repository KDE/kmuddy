/***************************************************************************
                               cmaptoolzone.cpp
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

#include "cmaptoolzone.h"

#include <qcolor.h>

#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

#include <qcursor.h>

#include "../../../cmapmanager.h"
#include "../../../cmapviewbase.h"


CMapToolZone::CMapToolZone(KActionCollection *actionCollection,CMapManager *manager,QObject *parent)
	: CMapToolBase(actionCollection,
                   i18n("Create Zone"),
                   BarIcon("kmud_zone.png"),
                   manager,"toolsZone",0,parent)
{
}

CMapToolZone::~CMapToolZone()
{
}

void CMapToolZone::paint(QPainter *p)
{
  if ((lastPos.x() < 0) || (lastPos.y() < 0)) return;

  p->setPen(QColor(255, 255, 255, 128));
  p->setBrush(QColor(0, 255, 255, 64));

  QSize gSize = mapManager->getMapData()->gridSize;
  p->drawRect(lastPos.x()+1,lastPos.y()+1, gSize.width()-2,gSize.height()-2);
}


/** Called when the tool recives a mouse release event */
void CMapToolZone::mouseReleaseEvent(QPoint mousePos,CMapLevel *currentLevel)
{
	//FIXME_jp : Check to see if the zone was placed at the edge of the map and move map if nescerary

	//Create the room
	mapManager->createZone(mapManager->cordsSnapGrid(mousePos),currentLevel);

	lastPos=QPoint(-100,-100);
        mapManager->getActiveView()->requestPaint();
}

/** Called when the tool recives a mouse move event */
void CMapToolZone::mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *)
{
	lastPos = mapManager->cordsSnapGrid(mousePos);
        mapManager->getActiveView()->requestPaint();
}

/** This function called when a tool is selected */
void CMapToolZone::toolSelected(void)
{
	CMapViewBase *view = mapManager->getActiveView();
	if (view)
	{
		view->setMouseTracking(true);
		view->setCursor(Qt::ArrowCursor);	
	}
	lastPos=QPoint(-100,-100);
}

