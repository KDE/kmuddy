/***************************************************************************
                               cmapfilefilterkmudone.cpp
                             -------------------
    begin                : Mon May 27 2002
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

#include "cmapfilefilterkmudone.h"

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

const char mapFileVerMajor=1;
const char mapFileVerMinor=10;


CMapFileFilterKmudOne::CMapFileFilterKmudOne(CMapManager *manager) : CMapFileFilterBase(manager)
{
}

CMapFileFilterKmudOne::~CMapFileFilterKmudOne()
{
}

/** This returns name of the import/export filter. This should be kept small
  * @return The Name of the filter */	
QString CMapFileFilterKmudOne::getName(void)
{
	return "Kmud 1 map files (*.map)";
}

/** This returns a discription of the import/export filter
  * @return The discription */
QString CMapFileFilterKmudOne::getDescription(void)
{
	return "A import only filter for kmud 1 and older map files";
}

/** This returns the extension  of the filename that will be loaded,created
  * @return The exstension */
QString CMapFileFilterKmudOne::getExtension(void)
{
	return ".map";
}

/** This returns the pattern extension of the filename that will be loaded,created
      * @return The exstension */
QString CMapFileFilterKmudOne::getPatternExtension(void)
{
	return "*" + getExtension();
}


/** This method will return true or false depending on if it's a export filter
  * @return True if this is a export filter, otherwise false */
bool CMapFileFilterKmudOne::supportSave(void)
{
	return false;
}

/** This method will return true or false depending on if it's a import filter
  * @return True if this is a import filter, otherwise false */
bool CMapFileFilterKmudOne::supportLoad(void)
{
	return true;
}

/** This method should be reimplemented if this is a to be a export filter. It
  * is called to save the map data
  * @param url The url of the file to be saved
  * @return This method will always return 0 */
int CMapFileFilterKmudOne::saveData(QString )
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
int CMapFileFilterKmudOne::loadData(QString filename)
{

	QFile file(filename);

	struct speedwalkRoomTyp in_speedwalk_room;
	struct zoneTyp       	in_zone;
	struct roomHeaderTyp 	in_room_header;
	struct roomTyp       	in_room;
	struct pathHeaderTyp 	in_path_header;
	struct pathTyp       	in_path;
	struct textHeaderTyp 	in_text_header;
	struct textTyp       	in_text;
	struct mainHeaderTyp 	in_main_header;

	int gridHeight = m_mapManager->getMapData()->gridSize.width();
	int gridWidth = m_mapManager->getMapData()->gridSize.height();
	char minor,major;
	int roomPos;
	
	if (!file.open(QIODevice::ReadOnly))
		return -1;

	// Get the version of the map file
	file.read((char *)&minor,sizeof(char));
	file.read((char *)&major,sizeof(char));


	if (minor==mapFileVerMinor && major==mapFileVerMajor)
	{
		// It is the current map version
	}
	else if(major==1 && minor==3)
	{
		KMessageBox::information (NULL,i18n("This version of the file format is no longer supported by kmud"),i18n("Kmud Mapper"));
		return -2;
	}
	else
	{
		KMessageBox::information (NULL,i18n("The file is the incorrect version,\nor not a valid map file."),i18n("Kmud Mapper"));

		return -2;
	}

    // Erase the current map
	m_mapManager->eraseMap();

	// Read the main header
	file.read((char *)&in_main_header, sizeof(in_main_header));

	// Load the zones
	int zoneNumber = 0;
	for ( signed int zoneCount=0 ; zoneCount<in_main_header.numZones; zoneCount++)
	{
		CMapZone *zone;

		// Read the zone details
		file.read((char *)&in_zone,sizeof(in_zone));
		QString zoneName = readStr(&file);

		// Is the zone be loaded the first one
		if (in_zone.zone == -1)
		{
			// Create the root zone
			zone = m_mapManager->createZone(QPoint(-1,-1),NULL);
		}
		else
		{
			CMapZone *intoZone = m_mapManager->findZone(in_zone.zone );
            CMapLevel *intoLevel = intoZone->getLevels()->at(in_zone.level);
			
			zone = m_mapManager->createZone(QPoint(in_zone.x * gridWidth,in_zone.y * gridHeight),intoLevel);
		}

	
		zone->setZoneID(zoneNumber);
		zone->setLabel(zoneName);

		// Create the levels of the zone
		while (((signed int)zone->getLevels()->count())<in_zone.numLevels)
		{
			m_mapManager->createLevel(UP,zone);
		}

		zoneNumber++;
	}

	m_mapManager->m_zoneCount=zoneNumber;
	
	// Read in the room header
	file.read((char *)&in_room_header,sizeof(in_room_header));

	roomPos = file.at();
	// Create the rooms of the map
	for ( int roomCount=0; roomCount < in_room_header.numRooms; roomCount++)
	{
		// Read the room
		file.read((char *)&in_room,sizeof(in_room));

		CMapZone *zone = m_mapManager->findZone(in_room.zone);
		CMapLevel *level = zone->getLevels()->at(in_room.level);

		QString label = readStr(&file);
		QString description = readStr(&file);

		// Create the new room
		CMapRoom *room = m_mapManager->createRoom(QPoint(in_room.x * gridWidth,in_room.y * gridHeight),level);

		// Set room properties
		room->setUseDefaultCol(in_room.useDefaultCol);
		room->setColor(QColor(in_room.red,in_room.green,in_room.blue));
		room->setLabel(label);
		room->setDescription(description);
		
		// Skip past the room commands
		readStr(&file);
		readStr(&file);
		readStr(&file);
		readStr(&file);
	}
	
	
    // Read the paths header
	file.read((char *)&in_path_header,sizeof(in_path_header));

	// Read all the paths
	for (int pathCount=0; pathCount<in_path_header.numPaths ; pathCount++)
	{
		file.read((char *)&in_path,sizeof(in_path));

		if (in_path.srcDir==10)
		{
			in_path.srcDir=SPECIAL;
		}

		if (in_path.destDir==10)
		{
			in_path.destDir=SPECIAL;
		}

		// Get the src level of the path
		CMapZone *srcZone = m_mapManager->findZone(in_path.srcZone);
		CMapLevel *srcLevel = srcZone->getLevels()->at(in_path.srcLevel);

		// Get the dest level of the path
		CMapZone *destZone = m_mapManager->findZone(in_path.destZone);
		CMapLevel *destLevel = destZone->getLevels()->at(in_path.destLevel);

		// Create the path
		CMapPath *path =m_mapManager->createPath(QPoint(in_path.srcX * gridWidth,in_path.srcY*gridHeight),srcLevel,in_path.srcDir,QPoint(in_path.destX* gridWidth,in_path.destY*gridHeight),destLevel,in_path.destDir);
		path->setSpecialExit(in_path.special);
		path->setSpecialCmd(readStr(&file));

		// Read the paths bends, then add them to the path
		for (int i = 0;i<in_path.bendsCount; i++)
		{
			int x = readInt(&file);
			int y = readInt(&file);
			path->addBendWithUndo(QPoint(x,y));
		}

	}

	// Read in the text header
	file.read((char *)&in_text_header,sizeof(in_text_header));

	// Read all the text elements
	for ( int textCount = 0; textCount < in_text_header.numText; textCount++)
	{
		// Read the text elements properties
		file.read((char *)&in_text,sizeof(in_text));
 		QString str = readStr(&file);
 		QString family = readStr(&file);

		// Create the text element with the properties that were read
		QFont font(family,in_text.size,in_text.weight,in_text.italic);
    	QColor color(in_text.red,in_text.green,in_text.blue);

		if (!str.isEmpty())
		{
			CMapZone *zone = m_mapManager->findZone(in_text.zone);
			CMapLevel *level = zone->getLevels()->at(in_text.level);
			m_mapManager->createText(QPoint (in_text.x,in_text.y),level,str,font,color);
		}
	}

	// Read in the speed walk room header
	file.read((char *)&in_room_header,sizeof(in_room_header));

	// Read in the rooms that are in the speedwalk list
	for ( int roomCount=0; roomCount < in_room_header.numRooms; roomCount++)
	{
		// Read the room
		file.read((char *)&in_speedwalk_room,sizeof(in_speedwalk_room));

		CMapZone *zone = m_mapManager->findZone(in_speedwalk_room.zone);
		CMapLevel *level = zone->getLevels()->at(in_speedwalk_room.level);
		
		CMapRoom *room = m_mapManager->findRoomAt(QPoint(in_speedwalk_room.x  * gridWidth ,in_speedwalk_room.y  * gridHeight),level);
		if (room)
		{
			m_mapManager->addSpeedwalkRoom(room);
		}
	}

	// Reprocess the rooms so that we can add the room command detials not that paths
	// have been created
	file.at(roomPos);

    for ( int roomCount=0; roomCount < in_room_header.numRooms; roomCount++)
	{
		// Read the room
		file.read((char *)&in_room,sizeof(in_room));

		CMapZone *zone = m_mapManager->findZone(in_room.zone);
		CMapLevel *level = zone->getLevels()->at(in_room.level);

		// Skip past label and description properties
		readStr(&file);
		readStr(&file);

		// Create the new room
		CMapRoom *room = m_mapManager->findRoomAt(QPoint(in_room.x * gridWidth,in_room.y * gridHeight),level);
		
		// Add the rooms commands to the paths
		QString beforeEnterCmd = readStr(&file);
		QString afterEnterCmd =readStr(&file);
		QString beforeExitCmd =readStr(&file);
		QString afterExitCmd = readStr(&file);

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

	// Close the map, it has now been saved
	file.close();

	// No errors so return 0
	return 0;
}

/**
 * This method is used to read a string from a file
 * @param f The file it is to be read from
 * @return The string that was read
 */
QString CMapFileFilterKmudOne::readStr(QFile *f)
{
	int len;
	char* cstr;

	len = readInt(f);
	if (len <= 0)
		return QString::null;

	len *= sizeof(char);
	cstr = new char[len+1];

	f->read(cstr, len);
	cstr[len]='\0';

	QString str(cstr);
	delete[] cstr;

	return str;
}

/**
 * This method is used to read a int from a file
 * @param f The file that it is to be read from
 * @return The int that was read
 */
int CMapFileFilterKmudOne::readInt(QFile *f)
{
	int i;

	f->read((char *)&i,sizeof(int));

	return i;
}
