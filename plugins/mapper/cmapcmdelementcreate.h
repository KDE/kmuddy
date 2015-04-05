/***************************************************************************
                               cmapcmdelementcreate.h
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

#ifndef CMAPCMDELEMENTCREATE_H
#define CMAPCMDELEMENTCREATE_H

#include <QList>
#include <qstringlist.h>
#include <qfont.h>
#include <qpoint.h>
#include <qstring.h>

#include "cmapcommand.h"
#include "cmapelementutil.h"

#include "kmemconfig.h"

class CMapElement;
class CMapManager;
class CMapLevel;
class CMapRoom;
class CMapZone;
class CMapText;
class CMapPath;

class CMapCmdElementCreate : public CMapCommand,CMapElementUtil
{
public:
	CMapCmdElementCreate(CMapManager *mapManager,QString name);
	~CMapCmdElementCreate();
	virtual void redo();
	virtual void undo();

	void addElement(KMemConfig *newElementProperties,QString grp = "Properties");

	QList<CMapElement *> *getElements();

private:
	/** The number of elements stored in the command */
	int groups;
	/** A Pointer to the map manager */
	CMapManager *manager;
	/** Contains all the properties need to create the element */
	KMemConfig *properties;
	/** This contains a list of elements that have been create by the execute method */
	QList<CMapElement *> elements;
        bool executed;
};

#endif
