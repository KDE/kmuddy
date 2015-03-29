/***************************************************************************
                               cmapdata.cpp
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

#include "cmapdata.h"

#include <kglobalsettings.h>

#include "cmaplevel.h"

CMapData::CMapData()
{
	gridSize.setWidth(20);
	gridSize.setHeight(20);

	// FIXME_jp: This needs loading/saving
	defaultPathTwoWay = true;
	showUpperLevel = true;
	showLowerLevel = true;
	createModeActive = true;
	gridVisable = true;

	speedwalkAbortActive = false;
	speedwalkAbortLimit = 100;
	speedwalkDelay = 5;

	validRoomCheck = false;

	defaultTextColor = Qt::black;
	defaultTextFont = KGlobalSettings::generalFont();

	failedMoveMsg.clear();

	rootZone = NULL;
	
	//initDirections();
}

CMapData::~CMapData()
{
}

void CMapData::initDirections(void)
{
	directions[NORTH]     = "north";
	directions[SOUTH]     = "south";
	directions[WEST]      = "west";
	directions[EAST]      = "east";
	directions[NORTHWEST] = "northwest";
	directions[NORTHEAST] = "northeast";
	directions[SOUTHWEST] = "southwest";
	directions[SOUTHEAST] = "southeast";
	directions[UP]        = "up";
	directions[DOWN]      = "down";

	directions[NORTH+(NUM_DIRECTIONS/2)]     = "n";
	directions[SOUTH+(NUM_DIRECTIONS/2)]     = "s";
	directions[WEST+(NUM_DIRECTIONS/2)]      = "w";
	directions[EAST+(NUM_DIRECTIONS/2)]      = "e";
	directions[NORTHWEST+(NUM_DIRECTIONS/2)] = "nw";
	directions[NORTHEAST+(NUM_DIRECTIONS/2)] = "ne";
	directions[SOUTHWEST+(NUM_DIRECTIONS/2)] = "sw";
	directions[SOUTHEAST+(NUM_DIRECTIONS/2)] = "se";
	directions[UP+(NUM_DIRECTIONS/2)]        = "u";
	directions[DOWN+(NUM_DIRECTIONS/2)]      = "d";

}


