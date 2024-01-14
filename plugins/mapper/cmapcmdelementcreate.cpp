/***************************************************************************
                               cmapcmdelementcreate.cpp
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

#include "cmapcmdelementcreate.h"


#include "cmapelement.h"
#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmaproom.h"
#include "cmappath.h"

#include "cmapcmdelementproperties.h"

#include <KLocalizedString>
#include <QDebug>

CMapCmdElementCreate::CMapCmdElementCreate(CMapManager *mapManager,QString name) : CMapCommand(name),CMapElementUtil(mapManager)
{
	manager = mapManager;
	properties = new KMemConfig();
	groups = 0;
        executed = false;
}

CMapCmdElementCreate::~CMapCmdElementCreate()
{
	delete properties;
}

QList<CMapElement *> *CMapCmdElementCreate::getElements()
{
  if (!executed) qWarning() << "CMapCmdElementCreate::getElements called without actually executing the command, this will not work!";

  return &elements;
}

void CMapCmdElementCreate::redo()
{
	QStringList groupList = properties->groupList();
	elements.clear();

	for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
	{
		if (*it != "<default>")
		{
			CMapElement *element = createElement(properties->group (*it));
			if (element) elements.append(element);
		}
	}
        executed = true;
}

void CMapCmdElementCreate::undo()
{
	QStringList groupList = properties->groupList();

	for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
	{
		if (*it != "<default>")
		{
			deleteElement(properties->group(*it));
		}
	}	

	elements.clear();
        executed = false;
}

void CMapCmdElementCreate::addElement(KMemConfig *newElementProperties,QString grp)
{
  KConfigGroup group = properties->group(QString::number(groups++));
  newElementProperties->group(grp).copyTo(&group);
}

