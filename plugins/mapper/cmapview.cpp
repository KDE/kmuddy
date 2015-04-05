/***************************************************************************
                               cmapview.cpp
                             -------------------
    begin                : Mon Mar 19 2001
    copyright            : (C) 2001 by Kmud Developer Team
                           (C) 2007 Tomas Mecir <kmuddy@kmuddy.net>
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

#include "cmapview.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>

#include "cmapmanager.h"
#include "cmapzone.h"
#include "cmaplevel.h"
#include "cmappath.h"
#include "cmapview.h"
#include "cmapelement.h"
#include "cmapviewbase.h"
#include "cmapwidget.h"
#include "cmapviewstatusbar.h"
#include "cmaptoolbase.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

CMapView::CMapView(CMapManager *manager,QWidget *parent) : CMapViewBase(manager,parent)
{
  kDebug() << "CMapView::CMapView create view";

  activeLed = UserIcon ("kmud_active.png");
  inactiveLed = UserIcon ("kmud_inactive.png");

  QVBoxLayout *vbox = new QVBoxLayout((QWidget *)this);

  scroller = new QScrollArea(this);
  vbox->addWidget(scroller);
  scroller->setWidgetResizable(true);
  scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  mapWidget = new CMapWidget(this,manager,scroller);
  scroller->setWidget(mapWidget);
  mapWidget->show();

  statusbar = new CMapViewStatusbar(this);
  vbox->addWidget(statusbar);

  lblActive = new QLabel(i18n("Active"),statusbar);
  lblActive->setPixmap(inactiveLed);
  lblActive->setFocusProxy(this);
  statusbar->addViewIndicator(lblActive);

  cmdFollowMode = new QPushButton(i18n("Follow Mode"),statusbar);
  cmdFollowMode->setPixmap(UserIcon("kmud_follow.png"));
  cmdFollowMode->setToggleButton(true);
  cmdFollowMode->setFocusProxy(this);
  statusbar->addFollowButton(cmdFollowMode);

  //FIXME_jp : get settings for status bar instead of defaults

  //FIXME_jp : Add proper resize instead of test size
  changed();
}

CMapView::~CMapView()
{
  kDebug() << "CMapView::~CMapView()";
}

/** This method is called when an element is added */
void CMapView::addedElement(CMapElement *element)
{
  if (isElementVisible(element))
  {
    checkSize(element->getHighPos());
    mapWidget->update();
  }
}

/** This method is called when an element is deleted */
void CMapView::deletedElement(CMapLevel *deletedFromLevel)
{
  CMapLevel *upperLevel = getCurrentlyViewedLevel()->getNextLevel();
  CMapLevel *lowerLevel = getCurrentlyViewedLevel()->getPrevLevel();

  if (deletedFromLevel == getCurrentlyViewedLevel())
    mapWidget->update();

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
    if (deletedFromLevel == upperLevel)
      mapWidget->update();

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
    if (deletedFromLevel == lowerLevel)
      mapWidget->update();
}

/** This method is called when an element is changed */
void CMapView::changedElement(CMapElement *element)
{
  if (isElementVisible(element))
  {
    checkSize(element->getHighPos());
    mapWidget->update();
  }

  if (element == mapManager->getCurrentRoom())
    statusbar->setRoom(mapManager->getCurrentRoom()->getLabel());
}

/** This method is called when a map level is changed */
void CMapView::changedLevel(CMapLevel *level)
{
  if (!isLevelVisible(level)) return;
  changed();

}

void CMapView::requestPaint()
{
  mapWidget->update();
}

void CMapView::changed()
{
  maxSize = QSize(0,0);

  CMapLevel *level = getCurrentlyViewedLevel();
  if (!level) {
    mapWidget->update();
    return;
  }

  CMapLevel *upperLevel = level->getNextLevel();
  CMapLevel *lowerLevel = level->getPrevLevel();

  QPoint size(0,0);

  // Calc the size the widget should be
  QList<CMapElement *> lst = level->getAllElements();
  foreach (CMapElement *element, lst)
  {
    if (element->getHighX()>size.x()) size.setX(element->getHighX());
    if (element->getHighY()>size.y()) size.setY(element->getHighY());
  }

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    lst = upperLevel->getAllElements();
    foreach (CMapElement *element, lst)
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
  {
    lst = lowerLevel->getAllElements();
    foreach (CMapElement *element, lst)
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  checkSize(size);
  mapWidget->update();
}


/** Tell this map widget to display a different zone */
void CMapView::showPosition(QPoint pos,CMapLevel *level,bool centerView)
{
  if (!level) { changed(); return; }

  setLevel(level);

  changed();

  // Center on the position
  if (centerView)
    scroller->ensureVisible(pos.x(),pos.y(), width()/2, height()/2);

  // Update the status bar
  statusbar->setRoom(mapManager->getCurrentRoom()->getLabel());
  statusbar->setZone(level->getZone()->getLabel());
  statusbar->setLevel(level->getNumber());

  if (getActive())
    mapManager->activeViewChanged();
}

/** This is used ensure a location is visiable for views that scroll */
void CMapView::ensureVisible(QPoint pos)
{
  scroller->ensureVisible(pos.x(),pos.y(),10,10);
}

/** Used to calculate the correct size for the widget */
void CMapView::checkSize(QPoint pos)
{
  if (pos.x() > maxSize.width()) maxSize.setWidth(pos.x());
  if (pos.y() > maxSize.height()) maxSize.setHeight(pos.y());

  int view_x = width();
  int view_y = height();

  if (maxSize.width() > view_x)
    view_x = maxSize.width();

  if (maxSize.height() > view_y)
    view_y = maxSize.height();

  QSize grid = mapManager->getMapData()->gridSize;
  view_x += grid.width() * 3;
  view_y += grid.height() * 3;

  if (view_y != mapWidget->height() || view_x != mapWidget->width())
    mapWidget->setFixedSize(view_x, view_y);
}

/** Get the max x cord of all elements */
int CMapView::getMaxX(void)
{
        return maxSize.width();
}

/** Get the max y cord of all elements */
int CMapView::getMaxY(void)
{
        return maxSize.height();
}

/** Used to set the follow mode */
void CMapView::setFollowMode(bool follow)
{
  cmdFollowMode->setChecked(follow);
}

/** Used to get the follow mode */
bool CMapView::getFollowMode(void)
{
  return cmdFollowMode->isChecked();
}

int CMapView::getWidth(void)
{
  if (mapWidget->width() > scroller->viewport()->width())
    return mapWidget->width();
  return scroller->viewport()->width();
}

int CMapView::getHeight(void)
{
  if (mapWidget->height() > scroller->viewport()->height())
    return mapWidget->height();
  return scroller->viewport()->height();
}

/** Used to set the view to active */
void CMapView::setActive(bool active)
{
  CMapViewBase::setActive(active);
  lblActive->setPixmap(active ? activeLed : inactiveLed);
}

void CMapView::setCursor ( const QCursor & cursor)
{
  if (mapWidget)
    mapWidget->setCursor(cursor);
}

void CMapView::resizeEvent (QResizeEvent *)
{
  changed();
}

