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

CMapElementUtil::CMapElementUtil(CMapManager *mapManager) : CMapLevelUtil(mapManager)
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
			case ZONE : deleteZone((CMapZone *)element); break;
			case TEXT : deleteText((CMapText *)element); break;
			case ROOM : deleteRoom((CMapRoom *)element); break;
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
			result = createPath(srcRoom,srcDir,destRoom,destDir);

			if (result)
			{
				((CMapPath *)result)->loadProperties(grp);
			}
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
				            	result = createText(lowPos,level,text,font,color);
				            }
				            else
				            {
				            	result = createText(lowPos,level,text);
				            }
				            if (result)
				            {
								if (!grp.hasKey("TextID"))
								{
									grp.writeEntry("TextID",((CMapText *)result)->getTextID());									
								}
								else
								{
									((CMapText *)result)->setTextID(grp.readEntry("TextID",-1));
								}
				            }
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

/** Used to create a new text label */
CMapText *CMapElementUtil::createText(QPoint pos,CMapLevel *level,QString str)
{
	CMapText *text = new CMapText(str,manager,pos,level);

	if (level)
		level->getTextList()->append(text);

	manager->addedElement(text);

	return text;
}

/** Used to create a new text label */
CMapText *CMapElementUtil::createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col)
{
	CMapText *text = new CMapText(str,font,col,manager,pos,level);

	if (level)
		level->getTextList()->append(text);

	manager->addedElement(text);

	return text;
}

/** Used to create a new room */
CMapRoom *CMapElementUtil::createRoom(QPoint pos,CMapLevel *level)
{
	if (manager->findElementAt(pos,level)!=NULL)
	{
		return NULL;	
	}

	QRect rect(pos,manager->getMapData()->gridSize);

	CMapRoom *room = new CMapRoom(manager,rect,level);

	if (level)
		level->getRoomList()->append(room);

	manager->addedElement(room);

	return room;
}

/** Used to create a new zone */
CMapZone *CMapElementUtil::createZone(QPoint pos,CMapLevel *level)
{
	if (manager->findElementAt(pos,level))
		return NULL;

	QRect rect(pos,manager->getMapData()->gridSize);

	CMapZone *zone= new CMapZone (manager,rect,level);

	// Set the root zone if not already set
	if (!manager->getMapData()->rootZone)
		manager->getMapData()->rootZone = zone;

	if (level)
		level->getZoneList()->append(zone);

	//manager->createLevel(UP,zone);
	if (zone != manager->getMapData()->rootZone)
		manager->addedElement(zone);

	return zone;
}

/** This is used to create a path between two rooms */
CMapPath *CMapElementUtil::createPath (CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir)
{
	// Create the path
	CMapPath *newPath = new CMapPath (manager,srcRoom,srcDir,destRoom,destDir);

	// Add the path to the src and dest room
	srcRoom->addPath( newPath );
	destRoom->getConnectingPathList()->append(newPath);

	// Check to see if there is a a path in the opsite directon, if so make this a two way path
	for (CMapPath *path =destRoom->getPathList()->first(); path!=0; path=destRoom->getPathList()->next())
	{
		// FIXME_jp : Fix this for multiple special paths between the same rooms with different cmd's
		if (path->getDestRoom()==srcRoom &&
		    path->getSrcDir() == destDir &&
		    path->getDestDir() == srcDir &&
			path->getSpecialCmd() == newPath->getSpecialCmd())
		{
			newPath->setOpsitePath(path);
			path->setOpsitePath(newPath);
		}
	}

	// Tell the views to redraw the new path
	manager->addedElement(newPath);

	return newPath;
}

/** Delete a zone map element */
void CMapElementUtil::deleteZone(CMapZone *zone)
{
	//FIXME_jp : when this is undone there are extra levels, this needs fixing
	// Delete the levels in the zone
	while (zone->getLevels()->first()!=0)
	{
		kWarning() << "deleteing a zone and found levels that should already have been deleted!!";
		deleteLevel(zone->getLevels()->first());
	}

	CMapLevel *deleteFromLevel = zone->getLevel();
	deleteFromLevel->getZoneList()->remove(zone);

        manager->getActiveView()->deletedElement(deleteFromLevel);
	manager->updateZoneListCombo();
}

/** Delete a text map element */
void CMapElementUtil::deleteText(CMapText *text)
{
	CMapLevel *deleteFromLevel = text->getLevel();

	CMapElement *elm = text->getLinkElement();
	if (elm)
	{
		if (elm->getElementType()==ROOM)
		{
			((CMapRoom *)elm)->textRemove();
		}

		if (elm->getElementType()==ZONE)
		{
			((CMapZone *)elm)->textRemove();
		}
	}

	deleteFromLevel->getTextList()->remove(text);

	manager->getActiveView()->deletedElement(deleteFromLevel);
}

/** Delete a room map element */
void CMapElementUtil::deleteRoom(CMapRoom *room)
{
	//FIXME_jp : Deleting a room with a visiale label causes a crash, this needs fixing
	CMapLevel *roomLevel = room->getLevel();

	if (room->getCurrentRoom())
	{
		if (room->getLevel()->getRoomList()->count()>1)
		{
			CMapRoom *lastRoom =room->getLevel()->getRoomList()->last();
			CMapRoom *firstRoom =room->getLevel()->getRoomList()->first();

			if (firstRoom==room)
			{
				manager->setCurrentRoom(lastRoom);
			}
			else
			{
				manager->setCurrentRoom(firstRoom);
			}
		}
		else
		{
			manager->setCurrentRoom(manager->findFirstRoom(room));
		}
	}

	if (room->getLoginRoom())
	{
		if (room->getLevel()->getRoomList()->count()>1)
		{
			if (room->getLevel()->getRoomList()->first()==room)
			{
				manager->setLoginRoom(room->getLevel()->getRoomList()->last());
			}
			else
			{
				manager->setLoginRoom(room->getLevel()->getRoomList()->first());
			}
		}
		else
			manager->setLoginRoom(manager->findFirstRoom(room));
	}

	// Delete the paths for the room
	for (CMapPath *path=room->getPathList()->last(); path!=0; path=room->getPathList()->last())
		deletePath(path);

	// Delete any paths connecting with this room
	for (CMapPath *path=room->getConnectingPathList()->last(); path!=0; path = room->getConnectingPathList()->last())
		deletePath(path);

	// delete the room
	CMapLevel *deleteFromLevel = room->getLevel();

	roomLevel->getRoomList()->remove(room);

	manager->getActiveView()->deletedElement(deleteFromLevel);
}

/** Delete a path map element */
void CMapElementUtil::deletePath(CMapPath *path,bool delOpsite)
{
	CMapRoom *srcRoom = path->getSrcRoom();
	CMapRoom *destRoom = path->getDestRoom();
	CMapPath *opsitePath = path->getOpsitePath();

	if (opsitePath)
	{
		if (delOpsite)
		{
			CMapLevel *deleteFromLevel = destRoom->getLevel();
			manager->getActiveView()->deletedElement(deleteFromLevel);
	
			opsitePath->getDestRoom()->getConnectingPathList()->remove(opsitePath);
			opsitePath->getSrcRoom()->getPathList()->remove(opsitePath);
		}
		else
		{
			opsitePath->setOpsitePath(NULL);
		}
	}

	CMapLevel *deleteFromLevel = srcRoom->getLevel();
	destRoom->getConnectingPathList()->remove(path);
	srcRoom->getPathList()->remove(path);

	manager->getActiveView()->deletedElement(deleteFromLevel);
}
