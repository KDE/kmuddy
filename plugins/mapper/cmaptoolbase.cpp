/***************************************************************************
                               cmaptoolbase.cpp
                             -------------------
    begin                : Tue May 1 2001
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

#include "cmaptoolbase.h"

#include "cmapmanager.h"

#include <kstandarddirs.h>
#include <qactiongroup.h>
#include <kactioncollection.h>

/** The construtor for the tool */
CMapToolBase::CMapToolBase(KActionCollection *actionCollection,QString description,QIcon icon,CMapManager *manager,QString actionName,QActionGroup *group,QObject *parent)
	: QObject (parent)
{
	
  mapManager = manager;

  action = new KToggleAction (this);
  action->setText (description);
  action->setIcon (icon);
  connect (action, SIGNAL (triggered (bool)), this, SLOT (slotActionSelected ()));
  if (!group) group = manager->toolGroup();
  group->addAction (action);
  actionCollection->addAction (actionName, action);
}

CMapToolBase::~CMapToolBase()
{
}

/** Used to tell the tool were to find the map manager */
void CMapToolBase::setManager(CMapManager *manager)
{
	mapManager = manager;
}

/** Used to set the checked state of the tool */
void CMapToolBase::setChecked(bool checked)
{
	action->setChecked(checked);
}

/** This is called when the tool button is clicked */
void CMapToolBase::slotActionSelected()
{
	if (action->isChecked())
	{
		emit actionSelected(this);
		mapManager->setCurrentTool(this);
	}
}

/** Used to set the whats this text for the tool */
void CMapToolBase::setWhatsThis(QString text)
{
	action->setWhatsThis(text);
}

/** Used to set the toop tip of the tool */
void CMapToolBase::setToolTip(QString tip)
{
	action->setToolTip(tip);
}

void CMapToolBase::plug (QWidget *w, int index)
{
	w->addAction(action);
}

