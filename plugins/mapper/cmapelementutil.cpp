/***************************************************************************
                               cmapelementutil.cpp
                             -------------------
    begin                : Thu Jan 10 2002
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

#include "cmapelementutil.h"

#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmaproom.h"
#include "cmaptext.h"
#include "cmapzone.h"
#include "cmappath.h"
#include "cmapelement.h"
#include "cmapviewbase.h"

#include "dialogs/dlgmappathproperties.h"

#include <kdebug.h>

CMapElementUtil::CMapElementUtil(CMapManager *mapManager)
{
	manager = mapManager;
}

CMapElementUtil::~CMapElementUtil()
{
}

void CMapElementUtil::deleteElement(KConfigGroup grp,bool delOpsite)
{
	CMapElement *element = manager->findElement(grp);

	if (element)
	{
		switch (element->getElementType())
		{
			case PATH : deletePath((CMapPath *)element,delOpsite); break;
			case ZONE : delete ((CMapZone *)element); break;
			case TEXT : delete ((CMapText *)element); break;
			case ROOM : delete ((CMapRoom *)element); break;
			default: break;
		}
	}
}

CMapElement *CMapElementUtil::createElement(KConfigGroup grp)
{
	CMapElement *result = NULL;
	QString text;

	elementTyp type  = (elementTyp)grp.readEntry("Type",(int)OTHER);
	
	if (type!=OTHER)
	{
		if  (type==PATH)
		{
			CMapLevel *srcLevel = manager->findLevel(grp.readEntry("SrcLevel",-1));
			CMapLevel *destLevel = manager->findLevel(grp.readEntry("DestLevel",-1));
			CMapRoom *srcRoom = srcLevel->findRoom(grp.readEntry("SrcRoom",-1));
			CMapRoom *destRoom = destLevel->findRoom(grp.readEntry("DestRoom",-1));

			directionTyp srcDir	 = (directionTyp)grp.readEntry("SrcDir",0);
			directionTyp destDir = (directionTyp)grp.readEntry("DestDir",0);

			result = new CMapPath(manager,srcRoom,srcDir,destRoom,destDir);

			((CMapPath *)result)->loadProperties(grp);
		}
		else
		{	
			CMapLevel *level = NULL;
			if (grp.hasKey("Level"))
			{
				level = manager->findLevel(grp.readEntry("Level",-1));
			}

			QPoint lowPos(-5,-5);
			lowPos.setX(grp.readEntry("X",-5));
			lowPos.setY(grp.readEntry("Y",-5));

			switch (type)
			{
				case ROOM : result = createRoom(lowPos,level);
				            if (result)
				            {
				            	if (!grp.hasKey("RoomID"))
				            	{
				            		grp.writeEntry("RoomID",((CMapRoom *)result)->getRoomID());
				            	}
				            	else
				            	{
				            		((CMapRoom *)result)->setRoomID(grp.readEntry("RoomID",-1));
				            	}
				            }
				            break;
				case TEXT : text = grp.readEntry("Text","");
				            if (grp.hasKey("Font") && grp.hasKey("Color"))
				            {
				            	QColor color=Qt::black;
				                color=grp.readEntry("Color",color);
				            	QFont font = manager->getMapData()->defaultTextFont;
				            	font = grp.readEntry("Font",font);
				            	result = new CMapText(text, font, color, manager, lowPos, level);
				            }
				            else
				            {
				            	result = new CMapText(text, manager, lowPos, level);
				            }
						if (!grp.hasKey("TextID"))
							grp.writeEntry("TextID",((CMapText *)result)->getTextID());									
						else
							((CMapText *)result)->setTextID(grp.readEntry("TextID",-1));
				            break;
				case ZONE : result = createZone(lowPos,level);
				            if (result)
				            {
				            	if (!grp.hasKey("ZoneID"))
				            	{
				            		grp.writeEntry("ZoneID",((CMapZone *)result)->getZoneID());
				            	}
				            	else
				            	{
				            		((CMapZone *)result)->setZoneID(grp.readEntry("ZoneID",-1));
				            	}
				            }
				            break;
				default : ;
			}

			// FIXME_jp: This should problay load the elements propties into the new element.
			//           Calls to loadProperties else were in the code will not then be need after
			//           creating the new element
		}
	}

	return result;
}

/** Used to create a new room */
CMapRoom *CMapElementUtil::createRoom(QPoint pos,CMapLevel *level)
{
  if ((!level) || level->findElementAt(pos))
    return NULL;

  QRect rect(pos,manager->getMapData()->gridSize);

  CMapRoom *room = new CMapRoom(manager,rect,level);

  return room;
}

/** Used to create a new zone */
CMapZone *CMapElementUtil::createZone(QPoint pos,CMapLevel *level)
{
  if (level && level->findElementAt(pos))
    return NULL;

  QRect rect(pos,manager->getMapData()->gridSize);

  return new CMapZone (manager,rect,level);
}

/** Delete a path map element */
void CMapElementUtil::deletePath(CMapPath *path,bool delOpsite)
{
  if (delOpsite) path->setOpsitePath(NULL);
  delete path;

}
