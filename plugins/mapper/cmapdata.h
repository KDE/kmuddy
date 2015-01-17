/***************************************************************************
                               cmapdata.h
                             -------------------
    begin                : Sat Mar 10 2001
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

#ifndef CMAPDATA_H
#define CMAPDATA_H

#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include <QList>

#include "cmapzone.h"
#include "cmaproom.h"

const uint NUM_DIRECTIONS = 10 * 2;

/**This class is store all the map data
  *@author Kmud Developer Team
  */
class CMapData
{
public:
	CMapData();
	~CMapData();

        /** Grab all the zones, recursively */
        QList<CMapZone *> getAllZones(CMapZone *zone = 0);

	/** Used to find the number of a zone */
	signed int getZoneNumber(CMapZone *findZone);
	/** Initialize the directions to their default values. */
	void initDirections(void);

public:	
	/** The default font for text labels */
	QFont defaultTextFont;
	/** The root zone */
	CMapZone *rootZone;
	/** The delay between each speed walk move in centi-seconds */
	int speedwalkDelay;
	/** Used to set the speedwalk abort active/inactive */
	bool speedwalkAbortActive;
	/** Used to store the number of steps before the speedwalk aports */
	int speedwalkAbortLimit;	
	/** Should the mapper check to see if a move was valid */
	bool validRoomCheck;

	// Used to store the color values
	QColor gridColor;
	QColor backgroundColor;
	QColor defaultZoneColor;
	QColor defaultPathColor;
	QColor defaultRoomColor;
	QColor lowerRoomColor;
	QColor lowerPathColor;
	QColor lowerZoneColor;
	QColor lowerTextColor;
	QColor higherRoomColor;
	QColor higherPathColor;
	QColor higherZoneColor;
	QColor higherTextColor;
	QColor defaultTextColor;
	QColor selectedColor;
	QColor specialColor;
	QColor loginColor;
	QColor editColor;
	QColor currentColor;

	// View options
	bool showUpperLevel;
	bool showLowerLevel;

	// Tool options
	bool createModeActive;
	bool gridVisable;

	QSize gridSize;

	bool defaultPathTwoWay;

	QStringList failedMoveMsg;
	
	QString directions[NUM_DIRECTIONS];
};

#endif
