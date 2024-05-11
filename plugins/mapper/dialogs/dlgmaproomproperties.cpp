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
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qtabwidget.h>
#include <qtreewidget.h>
#include <QDebug>

#include <kcolorbutton.h>
#include <KLocalizedString>

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

	lstContents->addItems(*room->getContentsList());

	regenerateExits();

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(ROOM,(CMapElement*)roomElement,(QWidget *)RoomsTab);
	for (CMapPropertiesPaneBase *pane : paneList)
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
  lstPaths->clear();
  QTreeWidgetItem *item = nullptr;
  for (CMapPath *path : *room->getPathList())
  {
    QString direction = mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd());
    qDebug() << "Path : " << path->getSrcDir() << "," << path->getSpecialCmd() << "," << direction;	

    item = new QTreeWidgetItem();
    item->setText(0, direction);
    item->setText(1, path->getBeforeCommand());
    item->setText(2, path->getAfterCommand());
    lstPaths->addTopLevelItem(item);
  }

  if (item) lstPaths->setCurrentItem(item);
}

void DlgMapRoomProperties::slotAccept()
{
  CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Changed Room Properties"),room);

  command->compare("Label",room->getLabel(),txtLabel->text().trimmed());
  command->compare("Description",room->getDescription(),txtDescription->toPlainText().trimmed());
  command->compare("Color",room->getColor(),cmdRoomColor->color());
  command->compare("DefaultColor",room->getUseDefaultCol(),chkUseDefaltColor->isChecked());
  command->compare("LabelPos",(int)room->getLabelPosition(),(int)getLabelPos());

  QStringList newContents;
  for (int i = 0; i < lstContents->count(); ++i)
  {
    QString text = lstContents->item(i)->text().trimmed();
    if (text.length()) newContents += text;
  }

  command->compare("Contents",*room->getContentsList(),newContents);

  QList<CMapPath*> wipe;
  for (CMapPath *path : *room->getPathList())
  {
    QString name = mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd());

    bool found = false;

    for (int i = 0; i < lstPaths->topLevelItemCount(); ++i)
      if (lstPaths->topLevelItem(i)->text(0) == name)
        found = true;
    if (!found)
      wipe.push_back(path);
  }
  for (CMapPath *path : wipe)
    mapManager->deleteElement(path);

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
  QListWidgetItem *item = lstContents->takeItem(lstContents->currentRow());
  if (item) delete item;
}

void DlgMapRoomProperties::slotAddItem()
{
  lstContents->addItem(QString());
  lstContents->setCurrentRow(lstContents->count() - 1);
}

void DlgMapRoomProperties::slotNewItemSelected()
{
  QList<QListWidgetItem *> sel = lstContents->selectedItems();
  if (!sel.count()) return;
  txtItemName->setText(sel.at(0)->text());
}

void DlgMapRoomProperties::slotEditItemName(const QString & name)
{
  QListWidgetItem *item = lstContents->currentItem();
  if (item) item->setText(name);
}

void DlgMapRoomProperties::slotPathDelete()
{
  QList<QTreeWidgetItem *> sel = lstPaths->selectedItems();
  for (QTreeWidgetItem *item : sel)
    delete item;
}

void DlgMapRoomProperties::slotPathProperties()
{
  QTreeWidgetItem *item = lstPaths->currentItem();
  int idx = lstPaths->indexOfTopLevelItem(item);
  if ((idx < 0) || (idx >= room->getPathList()->count())) return;
  CMapPath *path = room->getPathList()->at(idx);

  mapManager->propertiesPath(path);

  item->setText(0, mapManager->directionToText(path->getSrcDir(),path->getSpecialCmd()));
  item->setText(1, path->getBeforeCommand());
  item->setText(2, path->getAfterCommand());
}

#include "moc_dlgmaproomproperties.cpp"
