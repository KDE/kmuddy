/***************************************************************************
                          dlgmapinfo.cpp  -  description
                             -------------------
    begin                : Wed Mar 7 2001
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

#include "dlgmapinfo.h"

#include <qlabel.h>

#include "../cmapmanager.h"
#include "../cmaproom.h"
#include "../cmapdata.h"
#include "../cmapzone.h"
#include "../cmaplevel.h"

DlgMapInfo::DlgMapInfo(CMapManager *manager,QWidget *parent, const char *name ) : QDialog(parent, name)
{
  setupUi (this);

	// FIXME_jp: need to get character and mud detials

	// Starting position detials
	CMapRoom *loginRoom = manager->getLoginRoom();

	txtX->setText(intToStr(loginRoom->getX()));
	txtY->setText(intToStr(loginRoom->getY()));
	txtLvl->setText(intToStr(loginRoom->getLevel()->getNumber()));

	// Element detials
	int levels,rooms,paths,labels;

	manager->getCounts(&levels,&rooms,&paths,&labels);

	txtLvls->setText(intToStr(levels));
	txtRooms->setText(intToStr(rooms));
	txtPaths->setText(intToStr(paths));
}

DlgMapInfo::~DlgMapInfo()
{
}

QString DlgMapInfo::intToStr(int num)
{
	QString str;

	str.sprintf("%d",num);

	return str;
}
