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

#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <klocale.h>

#include "cmapzone.h"
#include "cmapzonemanager.h"
#include "cmapview.h"
#include "cmapmanager.h"

struct CMapViewStatusbar::Private {
  CMapManager *manager;

  QLabel *lblRoomLabel;
  QLabel *lblLevelLabel;
  QLabel *lblZoneLabel;

  QLabel *lblRoomStatus;
  QComboBox *levelPicker;
  QComboBox *zonePicker;

  CMapZone *zone;
  CMapLevel *level;
};

CMapViewStatusbar::CMapViewStatusbar(CMapManager *manager, QWidget *parent) : QStatusBar(parent)
{
  d = new Private;
  d->manager = manager;
  d->zone = 0;
  d->level = 0;

  // labels
  d->lblRoomLabel = new QLabel(i18n("Current Room : "), this);
  d->lblLevelLabel = new QLabel(i18n("Level : "),this);
  d->lblZoneLabel = new QLabel(i18n("Zone : "),this);

  QFont f = d->lblRoomLabel->font();
  f.setItalic(true);
  d->lblRoomStatus = new QLabel(i18n("Unknown"),this);
  d->lblRoomStatus->setFont(f);
  d->levelPicker = new QComboBox(this);
  d->levelPicker->setFont(f);
  d->levelPicker->setEditable(true);
  d->levelPicker->setInsertPolicy(QComboBox::NoInsert);
  connect(d->levelPicker, SIGNAL(activated(int)), this, SLOT(changeLevel(int)));
  connect(d->levelPicker, SIGNAL(editTextChanged(const QString &)), this, SLOT(renameLevel(const QString &)));

  d->zonePicker = new QComboBox(this);
  d->zonePicker->setFont(f);
  d->zonePicker->setEditable(true);
  d->zonePicker->setInsertPolicy(QComboBox::NoInsert);
  connect(d->zonePicker, SIGNAL(activated(int)), this, SLOT(changeZone(int)));
  connect(d->zonePicker, SIGNAL(editTextChanged(const QString &)), this, SLOT(renameZone(const QString &)));

  addWidget(d->lblZoneLabel);
  addWidget(d->zonePicker);
  addWidget(d->lblLevelLabel);
  addWidget(d->levelPicker);
  addWidget(d->lblRoomLabel);
  addWidget(d->lblRoomStatus);
}

CMapViewStatusbar::~CMapViewStatusbar()
{
  delete d;
}

void CMapViewStatusbar::setLevel(CMapLevel *lvl)
{
  d->level = lvl;
  int idx = d->zone->levelIndex(lvl);
  d->levelPicker->setCurrentIndex(idx);
}

void CMapViewStatusbar::setZone(CMapZone *zone)
{
  // this is because we can't set the model in the constructor, as the zone manager doesn't yet exist
  CMapZoneManager *zones = d->manager->zoneManager();
  if (zones) {
    if (d->zonePicker->model() != zones->zonesModel())
      d->zonePicker->setModel(zones->zonesModel());
    d->zonePicker->setCurrentIndex(zones->activeZone());
  }

  if (d->zone == zone) return;
  d->zone = zone;

  d->levelPicker->setModel(zone->levelsModel());
  setLevel(d->manager->getActiveView()->getCurrentlyViewedLevel());
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

void CMapViewStatusbar::changeLevel(int index)
{
  CMapLevel *level = d->zone->getLevel(index);
  if (!level) return;
  d->manager->displayLevel(level, false);
}

void CMapViewStatusbar::renameLevel(const QString &name)
{
  if (!d->zone) return;

  // apparently this also gets called when we switch to a new level - then we do not want to rename anything
  int idx = d->zone->levelIndex(d->level);
  if (idx != d->levelPicker->currentIndex()) return;

  d->zone->setLevelName(d->level, name);
}

void CMapViewStatusbar::changeZone(int index)
{
  d->manager->zoneManager()->loadZone(index);
}

void CMapViewStatusbar::renameZone(const QString &name)
{
  // apparently this also gets called when we switch to a new zone - then we do not want to rename anything
  int idx = d->manager->zoneManager()->activeZone();
  if (idx != d->zonePicker->currentIndex()) return;

  d->manager->zoneManager()->renameZone(idx, name);
}

