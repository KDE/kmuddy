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

#include <klocale.h>

#include "../cmapmanager.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"
#include "../cmappath.h"
#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"
#include "../cmapcmdtogglepathtwoway.h"

DlgMapPathProperties::DlgMapPathProperties(CMapManager *manager,KConfigGroup pathProperties,bool undoable,QWidget *parent, const char *name )
	: QDialog(parent,name)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotAccept()));

	useProperties = true;
	properties = pathProperties;
	pathUndoable = undoable;
	mapManager = manager;

	txtSrcBefore->setText(properties.readEntry("SrcBeforeCommand",""));
	txtSrcAfter->setText(properties.readEntry("SrcAfterCommand",""));

	if (properties.hasKey("PathTwoWay"))
	{
		txtDestBefore->setText(properties.readEntry("DestBeforeCommand",""));
		txtDestAfter->setText(properties.readEntry("DestAfterCommand",""));
		optTwoWay->setChecked(true);
	}
	else
	{
		fraDestSrcCommands->setEnabled(false);
		txtSpecialSrc->setEnabled(false);
		optOneWay->setChecked(true);
	}

	if (!(pathProperties.hasKey("SrcDir") && pathProperties.hasKey("DestDir")))
	{
		optTwoWay->setChecked(manager->getMapData()->defaultPathTwoWay);
		slotDirectionChange();
	}

	txtSpecialSrc->setText(properties.readEntry("SpecialCmdSrc"));
	txtSpecialDest->setText(properties.readEntry("SpecialCmdDest"));
	bool specialExit = properties.readEntry("SpecialExit",false);

	chkNormal->setChecked(!specialExit);
	chkSpecial->setChecked(specialExit);

	slotExitTypeChange();

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(PATH,NULL,(QWidget *)tabPaths);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		tabPaths->addTab(pane,pane->getTitle());
		connect(cmdOK,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}
}

DlgMapPathProperties::DlgMapPathProperties(CMapManager *manager,CMapPath *pathElement,bool undoable,QWidget *parent, const char *name )
	: QDialog(parent,name)
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
		optOneWay->setChecked(true);
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

}

DlgMapPathProperties::~DlgMapPathProperties()
{
}

void DlgMapPathProperties::setSrcDirection(directionTyp direction)
{
	cmdSrcN->setOn(false);
	cmdSrcS->setOn(false);
	cmdSrcE->setOn(false);
	cmdSrcW->setOn(false);
	cmdSrcSE->setOn(false);
	cmdSrcNE->setOn(false);
	cmdSrcSW->setOn(false);
	cmdSrcNW->setOn(false);
	cmdSrcUp->setOn(false);
	cmdSrcDown->setOn(false);

	switch (direction)
	{
		case NORTH     : cmdSrcN->setOn(true); break;
		case SOUTH     : cmdSrcS->setOn(true); break;
		case EAST      : cmdSrcE->setOn(true); break;
		case WEST      : cmdSrcW->setOn(true); break;
		case SOUTHEAST : cmdSrcSE->setOn(true); break;
		case NORTHEAST : cmdSrcNE->setOn(true); break;
		case SOUTHWEST : cmdSrcSW->setOn(true); break;
		case NORTHWEST : cmdSrcNW->setOn(true); break;
		case UP        : cmdSrcUp->setOn(true); break;
		case DOWN      : cmdSrcDown->setOn(true); break;
		case SPECIAL   : break;
	}
}

void DlgMapPathProperties::setDestDirection(directionTyp direction)
{
	cmdDestN->setOn(false);
	cmdDestS->setOn(false);
	cmdDestE->setOn(false);
	cmdDestW->setOn(false);
	cmdDestSE->setOn(false);
	cmdDestNE->setOn(false);
	cmdDestSW->setOn(false);
	cmdDestNW->setOn(false);
	cmdDestUp->setOn(false);
	cmdDestDown->setOn(false);

	switch (direction)
	{
		case NORTH     : cmdDestN->setOn(true); break;
		case SOUTH     : cmdDestS->setOn(true); break;
		case EAST      : cmdDestE->setOn(true); break;
		case WEST      : cmdDestW->setOn(true); break;
		case SOUTHEAST : cmdDestSE->setOn(true); break;
		case NORTHEAST : cmdDestNE->setOn(true); break;
		case SOUTHWEST : cmdDestSW->setOn(true); break;
		case NORTHWEST : cmdDestNW->setOn(true); break;
		case UP        : cmdDestUp->setOn(true); break;
		case DOWN      : cmdDestDown->setOn(true); break;
		case SPECIAL   : break;
	}
}

directionTyp DlgMapPathProperties::getSrcDirection(void)
{
	if (chkSpecial->isChecked()) return SPECIAL;
	if (cmdSrcNE->isOn()) return NORTHEAST;
	if (cmdSrcE->isOn()) return EAST;
	if (cmdSrcSE->isOn()) return SOUTHEAST;
	if (cmdSrcS->isOn()) return SOUTH;
	if (cmdSrcSW->isOn()) return SOUTHWEST;
	if (cmdSrcW->isOn()) return WEST;
	if (cmdSrcNW->isOn()) return NORTHWEST;
	if (cmdSrcN->isOn()) return NORTH;
	if (cmdSrcDown->isOn()) return DOWN;
	if (cmdSrcUp->isOn()) return UP;
	return SPECIAL;
}

directionTyp DlgMapPathProperties::getDestDirection(void)
{
	if (chkSpecial->isChecked()) return SPECIAL;
	if (cmdDestNE->isOn()) return NORTHEAST;
	if (cmdDestE->isOn()) return EAST;
	if (cmdDestSE->isOn()) return SOUTHEAST;
	if (cmdDestS->isOn()) return SOUTH;
	if (cmdDestSW->isOn()) return SOUTHWEST;
	if (cmdDestW->isOn()) return WEST;
	if (cmdDestNW->isOn()) return NORTHWEST;
	if (cmdDestN->isOn()) return NORTH;
	if (cmdDestDown->isOn()) return DOWN;
	if (cmdDestUp->isOn()) return UP;
	return SPECIAL;
}

void DlgMapPathProperties::propertiesAccept(QString)
{
	properties.writeEntry("SrcBeforeCommand",txtSrcBefore->text().trimmed());
	properties.writeEntry("SrcAfterCommand",txtSrcAfter->text().trimmed());
	properties.writeEntry("SrcDir",(int)getSrcDirection(),0);
	properties.writeEntry("DestDir",(int)getDestDirection(),0);
	properties.writeEntry("SpecialExit",chkSpecial->isChecked(),false);
	properties.writeEntry("SpecialCmdSrc",txtSpecialSrc->text().trimmed());

	if (properties.hasKey("PathTwoWay"))
	{
		if (optOneWay->isChecked())
		{
			properties.writeEntry("PathOneWay","");
			properties.deleteEntry("PathTwoWay");
		}
	}
	else
	{
		if (optTwoWay->isChecked())
		{
			properties.deleteEntry("PathOneWay");
			properties.writeEntry("PathTwoWay","");
		}
	}

	if (properties.hasKey("PathTwoWay"))
	{
		properties.writeEntry("DestBeforeCommand",txtDestBefore->text().trimmed());
		properties.writeEntry("DestAfterCommand",txtDestAfter->text().trimmed());
		properties.writeEntry("SpecialCmdDest",txtSpecialDest->text().trimmed());
	}
	else
	{
		if (optTwoWay->isChecked())
		{
			properties.writeEntry("DestBeforeCommand",txtDestBefore->text().trimmed());
			properties.writeEntry("DestAfterCommand",txtDestAfter->text().trimmed());
			properties.writeEntry("SpecialCmdDest",txtSpecialDest->text().trimmed());
		}
	}
}

void DlgMapPathProperties::pathAccept(QString cmdName)
{
	if (pathUndoable)
		mapManager->openCommandGroup(cmdName);

	if (path->getOpsitePath())
	{
		if (optOneWay->isChecked())
		{
			CMapCmdTogglePathTwoWay *cmd =  new CMapCmdTogglePathTwoWay(mapManager,i18n("Make Path Oneway"),path);
			if (pathUndoable)
				mapManager->addCommand(cmd);
			else
				cmd->redo();
		}
	}
	else
	{
		if (optTwoWay->isChecked())
		{
			CMapCmdTogglePathTwoWay *cmd =  new CMapCmdTogglePathTwoWay(mapManager,i18n("Make Path Twoway"),path);
			if (pathUndoable)
				mapManager->addCommand(cmd);
			else
				cmd->redo();
		}
	}

	CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,cmdName,path);

	command->compare("SrcBeforeCommand",path->getBeforeCommand(),txtSrcBefore->text().trimmed());
	command->compare("SrcAfterCommand",path->getAfterCommand(),txtSrcAfter->text().trimmed());
	command->compare("SrcDir",(int)path->getSrcDir(),(int)getSrcDirection());
	command->compare("DestDir",(int)path->getDestDir(),(int)getDestDirection());
	command->compare("SpecialExit",path->getSpecialExit(),chkSpecial->isChecked());
	command->compare("SpecialCmdSrc",path->getSpecialCmd(),txtSpecialSrc->text().trimmed());

	if (path->getOpsitePath())
	{
		command->compare("DestBeforeCommand",path->getOpsitePath()->getBeforeCommand(),txtDestBefore->text().trimmed());
		command->compare("DestAfterCommand",path->getOpsitePath()->getAfterCommand(),txtDestAfter->text().trimmed());
		command->compare("SpecialCmdDest",path->getOpsitePath()->getSpecialCmd(),txtSpecialDest->text().trimmed());
	}
	else
	{
		if (optTwoWay->isChecked())
		{
			command->getNewProperties().writeEntry("DestBeforeCommand",txtDestBefore->text().trimmed());
			command->getNewProperties().writeEntry("DestAfterCommand",txtDestAfter->text().trimmed());
			command->getNewProperties().writeEntry("SpecialCmdDest",txtSpecialDest->text().trimmed());
		}
	}

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
	fraDestSrcCommands->setEnabled(!optOneWay->isChecked());
	if ( !chkNormal->isChecked() )
	{
		txtSpecialDest->setEnabled(!optOneWay->isChecked());
		lblDestToSrc->setEnabled(!optOneWay->isChecked());
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

	lblDestToSrc->setEnabled(!normal && !optOneWay->isChecked());
	txtSpecialDest->setEnabled(!normal && !optOneWay->isChecked());

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
