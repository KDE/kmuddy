/***************************************************************************
                               cmapelementutil.h
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

#ifndef CMAPELEMENTUTIL_H
#define CMAPELEMENTUTIL_H

#include <qpoint.h>

#include "cmaplevelutil.h"
#include "cmapelement.h"

class CMapManager;
class CMapLevel;
class CMapRoom;
class CMapText;
class CMapZone;
class CMapPath;

#include "kmemconfig.h"
#include <kconfiggroup.h>

/** Classes that inherit this calls will be able to create map elements
  * directly onto the map
  * @author Kmud Developer Team
  */

class CMapElementUtil : public CMapLevelUtil
{
public: 
	CMapElementUtil(CMapManager *mapManager);
	~CMapElementUtil();

public:
	/** Used to delete a element specifyed in a list of properties */
	void deleteElement(KConfigGroup grp,bool delOpsite = true);
	/** Used to create a new element from a list of properties */
	CMapElement *createElement(KConfigGroup grp);
	/** Used to create a new room */
	CMapRoom *createRoom(QPoint pos,CMapLevel *level);
	/** Used to create a new zone */
	CMapZone *createZone(QPoint pos,CMapLevel *level);
	/** Used to create a new text label */
	CMapText *createText(QPoint pos,CMapLevel *level,QString str);
	/** Used to create a new text label */
	CMapText *createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col);
	/** This is used to create a path between two rooms */
	CMapPath *createPath (CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir);
	/** Delete a path map element */
	void deletePath(CMapPath *path,bool delOpsite = true);

private:
	CMapManager *manager;
};

#endif
