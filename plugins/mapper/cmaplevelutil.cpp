/***************************************************************************
                               cmaplevelutil.cpp
                             -------------------
    begin                : Mon Mar 4 2002
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

#include "cmaplevelutil.h"

#include "cmaplevel.h"
#include "cmapzone.h"
#include "cmapmanager.h"
#include "cmapviewbase.h"
//Added by qt3to4:
#include <Q3PtrList>

CMapLevelUtil::CMapLevelUtil(CMapManager *mapManager)
{
	m_mapManager = mapManager;
}

CMapLevelUtil::~CMapLevelUtil()
{
}

CMapLevel *CMapLevelUtil::createLevel(unsigned int index,CMapZone *intoZone)
{
	if (intoZone == NULL) return NULL;

	CMapLevel *nextLevel = NULL;
	CMapLevel *prevLevel = NULL;

	Q3PtrList<CMapLevel> *levels = intoZone->getLevels();

	if (levels->count()-1>=index && levels->count()>0)
	{
		nextLevel = levels->at(index);
	}

	if (index>0)
	{
		prevLevel = levels->at(index -1);
	}

	CMapLevel *newLevel = new CMapLevel(m_mapManager);
	newLevel->setZone(intoZone);

	if (nextLevel)
	{
		newLevel->setNextLevel(nextLevel);
		nextLevel->setPrevLevel(newLevel);
	}

	if (prevLevel)
	{
		newLevel->setPrevLevel(prevLevel);
		prevLevel->setNextLevel(newLevel);	
	}

	intoZone->getLevels()->insert(index,newLevel);

	return newLevel;
}
