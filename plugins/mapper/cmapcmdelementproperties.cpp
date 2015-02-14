/***************************************************************************
                               cmapcmdelementproperties.cpp
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

#include "cmapcmdelementproperties.h"

#include "cmapelement.h"
#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmaproom.h"

CMapCmdElementProperties::CMapCmdElementProperties(CMapManager *mapManager,QString name,CMapElement *element) : CMapCommand(name)
{
	m_manager = mapManager;
	m_properties = new KMemConfig;
	element->saveProperties(m_properties->group("Element"));
}

CMapCmdElementProperties::~CMapCmdElementProperties()
{
	delete m_properties;
}

void CMapCmdElementProperties::redo()
{
	int active = m_manager->getUndoActive();	
	m_manager->setUndoActive(false);

	CMapElement *elm = m_manager->findElement(m_properties->group("Element"));

	if (elm)
	{
		elm->loadProperties(m_properties->group("New"));
		m_manager->changedElement(elm);
	}
	m_manager->setUndoActive(active);
}

void CMapCmdElementProperties::undo()
{
	int active = m_manager->getUndoActive();	
	m_manager->setUndoActive(false);

	CMapElement *elm = m_manager->findElement(m_properties->group("Element"));

	if (elm)
	{
		elm->loadProperties(m_properties->group("Old"));
		m_manager->changedElement(elm);
	}
	m_manager->setUndoActive(active);
}

KConfigGroup CMapCmdElementProperties::getOrgProperties(void)
{
	return m_properties->group("Old");
}

KConfigGroup CMapCmdElementProperties::getNewProperties(void)
{
	return m_properties->group("New");
}

void CMapCmdElementProperties::setNewProperties(KConfigGroup newProperties)
{
  KConfigGroup grp = m_properties->group("New");
  newProperties.copyTo(&grp);
}

void CMapCmdElementProperties::setOldProperties(KConfigGroup oldProperties)
{
  KConfigGroup grp = m_properties->group("Old");
  oldProperties.copyTo(&grp);
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,QString orgValue,QString dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,bool orgValue,bool dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,int orgValue,int dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,QColor orgValue,QColor dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,QStringList orgValue,QStringList dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,QFont orgValue,QFont dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

/** Used to store check if the values are different and store then if they are */
void CMapCmdElementProperties::compare(QString id,QSize orgValue,QSize dialogValue)
{
	if (orgValue!=dialogValue)
	{
		m_properties->group("Old").writeEntry(id,orgValue);
		m_properties->group("New").writeEntry(id,dialogValue);
	}
}

