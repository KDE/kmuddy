/***************************************************************************
                               cmaplevelutil.h
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

#ifndef CMAPLEVELUTIL_H
#define CMAPLEVELUTIL_H

class CMapLevel;
class CMapZone;
class CMapManager;

/**This class contains util methods for level minupliaction
  *@author Kmud Developer Team
  */

class CMapLevelUtil
{
public: 
	CMapLevelUtil(CMapManager *mapManager);
	~CMapLevelUtil();
	

public:
	CMapLevel *createLevel(unsigned int index,CMapZone *intoZone);
	int deleteLevel(CMapLevel *level);

private:
	CMapManager *m_mapManager;
};

#endif
