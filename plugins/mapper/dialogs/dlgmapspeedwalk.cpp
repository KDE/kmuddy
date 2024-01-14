/***************************************************************************
                               dlgmapspeedwalk.cpp
                             -------------------
    begin                : Tue Jul 9 2002
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

#include "dlgmapspeedwalk.h"

#include "../cmapmanager.h"

#include <QCheckBox>

DlgMapSpeedwalk::DlgMapSpeedwalk(CMapManager *mapManager, QWidget *parent) : QWidget (parent)
{
	m_mapManager = mapManager;

        setupUi (this);

	chkLimitSpeedwalk->setChecked(m_mapManager->getMapData()->speedwalkAbortActive);
	txtAbortCount->setValue(m_mapManager->getMapData()->speedwalkAbortLimit);
	txtDelay->setValue(m_mapManager->getMapData()->speedwalkDelay);
}

DlgMapSpeedwalk::~DlgMapSpeedwalk()
{
}

/** This slot is called when the OK button is pressed */
void DlgMapSpeedwalk::slotOkPressed()
{
	m_mapManager->getMapData()->speedwalkAbortActive = chkLimitSpeedwalk->isChecked();
	m_mapManager->getMapData()->speedwalkAbortLimit = txtAbortCount->value();
	m_mapManager->getMapData()->speedwalkDelay = txtDelay->value();
}

#include "moc_dlgmapspeedwalk.cpp"
