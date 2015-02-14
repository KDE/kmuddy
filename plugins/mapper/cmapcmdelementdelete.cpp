/***************************************************************************
                               cmapcmdelementdelete.cpp
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

#include "cmapcmdelementdelete.h"


#include "cmapelement.h"
#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmaproom.h"
#include "cmappluginbase.h"
#include "cmappath.h"
#include "cmapcmdelementproperties.h"

#include <klocale.h>

CMapCmdElementDelete::CMapCmdElementDelete(CMapManager *mapManager,QString name,bool delOpsite) : CMapCommand(name),CMapElementUtil(mapManager)
{
	manager = mapManager;
	properties = new KMemConfig;
	groups = 0;
	m_delOpsite = delOpsite;
}

CMapCmdElementDelete::~CMapCmdElementDelete()
{
	delete properties;
}

void CMapCmdElementDelete::redo()
{
	QStringList groupList = properties->groupList();

	for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
	{
		if (*it != "<default>")
		{
			CMapElement *element = manager->findElement(properties->group(*it));

			for (CMapPluginBase *plugin = manager->getPluginList()->first(); plugin!=0; plugin = manager->getPluginList()->next())
			{
				plugin->beforeElementDeleted(element);
			}
			deleteElement(properties->group(*it),m_delOpsite);
		}
	}
}

void CMapCmdElementDelete::undo()
{
	bool zone = false;
	QStringList groupList = properties->groupList();

	for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
	{
		if (*it != "<default>")
		{
                        KConfigGroup group = properties->group (*it);

			CMapElement *elm = createElement(group);
			elm->loadProperties(group);

			if (elm->getElementType()==ZONE)
			{
				zone = true;
			}

			if (elm->getElementType()==PATH)
			{
				bool undo = manager->getUndoActive();
				manager->setUndoActive(false);
				CMapPath *path = (CMapPath *)elm;
				if (group.hasKey("PathTwoWay"))
				{
					manager->makePathTwoWay(path);

					path->getOpsitePath()->setAfterCommand(group.readEntry("DestAfterCommand",""));
					path->getOpsitePath()->setBeforeCommand(group.readEntry("DestBeforeCommand",""));
				}
				manager->setUndoActive(undo);
			}

			for (CMapPluginBase *plugin = manager->getPluginList()->first(); plugin!=0; plugin = manager->getPluginList()->next())
			{
				plugin->afterElementUndeleted(elm);
			}
		}
	}
	
	if (zone)
	{
		manager->updateZoneListCombo();
	}

	
}

void CMapCmdElementDelete::addElement(KMemConfig *newElementProperties)
{
	QString grpName ="";
	grpName.sprintf("%d",groups++);
	KConfigGroup grp = properties->group(grpName);

	newElementProperties->group("Properties").copyTo(&grp);
}
