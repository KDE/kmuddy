/***************************************************************************
                               cmapfilefilterkconfig.cpp
                             -------------------
    begin                : Wed Aug 7 2002
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

#include "cmapfilefilterkconfig.h"

#include <kmessagebox.h>
#include <klocale.h>

#include <qfile.h>
#include <kvbox.h>

#include "../cmaproom.h"
#include "../cmapzone.h"
#include "../cmappath.h"
#include "../cmaptext.h"
#include "../cmapmanager.h"
#include "../cmaplevel.h"

#include "kcmapfile.h"

CMapFileFilterKConfig::CMapFileFilterKConfig(CMapManager *manager) : CMapFileFilterBase(manager)
{
}

CMapFileFilterKConfig::~CMapFileFilterKConfig()
{
}

/** This returns name of the import/export filter. This should be kept small
  * @return The Name of the filter */
QString CMapFileFilterKConfig::getName(void)
{
	return "Kmud 1 kconfig map files (*.cmap)";
}

/** This returns a discription of the import/export filter
  * @return The discription */
QString CMapFileFilterKConfig::getDescription(void)
{
	return "A import only filter for kconfig kmud 1 and older map files";
}

/** This returns the extension  of the filename that will be loaded,created
  * @return The exstension */
QString CMapFileFilterKConfig::getExtension(void)
{
	return ".cmap";
}

/** This returns the pattern extension of the filename that will be loaded,created
      * @return The exstension */
QString CMapFileFilterKConfig::getPatternExtension(void)
{
	return "*" + getExtension();
}

/** This method will return true or false depending on if it's a export filter
  * @return True if this is a export filter, otherwise false */
bool CMapFileFilterKConfig::supportSave(void)
{
	return false;
}

/** This method will return true or false depending on if it's a import filter
  * @return True if this is a import filter, otherwise false */
bool CMapFileFilterKConfig::supportLoad(void)
{
	return true;
}

/** This method should be reimplemented if this is a to be a export filter. It
  * is called to save the map data
  * @param url The url of the file to be saved
  * @return This method will always return 0 */
int CMapFileFilterKConfig::saveData(QString )
{
	// Not implemented because kmud 2 will only support loading kmud 1 files
	// and not saving them. There would be too much information that could not
	// be stored in the old format.

	return 0;
}

/** This method should be reimplemeted if this is to be a import filter. It is
  * called to load the map data
  * @param url The url of the file to be loaded
  * @return  0 , The file was loaded without problems
  *         -1 , Could not open the file
  *         -2 , If the file is corrupt
  */
int CMapFileFilterKConfig::loadData(QString filename)
{
	int major, minor;
	int gridHeight = m_mapManager->getMapData()->gridSize.width();
	int gridWidth = m_mapManager->getMapData()->gridSize.height();

	if (!QFile::exists(filename)) return -1;

	// Erase the current map
	m_mapManager->eraseMap();

	KCMapFile kcmfile(filename,gridWidth,gridHeight);
	kcmfile.readVersion(major, minor);

	if (major==0 && minor==1)
	{
		// Create a zone because the old map format does not support it
		CMapZone *zone = m_mapManager->createZone(QPoint(-1,-1),NULL);
        
		int num_levels = kcmfile.readCount("levels");
		// fprintf(stderr, "importKCMap: loading 0.1 file with %d levels\n", num_levels);

		while (zone->getLevels()->count()<num_levels)
		{
			// create the levels in the zone
			m_mapManager->createLevel(UP,zone);
 		}
	}
	else
	{
		int num_zones = kcmfile.readCount("zones");

		for (int zone_count=0; zone_count < num_zones; zone_count++)
		{
			CMapZone *zone = NULL;

			kcmfile.setZoneGroup(zone_count+1);
                        KConfigGroup group = kcmfile.currentGroup();
			int parent = group.readEntry("parent zone", -1);

			if (parent==-1)
			{
				zone = m_mapManager->createZone(QPoint(-1,-1),NULL);
			}
			else
			{
				int lev = group.readEntry("level", 0);

				CMapZone *intoZone = m_mapManager->findZone(parent);
    	        CMapLevel *intoLevel = intoZone->getLevels()->at(lev);

				zone = m_mapManager->createZone(QPoint(-30,-30),intoLevel);
				
			}

			zone->setZoneID(zone_count);

			kcmfile.loadZone(zone);

			int num_levels = group.readEntry("levels", 0);
			// Create the levels of the zone
			while (((signed int)zone->getLevels()->count())<num_levels)
			{
				m_mapManager->createLevel(UP,zone);
			}
		}

	}

	// read all rooms
	int num_rooms = kcmfile.readCount("rooms");
	for ( int room_count=0; room_count < num_rooms; room_count++)
	{
		// Read the room
	        kcmfile.setRoomGroup(room_count+1);
                KConfigGroup group = kcmfile.currentGroup();
		CMapZone *zone = m_mapManager->findZone(group.readEntry("zone", 0));
		CMapLevel *level = zone->getLevels()->at(group.readEntry("level", 0));

		CMapRoom *room = m_mapManager->createRoom(QPoint(-30,-30),level);
		kcmfile.loadRoom(room);
	}

	// read the texts
	int num_text = kcmfile.readCount("texts");
	for ( int text_count = 0; text_count < num_text; text_count++)
	{
		kcmfile.setTextGroup(text_count+1);
                KConfigGroup group = kcmfile.currentGroup();
		
		int x = group.readEntry("X", -1);
		int y = group.readEntry("Y", -1);

		CMapZone *zone = m_mapManager->findZone(group.readEntry("zone", 0));
		CMapLevel *level = zone->getLevels()->at(group.readEntry("level", 0));

		QFont font = group.readEntry("font");
		QColor col = group.readEntry("color", m_mapManager->getMapData()->defaultTextColor);
		QString text_str = group.readEntry("text");

		if (!text_str.isEmpty())
		{
			m_mapManager->createText(QPoint (x,y),level,text_str,font,col);
		}

	}
	
	//read the paths
	int num_paths = kcmfile.readCount("paths");
	for (int path_count=0; path_count<num_paths ; path_count++)
	{
		kcmfile.setPathGroup(path_count+1);
                KConfigGroup group = kcmfile.currentGroup();

		// Get the src level of the path
		CMapZone *srcZone = m_mapManager->findZone(group.readEntry("srcZone", 0));
		CMapLevel *srcLevel = srcZone->getLevels()->at(group.readEntry("srcLevel", 0));

		// Get the dest level of the path
		CMapZone *destZone = m_mapManager->findZone(group.readEntry("destZone", 0));
		CMapLevel *destLevel = destZone->getLevels()->at(group.readEntry("destLevel", 0));

		// Create the path
		int srcX = group.readEntry("srcX", -1);
		int srcY = group.readEntry("srcY", -1);
		directionTyp srcDir = (directionTyp)group.readEntry("srcDir", 0);

		int destX = group.readEntry("destX", -1);
		int destY = group.readEntry("destY", -1);
		directionTyp destDir = (directionTyp) group.readEntry("destDir", 0);

		if (((int)srcDir)==10)
		{
			srcDir=SPECIAL;
		}

		if (((int)destDir)==10)
		{
			destDir=SPECIAL;
		}

		CMapPath *path =m_mapManager->createPath(QPoint(srcX * gridWidth,srcY*gridHeight),srcLevel,srcDir,
                                                     QPoint(destX* gridWidth,destY*gridHeight),destLevel,destDir);

		if (path != NULL)
		{
			kcmfile.loadPath(path);
		}
	}

	// read the speedwalk list
	int num_speedwalks = kcmfile.readCount("speedwalks");
 
	for ( int room_count=0; room_count < num_speedwalks; room_count++)
	{
                kcmfile.setSpeedwalkGroup(room_count+1);
                KConfigGroup group = kcmfile.currentGroup();
		CMapZone *zone = m_mapManager->findZone(group.readEntry("zone", 0));
		CMapLevel *level = zone->getLevels()->at(group.readEntry("level", 0));
		int x = group.readEntry("X", -1);
		int y = group.readEntry("Y", -1);

		CMapRoom *room = m_mapManager->findRoomAt(QPoint(x  * gridWidth ,y  * gridHeight),level);
		if (room)
		{
			m_mapManager->addSpeedwalkRoom(room);
		}
	}

	for ( int roomCount=0; roomCount < num_rooms; roomCount++)
	{
		// Read the room
	    kcmfile.setRoomGroup(roomCount+1);
            KConfigGroup group = kcmfile.currentGroup();

		CMapZone *zone = m_mapManager->findZone(group.readEntry("zone", 0));
		CMapLevel *level = zone->getLevels()->at(group.readEntry("level", 0));
		int x = group.readEntry("X", -1);
		int y = group.readEntry("Y", -1);

		// Create the new room
		CMapRoom *room = m_mapManager->findRoomAt(QPoint(x * gridWidth,y * gridHeight),level);

		// Add the rooms commands to the paths
		QString beforeEnterCmd = group.readEntry("beforeEnterCommand", "");
		QString afterEnterCmd = group.readEntry("afterEnterCommand", "");
		QString beforeExitCmd =group.readEntry("beforeExitCommand", "");
		QString afterExitCmd =  group.readEntry("afterExitCommand", "");

		for (CMapPath *path=room->getPathList()->first();path!=0;path=room->getPathList()->next())
		{
			path->setAfterCommand(afterExitCmd);
			path->setBeforeCommand(beforeExitCmd);
		}

		for (CMapPath *path=room->getConnectingPathList()->first();path!=0;path=room->getConnectingPathList()->next())
		{
			path->setAfterCommand(afterEnterCmd);
			path->setBeforeCommand(beforeEnterCmd);
		}
	}

	return 0;
}
