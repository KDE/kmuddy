/***************************************************************************
                               dlgmapzoneproperties.cpp
                             -------------------
    begin                : Mon May 7 2001
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

#include "dlgmapzoneproperties.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <q3multilineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>

#include "../cmapmanager.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"
#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"

DlgMapZoneProperties::DlgMapZoneProperties(CMapManager *manager,CMapZone *zoneElement,QWidget *parent ) : QDialog(parent)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotAccept()));

	mapManager = manager;
	zone = zoneElement;

	txtLabel->setText(zone->getLabel());
	txtDescription->setText(zone->getDescription());
	cmdRoomColor->setColor(zone->getColor());
	slotUseDefaultColor(zone->getUseDefaultCol());
	setLabelPos(zone->getLabelPosition());	

	cmdBackgroundColor->setColor(zone->getBackgroundColor());

	optDefaultBackground->setChecked(zone->getUseDefaultBackground());
	optCustomBackground->setChecked(!zone->getUseDefaultBackground());

	slotBlackgroundOptChange();

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(ZONE,(CMapElement*)zoneElement,(QWidget *)zoneTabs);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		zoneTabs->addTab(pane,pane->getTitle());
		connect(cmdOK,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}

}

DlgMapZoneProperties::~DlgMapZoneProperties()
{
}

void DlgMapZoneProperties::slotAccept()
{
	CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Changed Zone Properties"),zone);

	command->compare("Label",zone->getLabel(),txtLabel->text().trimmed());
	command->compare("Description",zone->getDescription(),txtDescription->text().trimmed());
	command->compare("Color",zone->getColor(),cmdRoomColor->color());
	command->compare("DefaultColor",zone->getUseDefaultCol(),chkUseDefaltColor->isChecked());
	command->compare("LabelPos",(int)zone->getLabelPosition(),(int)getLabelPos());
	command->compare("BackgroundColor",zone->getBackgroundColor(),cmdBackgroundColor->color());
	command->compare("DefaultBackground",zone->getUseDefaultBackground(),optDefaultBackground->isChecked());

	mapManager->addCommand(command);
}

void DlgMapZoneProperties::setLabelPos(CMapZone::labelPosTyp position)
{
	cmdN->setChecked(false);
	cmdNE->setChecked(false);
	cmdE->setChecked(false);
	cmdSE->setChecked(false);
	cmdS->setChecked(false);
	cmdSW->setChecked(false);
	cmdW->setChecked(false);
	cmdNW->setChecked(false);
	cmdHide->setChecked(false);
	cmdCustom->setChecked(false);

	switch (position)
	{
		case CMapZone::NORTH     : cmdN->setChecked(true); break;
		case CMapZone::NORTHEAST : cmdNE->setChecked(true); break;
		case CMapZone::EAST      : cmdE->setChecked(true); break;
		case CMapZone::SOUTHEAST : cmdSE->setChecked(true); break;
		case CMapZone::SOUTH     : cmdS->setChecked(true); break;
		case CMapZone::SOUTHWEST : cmdSW->setChecked(true); break;
		case CMapZone::WEST      : cmdW->setChecked(true); break;
		case CMapZone::NORTHWEST : cmdNW->setChecked(true); break;
		case CMapZone::HIDE      : cmdHide->setChecked(true); break;
		case CMapZone::CUSTOM    : cmdCustom->setChecked(true); break;
	}
}

CMapZone::labelPosTyp DlgMapZoneProperties::getLabelPos(void)
{
	if (cmdN->isChecked())
		return CMapZone::NORTH;

	if (cmdE->isChecked())
		return CMapZone::EAST;

	if (cmdS->isChecked())
		return CMapZone::SOUTH;

	if (cmdW->isChecked())
		return CMapZone::WEST;

	if (cmdNE->isChecked())
		return CMapZone::NORTHEAST;

	if (cmdSE->isChecked())
		return CMapZone::SOUTHEAST;

	if (cmdSW->isChecked())
		return CMapZone::SOUTHWEST;

	if (cmdNW->isChecked())
		return CMapZone::NORTHWEST;

	if (cmdHide->isChecked())
		return CMapZone::HIDE;

	if (cmdCustom->isChecked())
		return CMapZone::CUSTOM;

	return CMapZone::HIDE;
}

void DlgMapZoneProperties::slotE()
{
	setLabelPos(CMapZone::EAST);
}

void DlgMapZoneProperties::slotHide()
{
	setLabelPos(CMapZone::HIDE);
}

void DlgMapZoneProperties::slotN()
{
	setLabelPos(CMapZone::NORTH);
}

void DlgMapZoneProperties::slotNE()
{
	setLabelPos(CMapZone::NORTHEAST);
}

void DlgMapZoneProperties::slotNW()
{
	setLabelPos(CMapZone::NORTHWEST);
}

void DlgMapZoneProperties::slotS()
{
	setLabelPos(CMapZone::SOUTH);
}

void DlgMapZoneProperties::slotSE()
{
	setLabelPos(CMapZone::SOUTHEAST);
}

void DlgMapZoneProperties::slotSW()
{
	setLabelPos(CMapZone::SOUTHWEST);
}

void DlgMapZoneProperties::slotW()
{
	setLabelPos(CMapZone::WEST);
}

void DlgMapZoneProperties::slotCustom()
{
	setLabelPos(CMapZone::CUSTOM);
}

void DlgMapZoneProperties::slotUseDefaultColor(bool useDefaultColor)
{
	chkUseDefaltColor->setChecked(useDefaultColor);

	cmdRoomColor->setEnabled(!useDefaultColor);
	lblRoomColor->setEnabled(!useDefaultColor);
}

void DlgMapZoneProperties::slotBlackgroundOptChange()
{
	bool enabled = !optDefaultBackground->isChecked();

	cmdSelectImage->setEnabled(enabled);
	cmdBackgroundColor->setEnabled(enabled);
	lblBackgroundColor->setEnabled(enabled);
	lblImage->setEnabled(enabled);
	txtImage->setEnabled(enabled);
}
