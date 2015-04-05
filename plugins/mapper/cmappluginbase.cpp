/***************************************************************************
                               cmappluginbase.cpp
                             -------------------
    begin                : Mon Aug 6 2001
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

#include "cmappluginbase.h"

#include "cmappropertiespanebase.h"
#include "cmaptoolbase.h"
#include "cmapmanager.h"

CMapPluginBase::CMapPluginBase(QObject *parent) : KParts::Plugin(parent)
{
	mapManager = dynamic_cast<CMapManager *>(parent);
}

CMapPluginBase::~CMapPluginBase()
{
}

/** Used to get a list of the tools */
QList<CMapToolBase *> *CMapPluginBase::getToolList(void)
{
	return &toolList;
}

/** Used to get a list of the property pages for a map element */
QList<CMapPropertiesPaneBase *> CMapPluginBase::createPropertyPanes(elementTyp ,CMapElement *,QWidget *)
{
  return QList<CMapPropertiesPaneBase *>();
}

/** Used to tell the tool were to find the map manager */
void CMapPluginBase::setManager(CMapManager *manager)
{
	mapManager = manager;
}

/** Used to get the map manager */
CMapManager *CMapPluginBase::getManager(void)
{
	return mapManager;
}

/** This is called before a element menu is openend */
void CMapPluginBase::beforeOpenElementMenu(CMapElement *)
{
}

/** This is called before a element is deleted */
void CMapPluginBase::beforeElementDeleted(CMapElement *)
{
}

/** This is called when a element is changed */
void CMapPluginBase::elementChanged(CMapElement *)
{
}

void CMapPluginBase::saveConfigOptions(void)
{
}

void CMapPluginBase::loadConfigOptions(void)
{
}

void CMapPluginBase::mapErased(void)
{
}

/** This method is called after undoing a delete action
  * @param element The elemening being restored */
void CMapPluginBase::afterElementUndeleted(CMapElement *)
{	
}


void CMapPluginBase::createGlobalConfigPanes()
{
}

void CMapPluginBase::createProfileConfigPanes()
{
}

