/***************************************************************************
                          dlgmaproomproperties.cpp  -  description
                             -------------------
    begin                : Thu Mar 8 2001
    copyright            : (C) 2001 by KMud Development Team
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

#include "dlgmaproomproperties.h"

#include "../cmaproom.h"
#include "../cmappath.h"
#include "../cmapmanager.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"
#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"

#include <qlineedit.h>
#include <q3multilineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <q3listview.h>
#include <qtabwidget.h>

#include <kcolorbutton.h>
#include <klocale.h>
#include <kdebug.h>

DlgMapRoomProperties::DlgMapRoomProperties(CMapManager *manager,CMapRoom *roomElement,QWidget *parent )
	: QDialog(parent)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotAccept()));

	room = roomElement;
	mapManager = manager;

	// Populate the dialog
	txtLabel->setText(room->getLabel());
	txtDescription->setText(room->getDescription());
	cmdRoomColor->setColor(room->getColor());
	slotUseDefaultColor(room->getUseDefaultCol());
	setLabelPos(room->getLabelPosition());

	for ( QStringList::Iterator it = room->getContentsList()->begin(); it != room->getContentsList()->end(); ++it )
	{
		(void) new Q3ListViewItem(lstContents,*it);
	}

	Q3ListViewItem *blankItem= new Q3ListViewItem(lstContents,"");
	lstContents->setSelected(blankItem,true);

	regenerateExits();

	lstContents->setColumnWidthMode(0,Q3ListView::Maximum);

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(ROOM,(CMapElement*)roomElement,(QWidget *)RoomsTab);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		RoomsTab->addTab(pane,pane->getTitle());
		connect(cmdOk,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}
}

DlgMapRoomProperties::~DlgMapRoomProperties()
{
}

void DlgMapRoomProperties::regenerateExits(void)
{
	Q3ListViewItem *exit =NULL;
	lstPaths->clear();
	foreach (CMapPath *path, *room->getPathList())
	{

		QString direction;

		direction = mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd());
		kDebug() << "Path : " << path->getSrcDir() << "," << path->getSpecialCmd() << "," << direction;	
		
		exit = new Q3ListViewItem(lstPaths,direction,path->getBeforeCommand(),path->getAfterCommand());
	}

	lstPaths->setSelected(exit,true);
}

/** Used to get the path that a item responds to */
CMapPath *DlgMapRoomProperties::itemToPath(Q3ListViewItem *item)
{
	if (item)
	{
		QString textDir = item->text(0);
		
		directionTyp direction = mapManager->textToDirection(textDir);

		if (direction!=SPECIAL)
			textDir = "";

		return room->getPathDirection(direction,textDir);
	}
	
	return NULL;
}

void DlgMapRoomProperties::slotAccept()
{
	CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Changed Room Properties"),room);

	command->compare("Label",room->getLabel(),txtLabel->text().trimmed());
	command->compare("Description",room->getDescription(),txtDescription->text().trimmed());
	command->compare("Color",room->getColor(),cmdRoomColor->color());
	command->compare("DefaultColor",room->getUseDefaultCol(),chkUseDefaltColor->isChecked());
	command->compare("LabelPos",(int)room->getLabelPosition(),(int)getLabelPos());

	QStringList newContents;

	Q3ListViewItemIterator it( lstContents );
    for ( ; it.current(); ++it )
	{
		QString item = it.current()->text(0).trimmed();
	
		if (item!="")
			newContents+=item;
	}

	command->compare("Contents",*room->getContentsList(),newContents);

	foreach (CMapPath *path, *room->getPathList())
	{
		QString name = mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd());

		bool found = false;

		Q3ListViewItemIterator exits (lstPaths);
		for (; exits.current(); ++exits)
		{
			if (exits.current()->text(0)==name)
			{	
				found = true;
			}
		}

		if (!found)
			mapManager->deleteElement(path);
	}

	mapManager->addCommand(command);
}

void DlgMapRoomProperties::slotUseDefaultColor(bool useDefaultColor)
{
	chkUseDefaltColor->setChecked(useDefaultColor);
	
	cmdRoomColor->setEnabled(!useDefaultColor);
	lblRoomColor->setEnabled(!useDefaultColor);
}

void DlgMapRoomProperties::setLabelPos(CMapRoom::labelPosTyp position)
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
		case CMapRoom::NORTH     : cmdN->setChecked(true); break;
		case CMapRoom::NORTHEAST : cmdNE->setChecked(true); break;
		case CMapRoom::EAST      : cmdE->setChecked(true); break;
		case CMapRoom::SOUTHEAST : cmdSE->setChecked(true); break;
		case CMapRoom::SOUTH     : cmdS->setChecked(true); break;
		case CMapRoom::SOUTHWEST : cmdSW->setChecked(true); break;
		case CMapRoom::WEST      : cmdW->setChecked(true); break;
		case CMapRoom::NORTHWEST : cmdNW->setChecked(true); break;
		case CMapRoom::HIDE      : cmdHide->setChecked(true); break;
		case CMapRoom::CUSTOM    : cmdCustom->setChecked(true); break;
	}
}

CMapRoom::labelPosTyp DlgMapRoomProperties::getLabelPos(void)
{
	if (cmdN->isChecked())
		return CMapRoom::NORTH;

	if (cmdE->isChecked())
		return CMapRoom::EAST;

	if (cmdS->isChecked())
		return CMapRoom::SOUTH;

	if (cmdW->isChecked())
		return CMapRoom::WEST;

	if (cmdNE->isChecked())
		return CMapRoom::NORTHEAST;

	if (cmdSE->isChecked())
		return CMapRoom::SOUTHEAST;

	if (cmdSW->isChecked())
		return CMapRoom::SOUTHWEST;

	if (cmdNW->isChecked())
		return CMapRoom::NORTHWEST;

	if (cmdHide->isChecked())
		return CMapRoom::HIDE;

	if (cmdCustom->isChecked())
		return CMapRoom::CUSTOM;

	return CMapRoom::HIDE;
}

void DlgMapRoomProperties::slotE()
{
	setLabelPos(CMapRoom::EAST);
}

void DlgMapRoomProperties::slotHide()
{
	setLabelPos(CMapRoom::HIDE);
}

void DlgMapRoomProperties::slotN()
{
	setLabelPos(CMapRoom::NORTH);
}

void DlgMapRoomProperties::slotNE()
{
	setLabelPos(CMapRoom::NORTHEAST);
}

void DlgMapRoomProperties::slotNW()
{
	setLabelPos(CMapRoom::NORTHWEST);
}

void DlgMapRoomProperties::slotS()
{
	setLabelPos(CMapRoom::SOUTH);
}

void DlgMapRoomProperties::slotSE()
{
	setLabelPos(CMapRoom::SOUTHEAST);
}

void DlgMapRoomProperties::slotSW()
{
	setLabelPos(CMapRoom::SOUTHWEST);
}

void DlgMapRoomProperties::slotW()
{
	setLabelPos(CMapRoom::WEST);
}

void DlgMapRoomProperties::slotCustom()
{
	setLabelPos(CMapRoom::CUSTOM);
}

void DlgMapRoomProperties::slotRemoveItem()
{
	Q3ListViewItem *item = lstContents->selectedItem();

	if (item)
		delete item;
}

void DlgMapRoomProperties::slotAddItem()
{	
	Q3ListViewItem *firstItem = lstContents->firstChild();
	if (firstItem)
	{
		QString name = firstItem->text(0).trimmed();

		if (name!="")
		{
			Q3ListViewItem *blankItem = new Q3ListViewItem(lstContents,"");
			lstContents->setSelected(blankItem,true);
		}
	}
}

void DlgMapRoomProperties::slotNewItemSelected(Q3ListViewItem *item)
{
	txtItemName->setText(item->text(0).trimmed());
}

void DlgMapRoomProperties::slotEditItemName(const QString & name)
{
	Q3ListViewItem *item = lstContents->selectedItem();

	if (item)
	{
		item->setText(0,name);
	}
}

void DlgMapRoomProperties::slotPathDelete()
{
	Q3ListViewItem *item = lstPaths->selectedItem();

	if (item)
		delete item;
}

void DlgMapRoomProperties::slotPathProperties()
{
	Q3ListViewItem *item = lstPaths->selectedItem();
        if (!item) return;

	CMapPath *path = itemToPath(item);

	mapManager->propertiesPath(path);

	item->setText(0,mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd()));
	item->setText(1,path->getBeforeCommand());
	item->setText(2,path->getAfterCommand());
}
