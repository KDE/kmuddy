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
//Added by qt3to4:
#include <Q3PtrList>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kvbox.h>

#include "../cmapmanager.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"
#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"

DlgMapZoneProperties::DlgMapZoneProperties(CMapManager *manager,CMapZone *zoneElement,QWidget *parent, const char *name ) : QDialog(parent,name)
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
	for (CMapPluginBase *plugin=mapManager->getPluginList()->first();
	     plugin!=0;
	     plugin=mapManager->getPluginList()->next())
	{
		Q3PtrList<CMapPropertiesPaneBase> paneList = plugin->getPropertyPanes(ZONE,(CMapElement*)zoneElement,(QWidget *)zoneTabs);
		for (CMapPropertiesPaneBase *pane = paneList.first();pane!=0;pane = paneList.next())
		{
			zoneTabs->addTab(pane,pane->getTitle());
			connect(cmdOK,SIGNAL(clicked()),pane,SLOT(slotOk()));
			connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
		}
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
	command->compare("DefaultBackground",zone->getUseDefaultBackground(),optDefaultBackground->isOn());

	mapManager->addCommand(command);
}

void DlgMapZoneProperties::setLabelPos(CMapZone::labelPosTyp position)
{
	cmdN->setOn(false);
	cmdNE->setOn(false);
	cmdE->setOn(false);
	cmdSE->setOn(false);
	cmdS->setOn(false);
	cmdSW->setOn(false);
	cmdW->setOn(false);
	cmdNW->setOn(false);
	cmdHide->setOn(false);
	cmdCustom->setOn(false);

	switch (position)
	{
		case CMapZone::NORTH     : cmdN->setOn(true); break;
		case CMapZone::NORTHEAST : cmdNE->setOn(true); break;
		case CMapZone::EAST      : cmdE->setOn(true); break;
		case CMapZone::SOUTHEAST : cmdSE->setOn(true); break;
		case CMapZone::SOUTH     : cmdS->setOn(true); break;
		case CMapZone::SOUTHWEST : cmdSW->setOn(true); break;
		case CMapZone::WEST      : cmdW->setOn(true); break;
		case CMapZone::NORTHWEST : cmdNW->setOn(true); break;
		case CMapZone::HIDE      : cmdHide->setOn(true); break;
		case CMapZone::CUSTOM    : cmdCustom->setOn(true); break;
	}
}

CMapZone::labelPosTyp DlgMapZoneProperties::getLabelPos(void)
{
	if (cmdN->isOn())
		return CMapZone::NORTH;

	if (cmdE->isOn())
		return CMapZone::EAST;

	if (cmdS->isOn())
		return CMapZone::SOUTH;

	if (cmdW->isOn())
		return CMapZone::WEST;

	if (cmdNE->isOn())
		return CMapZone::NORTHEAST;

	if (cmdSE->isOn())
		return CMapZone::SOUTHEAST;

	if (cmdSW->isOn())
		return CMapZone::SOUTHWEST;

	if (cmdNW->isOn())
		return CMapZone::NORTHWEST;

	if (cmdHide->isOn())
		return CMapZone::HIDE;

	if (cmdCustom->isOn())
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
