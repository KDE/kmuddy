/***************************************************************************
                               dlgmapcolor.cpp
                             -------------------
    begin                : Tue Jul 9 2002
    copyright            : (C) 2002 by Kmud Developer Team
                           (C) 2007 Tomas Mecir <kmuddy@kmuddy.com>
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

#include "dlgmapcolor.h"

#include <kcolorbutton.h>
#include <kdebug.h>

#include <qcolor.h>

#include "../cmapmanager.h"

DlgMapColor::DlgMapColor (CMapManager *mapManager, QWidget *parent) : QWidget (parent)
{
  setupUi (this);
	m_mapManager = mapManager;
	loadColors();
}

DlgMapColor::~DlgMapColor()
{
}

void DlgMapColor::loadColors()
{
	cmdSelected->setColor(m_mapManager->getMapData()->selectedColor);
	cmdLoginRoom->setColor(m_mapManager->getMapData()->loginColor);
	cmdDefaultText->setColor(m_mapManager->getMapData()->defaultTextColor);
	cmdDefaultPath->setColor(m_mapManager->getMapData()->defaultPathColor);
	cmdDefaultZone->setColor(m_mapManager->getMapData()->defaultZoneColor);
	cmdSpecialExit->setColor(m_mapManager->getMapData()->specialColor);
	cmdCurrentPosition->setColor(m_mapManager->getMapData()->currentColor);
	cmdEditMode->setColor(m_mapManager->getMapData()->editColor);
	cmdDefaultRoom->setColor(m_mapManager->getMapData()->defaultRoomColor);
	cmdLowerRoom->setColor(m_mapManager->getMapData()->lowerRoomColor);
	cmdLowerPath->setColor(m_mapManager->getMapData()->lowerPathColor);
	cmdLowerText->setColor(m_mapManager->getMapData()->lowerTextColor);
	cmdLowerZone->setColor(m_mapManager->getMapData()->lowerZoneColor);
	cmdUpperRoom->setColor(m_mapManager->getMapData()->higherRoomColor);
	cmdUpperPath->setColor(m_mapManager->getMapData()->higherPathColor);
	cmdUpperText->setColor(m_mapManager->getMapData()->higherTextColor);
	cmdUpperZone->setColor(m_mapManager->getMapData()->higherZoneColor);
	cmdBackground->setColor(m_mapManager->getMapData()->backgroundColor);
	cmdGrid->setColor(m_mapManager->getMapData()->gridColor);
}

/** This slot is called when the OK button is pressed */
void DlgMapColor::slotOkPressed()
{
	m_mapManager->getMapData()->selectedColor = cmdSelected->color();
	m_mapManager->getMapData()->loginColor = cmdLoginRoom->color();
	m_mapManager->getMapData()->defaultTextColor = cmdDefaultText->color();
	m_mapManager->getMapData()->defaultPathColor = cmdDefaultPath->color();
	m_mapManager->getMapData()->defaultZoneColor = cmdDefaultZone->color();
	m_mapManager->getMapData()->specialColor = cmdSpecialExit->color();
	m_mapManager->getMapData()->currentColor = cmdCurrentPosition->color();
	m_mapManager->getMapData()->editColor = cmdEditMode->color();
	m_mapManager->getMapData()->defaultRoomColor = cmdDefaultRoom->color();
	m_mapManager->getMapData()->lowerRoomColor = cmdLowerRoom->color();
	m_mapManager->getMapData()->lowerPathColor = cmdLowerPath->color();
	m_mapManager->getMapData()->lowerTextColor = cmdLowerText->color();
	m_mapManager->getMapData()->lowerZoneColor = cmdLowerZone->color();
	m_mapManager->getMapData()->higherRoomColor = cmdUpperRoom->color();
	m_mapManager->getMapData()->higherPathColor = cmdUpperPath->color();
	m_mapManager->getMapData()->higherTextColor = cmdUpperText->color();
	m_mapManager->getMapData()->higherZoneColor = cmdUpperZone->color();
	m_mapManager->getMapData()->backgroundColor = cmdBackground->color();
	m_mapManager->getMapData()->gridColor = cmdGrid->color();
}
