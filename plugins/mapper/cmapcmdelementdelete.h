/***************************************************************************
                               cmapcmdelementdelete.h
                             -------------------
    begin                : Wed Feb 27 2002
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

#ifndef CMAPCMDELEMENTDELETE_H
#define CMAPCMDELEMENTDELETE_H

#include <qstringlist.h>
#include <qfont.h>
#include <qpoint.h>

#include "cmapcommand.h"
#include "cmapelementutil.h"

class CMapElement;
class CMapManager;
class CMapLevel;
class CMapRoom;
class CMapZone;
class CMapText;
class CMapPath;

class CMapCmdElementDelete : public CMapCommand,CMapElementUtil
{
public:
	CMapCmdElementDelete(CMapManager *mapManager,QString name,bool delOpsite = true);
	~CMapCmdElementDelete() override;
	void redo() override;
	void undo() override;

	void addElement(KMemConfig *newElementProperties);
	
private:
	/** The number of elements stored in the command */
	int groups;
	/** A Pointer to the map manager */
	CMapManager *manager;
	/** Contains all the properties need to create the element */
	KMemConfig *properties;
	/** If this is false and element is a path don't delete the opsite path */
	bool m_delOpsite;
};


#endif
