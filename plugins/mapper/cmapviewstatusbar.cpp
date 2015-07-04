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

#include <qlabel.h>
#include <qpushbutton.h>
#include <QHBoxLayout>

#include <klocale.h>

struct CMapViewStatusbar::Private {
  QLabel *lblRoomLabel;
  QLabel *lblRoomStatus;
  QLabel *lblLevelLabel;
  QLabel *lblLevelStatus;
  QLabel *lblZoneLabel;
  QLabel *lblZoneStatus;
};

CMapViewStatusbar::CMapViewStatusbar(QWidget *parent) : QStatusBar(parent)
{
  d = new Private;

  // labels
  d->lblRoomLabel = new QLabel(i18n("Current Room : "), this);
  d->lblLevelLabel = new QLabel(i18n("Level : "),this);
  d->lblZoneLabel = new QLabel(i18n("Zone : "),this);

  QFont f = d->lblRoomLabel->font();
  f.setItalic(true);
  d->lblRoomStatus = new QLabel(i18n("Unknown"),this);
  d->lblRoomStatus->setFont(f);
  d->lblLevelStatus = new QLabel(i18n("1"),this);
  d->lblLevelStatus->setFont(f);
  d->lblZoneStatus = new QLabel(i18n("Unnamed"),this);
  d->lblZoneStatus->setFont(f);

  addWidget(d->lblZoneLabel);
  addWidget(d->lblZoneStatus);
  addWidget(d->lblLevelLabel);
  addWidget(d->lblLevelStatus);
  addWidget(d->lblRoomLabel);
  addWidget(d->lblRoomStatus);
}

CMapViewStatusbar::~CMapViewStatusbar()
{
  delete d;
}

void CMapViewStatusbar::setLevel(int lvl)
{
  d->lblLevelStatus->setText(QString::number(lvl));
}

void CMapViewStatusbar::setZone(QString zone)
{
  if (zone.isEmpty())
    zone = "Unknown";
  d->lblZoneStatus->setText(zone);
}

void CMapViewStatusbar::setRoom(QString room)
{
  if (room.isEmpty()) room = "Unknown";
  d->lblRoomStatus->setText(room);
}	

void CMapViewStatusbar::addFollowButton(QPushButton *button)
{
  addPermanentWidget(button);
}

