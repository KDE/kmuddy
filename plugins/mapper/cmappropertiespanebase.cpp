/***************************************************************************
                               cmappropertiespanebase.cpp
                             -------------------
    begin                : Mon Aug 6 2001
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

#include "cmappropertiespanebase.h"

CMapPropertiesPaneBase::CMapPropertiesPaneBase(QString title,QIcon * icon,elementTyp panelType,CMapElement *element,QWidget *parent)
	: QWidget(parent)
{
	setIcon(icon);
	setPanelType(panelType);
	setTitle(title);
	setElement(element);
}

#include "moc_cmappropertiespanebase.cpp"
