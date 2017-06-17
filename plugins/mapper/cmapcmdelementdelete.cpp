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
    if (*it == "<default>") continue;
    CMapElement *element = manager->findElement(properties->group(*it));
    if (!element) properties->deleteGroup(*it);
  }

  for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
  {
    if (*it == "<default>") continue;

    // ignore elements that already got deleted - this is because deletion of multiple rooms can include the same exit twice
    // also mark them as ignored, so that undo doesn't create these twice
    KConfigGroup group = properties->group(*it);
    CMapElement *element = manager->findElement(group);
    if (!element) {
      group.writeEntry("Deleted", "1");
      continue;
    }

    for (CMapPluginBase *plugin : manager->getPluginList())
      plugin->beforeElementDeleted(element);

    deleteElement(properties->group(*it),m_delOpsite);
  }
}

void CMapCmdElementDelete::undo()
{
  QStringList groupList = properties->groupList();

  for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
  {
    if (*it == "<default>") continue;

    KConfigGroup group = properties->group (*it);
    if (group.hasKey("Deleted")) continue;

    CMapElement *elm = createElement(group);
    elm->loadProperties(group);

    for (CMapPluginBase *plugin : manager->getPluginList())
      plugin->afterElementUndeleted(elm);
  }
}

void CMapCmdElementDelete::addElement(KMemConfig *newElementProperties)
{
	QString grpName ="";
	grpName.sprintf("%d",groups++);
	KConfigGroup grp = properties->group(grpName);

	newElementProperties->group("Properties").copyTo(&grp);
}
