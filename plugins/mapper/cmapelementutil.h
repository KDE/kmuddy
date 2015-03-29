/***************************************************************************
                               cmapelementutil.h
                             -------------------
    begin                : Thu Jan 10 2002
    copyright            : (C) 2002 by Kmud Developer Team
                           (C) 2014-2015 by Tomas Mecir <mecirt@gmail.com>
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

#include "cmapelement.h"

class CMapManager;
class CMapLevel;
class CMapRoom;
class CMapText;
class CMapZone;
class CMapPath;

#include "kmemconfig.h"
#include <kconfiggroup.h>

/** Classes that inherit this calls will be able to create map elements directly onto the map
  * These functions do not alter the undo history.
  */

class CMapElementUtil
{
public: 
  CMapElementUtil(CMapManager *mapManager);
  ~CMapElementUtil();

public:
  /** Used to delete a element specifyed in a list of properties */
  void deleteElement(KConfigGroup grp,bool delOpsite = true);
  /** Used to create a new element from a list of properties */
  CMapElement *createElement(KConfigGroup grp);

// static functions - note that these are low-level and do not modify the undo history

  /** Used to create a new room */
  static CMapRoom *createRoom(CMapManager *manager, QPoint pos, CMapLevel *level);
  static CMapText *createText(CMapManager *manager, QPoint pos, CMapLevel *level, QString str);
  static CMapText *createText(CMapManager *manager, QPoint pos, CMapLevel *level, QString str, QFont f, QColor col);

  /** Delete a path map element */
  static void deletePath(CMapPath *path, bool delOpsite = true);

private:
  CMapManager *manager;
};

#endif
