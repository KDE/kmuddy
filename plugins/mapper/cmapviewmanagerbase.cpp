/***************************************************************************
                               cmapviewmanagerbase.cpp
                             -------------------
    begin                : Tue Aug 7 2001
    copyright            : (C) 2001 by Kmud Developer Team
    email                : kmud-devel@kmud.de
                         : (C) 2007 Tomas Mecir <kmuddy @kmuddy.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cmapviewmanagerbase.h"

#include "cmapviewbase.h"
#include "cmapmanager.h"

CMapViewManagerBase::CMapViewManagerBase(CMapManager *manager,QString actionName,QString description, KActionCollection *actionCollection,QIcon icon,QObject *parent, const char *name)
	: QObject(parent,name)
{
  action = new KToggleAction (this);
  action->setText (description);
  action->setIcon (icon);
  connect (action, SIGNAL (triggered ()), this, SLOT (slotRequestNewView ()));
  action->setChecked (false);
  actionCollection->addAction (actionName, action);

  actName = actionName;
  mapManager = manager;
}

CMapViewManagerBase::CMapViewManagerBase(CMapManager *manager,QString actionName,QString description, KActionCollection *actionCollection,QObject *parent, const char *name)
	: QObject(parent,name)
{

  action = new KToggleAction (this);
  action->setText (description);
  connect (action, SIGNAL (triggered ()), this, SLOT (slotRequestNewView ()));
  action->setChecked (false);
  actionCollection->addAction (actionName, action);
  
  actName = actionName;
  mapManager = manager;
}


CMapViewManagerBase::~CMapViewManagerBase()
{
}

/** Used to set the whats this text for the view */
void CMapViewManagerBase::setWhatsThis(QString text)
{
	action->setWhatsThis(text);
}

/** Used to set the toop tip of the view */
void CMapViewManagerBase::setToolTip(QString tip)
{
	action->setToolTip(tip);
}
