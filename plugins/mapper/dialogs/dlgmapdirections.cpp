/***************************************************************************
                               dlgmapdirections.cpp
                             -------------------
    begin                : Tue Nov 19 2002
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

#include "dlgmapdirections.h"

#include <qlineedit.h>

#include "../cmapmanager.h"


DlgMapDirections::DlgMapDirections(const CMapManager *mapManager, QWidget *parent) : QDialog(parent)
{
  setupUi (this);
  connect(this, &DlgMapDirections::accepted, this, &DlgMapDirections::slotOkPressed);

	m_mapManager = mapManager;

	m_txtNorthLong->setText(m_mapManager->getMapData()->directions[NORTH]);
	m_txtNorthEastLong->setText(m_mapManager->getMapData()->directions[NORTHEAST]);
	m_txtEastLong->setText(m_mapManager->getMapData()->directions[EAST]);
	m_txtSouthEastLong->setText(m_mapManager->getMapData()->directions[SOUTHEAST]);
	m_txtSouthLong->setText(m_mapManager->getMapData()->directions[SOUTH]);
	m_txtSouthWestLong->setText(m_mapManager->getMapData()->directions[SOUTHWEST]);
	m_txtWestLong->setText(m_mapManager->getMapData()->directions[WEST]);
	m_txtNorthWestLong->setText(m_mapManager->getMapData()->directions[NORTHWEST]);
	m_txtUpLong->setText(m_mapManager->getMapData()->directions[UP]);
	m_txtDownLong->setText(m_mapManager->getMapData()->directions[DOWN]);

	m_txtNorthShort->setText(m_mapManager->getMapData()->directions[NORTH+(NUM_DIRECTIONS/2)]);
	m_txtNorthEastShort->setText(m_mapManager->getMapData()->directions[NORTHEAST+(NUM_DIRECTIONS/2)]);
	m_txtEastShort->setText(m_mapManager->getMapData()->directions[EAST+(NUM_DIRECTIONS/2)]);
	m_txtSouthEastShort->setText(m_mapManager->getMapData()->directions[SOUTHEAST+(NUM_DIRECTIONS/2)]);
	m_txtSouthShort->setText(m_mapManager->getMapData()->directions[SOUTH+(NUM_DIRECTIONS/2)]);
	m_txtSouthWestShort->setText(m_mapManager->getMapData()->directions[SOUTHWEST+(NUM_DIRECTIONS/2)]);
	m_txtWestShort->setText(m_mapManager->getMapData()->directions[WEST+(NUM_DIRECTIONS/2)]);
	m_txtNorthWestShort->setText(m_mapManager->getMapData()->directions[NORTHWEST+(NUM_DIRECTIONS/2)]);
	m_txtUpShort->setText(m_mapManager->getMapData()->directions[UP+(NUM_DIRECTIONS/2)]);
	m_txtDownShort->setText(m_mapManager->getMapData()->directions[DOWN+(NUM_DIRECTIONS/2)]);

}

DlgMapDirections::~DlgMapDirections()
{
}

/** This slot is called when the OK button is pressed */
void DlgMapDirections::slotOkPressed()
{
	m_mapManager->getMapData()->directions[NORTH] = m_txtNorthLong->text();
	m_mapManager->getMapData()->directions[NORTHEAST] = m_txtNorthEastLong->text();
	m_mapManager->getMapData()->directions[EAST] = m_txtEastLong->text();
	m_mapManager->getMapData()->directions[SOUTHEAST] = m_txtSouthEastLong->text();
	m_mapManager->getMapData()->directions[SOUTH] = m_txtSouthLong->text();
	m_mapManager->getMapData()->directions[SOUTHWEST] = m_txtSouthWestLong->text();
	m_mapManager->getMapData()->directions[WEST] = m_txtWestLong->text();
	m_mapManager->getMapData()->directions[NORTHWEST] = m_txtNorthWestLong->text();
	m_mapManager->getMapData()->directions[UP] = m_txtUpLong->text();
	m_mapManager->getMapData()->directions[DOWN] = m_txtDownLong->text();

	m_mapManager->getMapData()->directions[NORTH+(NUM_DIRECTIONS/2)] = m_txtNorthShort->text();
	m_mapManager->getMapData()->directions[NORTHEAST+(NUM_DIRECTIONS/2)] = m_txtNorthEastShort->text();
	m_mapManager->getMapData()->directions[EAST+(NUM_DIRECTIONS/2)] = m_txtEastShort->text();
	m_mapManager->getMapData()->directions[SOUTHEAST+(NUM_DIRECTIONS/2)] = m_txtSouthEastShort->text();
	m_mapManager->getMapData()->directions[SOUTH+(NUM_DIRECTIONS/2)] = m_txtSouthShort->text();
	m_mapManager->getMapData()->directions[SOUTHWEST+(NUM_DIRECTIONS/2)] = m_txtSouthWestShort->text();
	m_mapManager->getMapData()->directions[WEST+(NUM_DIRECTIONS/2)] = m_txtWestShort->text();
	m_mapManager->getMapData()->directions[NORTHWEST+(NUM_DIRECTIONS/2)] = m_txtNorthWestShort->text();
	m_mapManager->getMapData()->directions[UP+(NUM_DIRECTIONS/2)] = m_txtUpShort->text();
	m_mapManager->getMapData()->directions[DOWN+(NUM_DIRECTIONS/2)] = m_txtDownShort->text();
}

#include "moc_dlgmapdirections.cpp"
