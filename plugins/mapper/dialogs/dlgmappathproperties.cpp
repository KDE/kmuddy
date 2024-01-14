/***************************************************************************
                          dlgmappathproperties.cpp  -  description
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

#include "dlgmappathproperties.h"

#include <qlineedit.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <QLocale>

#include "../cmapmanager.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"
#include "../cmappath.h"
#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"

#include <KLocalizedString>

DlgMapPathProperties::DlgMapPathProperties(CMapManager *manager,KConfigGroup pathProperties,bool undoable,QWidget *parent)
	: QDialog(parent)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotAccept()));

	useProperties = true;
	properties = pathProperties;
	pathUndoable = undoable;
	mapManager = manager;
        path = nullptr;

	txtSrcBefore->setText(properties.readEntry("SrcBeforeCommand",""));
	txtSrcAfter->setText(properties.readEntry("SrcAfterCommand",""));
	txtDestBefore->setText(properties.readEntry("DestBeforeCommand",""));
	txtDestAfter->setText(properties.readEntry("DestAfterCommand",""));

        bool twoWay = properties.readEntry("PathTwoWay", manager->getMapData()->defaultPathTwoWay);
        directionTyp srcDir = (directionTyp) properties.readEntry("SrcDir", (int) SPECIAL);
        if (srcDir != SPECIAL) setSrcDirection(srcDir);
        directionTyp destDir = (directionTyp) properties.readEntry("DestDir", (int) SPECIAL);
        if (destDir != SPECIAL) setDestDirection(destDir);

	optTwoWay->setChecked(twoWay);
	slotDirectionChange();

	txtSpecialSrc->setText(properties.readEntry("SpecialCmdSrc"));
	txtSpecialDest->setText(properties.readEntry("SpecialCmdDest"));
	bool specialExit = properties.readEntry("SpecialExit",false);

	chkNormal->setChecked(!specialExit);
	chkSpecial->setChecked(specialExit);

	slotExitTypeChange();

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(PATH,nullptr,(QWidget *)tabPaths);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		tabPaths->addTab(pane,pane->getTitle());
		connect(cmdOK,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}
}

DlgMapPathProperties::DlgMapPathProperties(CMapManager *manager,CMapPath *pathElement,bool undoable,QWidget *parent)
	: QDialog(parent)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotAccept()));

	useProperties = false;
	pathUndoable = undoable;
	path = pathElement;
	mapManager = manager;

	txtSpecialSrc->setText(path->getSpecialCmd());
	txtSrcBefore->setText(path->getBeforeCommand());
	txtSrcAfter->setText(path->getAfterCommand());

	if (path->getOpsitePath())
	{
		txtDestBefore->setText(path->getOpsitePath()->getBeforeCommand());
		txtDestAfter->setText(path->getOpsitePath()->getAfterCommand());
		txtSpecialDest->setText(path->getOpsitePath()->getSpecialCmd());
		optTwoWay->setChecked(true);
	}
	else
	{
		fraDestSrcCommands->setEnabled(false);
		txtSpecialSrc->setEnabled(false);
		optTwoWay->setChecked(false);
	}


	chkNormal->setChecked(!path->getSpecialExit());
	chkSpecial->setChecked(path->getSpecialExit());

	setSrcDirection(path->getSrcDir());
	setDestDirection(path->getDestDir());

	slotExitTypeChange();

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(PATH,(CMapElement*)pathElement,(QWidget *)tabPaths);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		tabPaths->addTab(pane,pane->getTitle());
		connect(cmdOK,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}

qDebug()<<optTwoWay->isChecked();
}

DlgMapPathProperties::~DlgMapPathProperties()
{
}

void DlgMapPathProperties::setSrcDirection(directionTyp direction)
{
	cmdSrcN->setChecked(false);
	cmdSrcS->setChecked(false);
	cmdSrcE->setChecked(false);
	cmdSrcW->setChecked(false);
	cmdSrcSE->setChecked(false);
	cmdSrcNE->setChecked(false);
	cmdSrcSW->setChecked(false);
	cmdSrcNW->setChecked(false);
	cmdSrcUp->setChecked(false);
	cmdSrcDown->setChecked(false);

	switch (direction)
	{
		case NORTH     : cmdSrcN->setChecked(true); break;
		case SOUTH     : cmdSrcS->setChecked(true); break;
		case EAST      : cmdSrcE->setChecked(true); break;
		case WEST      : cmdSrcW->setChecked(true); break;
		case SOUTHEAST : cmdSrcSE->setChecked(true); break;
		case NORTHEAST : cmdSrcNE->setChecked(true); break;
		case SOUTHWEST : cmdSrcSW->setChecked(true); break;
		case NORTHWEST : cmdSrcNW->setChecked(true); break;
		case UP        : cmdSrcUp->setChecked(true); break;
		case DOWN      : cmdSrcDown->setChecked(true); break;
		case SPECIAL   : break;
	}
}

void DlgMapPathProperties::setDestDirection(directionTyp direction)
{
	cmdDestN->setChecked(false);
	cmdDestS->setChecked(false);
	cmdDestE->setChecked(false);
	cmdDestW->setChecked(false);
	cmdDestSE->setChecked(false);
	cmdDestNE->setChecked(false);
	cmdDestSW->setChecked(false);
	cmdDestNW->setChecked(false);
	cmdDestUp->setChecked(false);
	cmdDestDown->setChecked(false);

	switch (direction)
	{
		case NORTH     : cmdDestN->setChecked(true); break;
		case SOUTH     : cmdDestS->setChecked(true); break;
		case EAST      : cmdDestE->setChecked(true); break;
		case WEST      : cmdDestW->setChecked(true); break;
		case SOUTHEAST : cmdDestSE->setChecked(true); break;
		case NORTHEAST : cmdDestNE->setChecked(true); break;
		case SOUTHWEST : cmdDestSW->setChecked(true); break;
		case NORTHWEST : cmdDestNW->setChecked(true); break;
		case UP        : cmdDestUp->setChecked(true); break;
		case DOWN      : cmdDestDown->setChecked(true); break;
		case SPECIAL   : break;
	}
}

directionTyp DlgMapPathProperties::getSrcDirection(void)
{
	if (chkSpecial->isChecked()) return SPECIAL;
	if (cmdSrcNE->isChecked()) return NORTHEAST;
	if (cmdSrcE->isChecked()) return EAST;
	if (cmdSrcSE->isChecked()) return SOUTHEAST;
	if (cmdSrcS->isChecked()) return SOUTH;
	if (cmdSrcSW->isChecked()) return SOUTHWEST;
	if (cmdSrcW->isChecked()) return WEST;
	if (cmdSrcNW->isChecked()) return NORTHWEST;
	if (cmdSrcN->isChecked()) return NORTH;
	if (cmdSrcDown->isChecked()) return DOWN;
	if (cmdSrcUp->isChecked()) return UP;
	return SPECIAL;
}

directionTyp DlgMapPathProperties::getDestDirection(void)
{
	if (chkSpecial->isChecked()) return SPECIAL;
	if (cmdDestNE->isChecked()) return NORTHEAST;
	if (cmdDestE->isChecked()) return EAST;
	if (cmdDestSE->isChecked()) return SOUTHEAST;
	if (cmdDestS->isChecked()) return SOUTH;
	if (cmdDestSW->isChecked()) return SOUTHWEST;
	if (cmdDestW->isChecked()) return WEST;
	if (cmdDestNW->isChecked()) return NORTHWEST;
	if (cmdDestN->isChecked()) return NORTH;
	if (cmdDestDown->isChecked()) return DOWN;
	if (cmdDestUp->isChecked()) return UP;
	return SPECIAL;
}

void DlgMapPathProperties::propertiesAccept(QString)
{
	properties.writeEntry("SrcBeforeCommand",txtSrcBefore->text().trimmed());
	properties.writeEntry("SrcAfterCommand",txtSrcAfter->text().trimmed());
	properties.writeEntry("SrcDir",(int)getSrcDirection());
	properties.writeEntry("DestDir",(int)getDestDirection());
	properties.writeEntry("SpecialExit",chkSpecial->isChecked());
	properties.writeEntry("SpecialCmdSrc",txtSpecialSrc->text().trimmed());

        properties.writeEntry("PathTwoWay", optTwoWay->isChecked());

	properties.writeEntry("DestBeforeCommand",txtDestBefore->text().trimmed());
	properties.writeEntry("DestAfterCommand",txtDestAfter->text().trimmed());
	properties.writeEntry("SpecialCmdDest",txtSpecialDest->text().trimmed());
}

void DlgMapPathProperties::pathAccept(QString cmdName)
{
	if (pathUndoable)
		mapManager->openCommandGroup(cmdName);

	CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,cmdName,path);

	command->compare("SrcBeforeCommand",path->getBeforeCommand(),txtSrcBefore->text().trimmed());
	command->compare("SrcAfterCommand",path->getAfterCommand(),txtSrcAfter->text().trimmed());
	command->compare("SrcDir",(int)path->getSrcDir(),(int)getSrcDirection());
	command->compare("DestDir",(int)path->getDestDir(),(int)getDestDirection());
	command->compare("SpecialExit",path->getSpecialExit(),chkSpecial->isChecked());
	command->compare("SpecialCmdSrc",path->getSpecialCmd(),txtSpecialSrc->text().trimmed());
        CMapPath *op = path->getOpsitePath();
	command->compare("PathTwoWay", op != nullptr, optTwoWay->isChecked());

	command->compare("DestBeforeCommand", op ? op->getBeforeCommand() : QString(), txtDestBefore->text().trimmed());
	command->compare("DestAfterCommand", op ? op->getAfterCommand() : QString(), txtDestAfter->text().trimmed());
	command->compare("SpecialCmdDest", op ? op->getSpecialCmd() : QString(), txtSpecialDest->text().trimmed());

	if (pathUndoable)
		mapManager->addCommand(command);
	else
		command->redo();

	if (pathUndoable)
		mapManager->closeCommandGroup();

}

bool DlgMapPathProperties::getTwoWay(void)
{
	return optTwoWay->isChecked();
}

QString DlgMapPathProperties::getDestBeforeCmd(void)
{
	return txtDestBefore->text().trimmed();
}

QString DlgMapPathProperties::getDestAfterCmd(void)
{
	return txtDestAfter->text().trimmed();
}

QString DlgMapPathProperties::getSpecialCmdDest(void)
{
	return txtSpecialDest->text().trimmed();
}


void DlgMapPathProperties::slotAccept()
{
	if (useProperties)
		propertiesAccept(i18n("Changed Path Properties"));
	else
		pathAccept(i18n("Changed Path Properties"));
}

void DlgMapPathProperties::slotDirectionChange()
{
	fraDestSrcCommands->setEnabled(optTwoWay->isChecked());
	if ( !chkNormal->isChecked() )
	{
		txtSpecialDest->setEnabled(optTwoWay->isChecked());
		lblDestToSrc->setEnabled(optTwoWay->isChecked());
	}
}

void DlgMapPathProperties::slotExitTypeChange()
{
	bool normal = chkNormal->isChecked();

	cmdSrcDown->setEnabled(normal);
	cmdSrcUp->setEnabled(normal);
	cmdSrcN->setEnabled(normal);
	cmdSrcNE->setEnabled(normal);
	cmdSrcE->setEnabled(normal);
	cmdSrcSE->setEnabled(normal);
	cmdSrcS->setEnabled(normal);
	cmdSrcSW->setEnabled(normal);
	cmdSrcW->setEnabled(normal);
	cmdSrcNW->setEnabled(normal);

	cmdDestDown->setEnabled(normal);
	cmdDestUp->setEnabled(normal);
	cmdDestN->setEnabled(normal);
	cmdDestNE->setEnabled(normal);
	cmdDestE->setEnabled(normal);
	cmdDestSE->setEnabled(normal);
	cmdDestS->setEnabled(normal);
	cmdDestSW->setEnabled(normal);
	cmdDestW->setEnabled(normal);
	cmdDestNW->setEnabled(normal);

	lblSrc->setEnabled(normal);
	lblDest->setEnabled(normal);

	lblSpecial->setEnabled(!normal);
	txtSpecialSrc->setEnabled(!normal);
	lblSrcToDest->setEnabled(!normal);

	lblDestToSrc->setEnabled(!normal && optTwoWay->isChecked());
	txtSpecialDest->setEnabled(!normal && optTwoWay->isChecked());

}

void DlgMapPathProperties::slotDestDown()
{
	setDestDirection(DOWN);
}

void DlgMapPathProperties::slotDestE()
{
	setDestDirection(EAST);
}

void DlgMapPathProperties::slotDestN()
{
	setDestDirection(NORTH);
}

void DlgMapPathProperties::slotDestNE()
{
	setDestDirection(NORTHEAST);
}

void DlgMapPathProperties::slotDestNW()
{
	setDestDirection(NORTHWEST);
}

void DlgMapPathProperties::slotDestS()
{
	setDestDirection(SOUTH);
}

void DlgMapPathProperties::slotDestSE()
{
	setDestDirection(SOUTHEAST);
}

void DlgMapPathProperties::slotDestSW()
{
	setDestDirection(SOUTHWEST);
}

void DlgMapPathProperties::slotDestUp()
{
	setDestDirection(UP);
}

void DlgMapPathProperties::slotDestW()
{
	setDestDirection(WEST);
}

void DlgMapPathProperties::slotSrcDown()
{
	setSrcDirection(DOWN);
}

void DlgMapPathProperties::slotSrcE()
{
	setSrcDirection(EAST);
}

void DlgMapPathProperties::slotSrcN()
{
	setSrcDirection(NORTH);
}

void DlgMapPathProperties::slotSrcNE()
{
	setSrcDirection(NORTHEAST);
}

void DlgMapPathProperties::slotSrcNW()
{
	setSrcDirection(NORTHWEST);
}

void DlgMapPathProperties::slotSrcS()
{
	setSrcDirection(SOUTH);
}

void DlgMapPathProperties::slotSrcSE()
{
	setSrcDirection(SOUTHEAST);
}

void DlgMapPathProperties::slotSrcSW()
{
	setSrcDirection(SOUTHWEST);
}

void DlgMapPathProperties::slotSrcUp()
{
	setSrcDirection(UP);
}

void DlgMapPathProperties::slotSrcW()
{
	setSrcDirection(WEST);
}

#include "moc_dlgmappathproperties.cpp"
