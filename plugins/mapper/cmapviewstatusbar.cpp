/***************************************************************************
                               cmapmudviewstatusbar.cpp
                             -------------------
    begin                : Thu May 10 2001
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

#include "cmapviewstatusbar.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3HBoxLayout>

#include <klocale.h>

CMapViewStatusbar::CMapViewStatusbar(QWidget *parent, const char *name ) : Q3Frame(parent,name)
{
	setFocusProxy(parent);
	setFrameStyle( Box | Sunken );
	layout = new Q3HBoxLayout(this);
	layout->setMargin(3);

	lblRoomLabel = new QLabel(i18n("Current Room : "),this);
	lblRoomLabel->setFocusProxy(parent);
	QFont f = lblRoomLabel->font();
	f.setItalic(true);
	lblRoomStatus = new QLabel(i18n("Unknown"),this);
	lblRoomStatus->setFont(f);
	lblRoomStatus->setFocusProxy(parent);
	lblLevelLabel = new QLabel(i18n("Level : "),this);
	lblLevelLabel->setFocusProxy(parent);
	lblLevelStatus = new QLabel(i18n("1"),this);
	lblLevelStatus->setFont(f);
	lblLevelStatus->setFocusProxy(parent);
	lblZoneLabel = new QLabel(i18n("Zone : "),this);
	lblZoneLabel->setFocusProxy(parent);
	lblZoneStatus = new QLabel(i18n("Unnamed"),this);
	lblZoneStatus->setFont(f);
	lblZoneStatus->setFocusProxy(parent);

	layout->addSpacing(5);
	layout->addWidget(lblZoneLabel);
	layout->addWidget(lblZoneStatus);
	layout->addSpacing(5);
	layout->addWidget(lblLevelLabel);
	layout->addWidget(lblLevelStatus);
	layout->addSpacing(5);
	layout->addWidget(lblRoomLabel);
	layout->addWidget(lblRoomStatus);
	layout->addStretch(1);
	layout->addSpacing(5);
}

CMapViewStatusbar::~CMapViewStatusbar()
{
}

void CMapViewStatusbar::setLevel(int lvl)
{
	lblLevelStatus->setText(QString::number(lvl));
}

void CMapViewStatusbar::setZone(QString zone)
{
	if (zone.isEmpty())
		zone = "Unknown";
		
	lblZoneStatus->setText(zone);
}

void CMapViewStatusbar::setRoom(QString room)
{
	if (room.isEmpty())
		room = "Unknown";

	lblRoomStatus->setText(room);
}	

void CMapViewStatusbar::addViewIndicator(QLabel *indicator)
{
	layout->insertWidget(0,indicator);
}

void CMapViewStatusbar::addFollowButton(QPushButton *button)
{
	layout->addWidget(button);
}

