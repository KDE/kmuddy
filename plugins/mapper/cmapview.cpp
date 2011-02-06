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

#include <qwidget.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QPixmap>
#include <QKeyEvent>
#include <Q3VBoxLayout>

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
#include <kvbox.h>

CMapView::CMapView(CMapManager *manager,QWidget *parent, const char *name) : CMapViewBase(manager,parent,name)
{
  kDebug() << "CMapView::CMapView create view";

  activeLed = UserIcon ("kmud_active.png");
  inactiveLed = UserIcon ("kmud_inactive.png");

  Q3VBoxLayout *vbox = new Q3VBoxLayout((QWidget *)this);
  mapWidget = new CMapWidget(this,manager,(QWidget *)this,"mapwidget");
  vbox->addWidget( mapWidget);
  mapWidget->show();

  statusbar = new CMapViewStatusbar(this,"statusbar");
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
  maxSize = QSize(0,0);
  checkSize(QPoint(300,300));
  bCtrlPressed = false;
}

CMapView::~CMapView()
{
  kDebug() << "CMapView::~CMapView()";
}

/** This method is called when an element is added */
void CMapView::addedElement(CMapElement *element)
{
  if (isElementVisibale(element))
  {
    checkSize(element->getHighPos());
    redraw();
  }
}

/** This method is called when an element is deleted */
void CMapView::deletedElement(CMapLevel *deletedFromLevel)
{
  CMapLevel *upperLevel = getCurrentlyViewedLevel()->getNextLevel();
  CMapLevel *lowerLevel = getCurrentlyViewedLevel()->getPrevLevel();


  if (deletedFromLevel == getCurrentlyViewedLevel())
  {
    redraw();
  }

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
    if (deletedFromLevel == upperLevel)
      redraw();

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
    if (deletedFromLevel == lowerLevel)
      redraw();
}

/** This method is called when an element is changed */
void CMapView::changedElement(CMapElement *element)
{
  if (isElementVisibale(element))
  {
    checkSize(element->getHighPos());
    redraw();
  }

  if (element == mapManager->getCurrentRoom())
    statusbar->setRoom(mapManager->getCurrentRoom()->getLabel());
}

/** This method is called when a map level is changed */
void CMapView::changedLevel(CMapLevel *level)
{
  if (isLevelVisibale(level))
  {
    int maxx=0,maxy=0;
    for (CMapElement *element=level->getFirstElement();element!=0;element=level->getNextElement())
    {      
      if (element->getHighX()>maxx)
        maxx =element->getHighX();

      if (element->getHighY()>maxx)
        maxx = element->getHighY();
    
    }

    redraw();
    checkSize(QPoint(maxx,maxy));
  }
}

/** Tell this map widget to display a different zone */
void CMapView::showPosition(QPoint pos,CMapLevel *level,bool centerView)
{
  if (!level) return;

  setLevel(level);

  CMapLevel *upperLevel = level->getNextLevel();
  CMapLevel *lowerLevel = level->getPrevLevel();

  QPoint size(0,0);

  // Calc the size the widget should be
  // FIXME_jp : Add lower level elements
  for (CMapElement *element =level->getFirstElement();element!=NULL;element=level->getNextElement())
  {
    if (element->getHighX()>size.x()) size.setX(element->getHighX());
    if (element->getHighY()>size.y()) size.setY(element->getHighY());
  }

  // FIXME_jp: Do a check to see if this needs to be done
  if (upperLevel  && mapManager->getMapData()->showUpperLevel)
  {
    for (CMapElement *element =upperLevel->getFirstElement();element!=NULL;element=upperLevel->getNextElement())
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  if (lowerLevel  && mapManager->getMapData()->showLowerLevel)
  {
    for (CMapElement *element =lowerLevel->getFirstElement();element!=NULL;element=lowerLevel->getNextElement())
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  // Resize the widget
  maxSize = QSize(0,0);
  checkSize(size);

  // Center on the position
  if (centerView)
    mapWidget->center(pos.x(),pos.y());

  // Update the status bar
  statusbar->setRoom(mapManager->getCurrentRoom()->getLabel());
  statusbar->setZone(level->getZone()->getLabel());
  statusbar->setLevel(level->getNumber());

  if (getActive())
  {
    mapManager->activeViewChanged();
  }
}

/** This is used ensure a location is visiable for views that scroll */
void CMapView::ensureVisible(QPoint pos)
{
  mapWidget->ensureVisible(pos.x(),pos.y(),10,10);
}

/** Used to calculate the correct size for the widget */
void CMapView::checkSize(QPoint pos)
{
  if (pos.x() > maxSize.width()) maxSize.setWidth(pos.x());
  if (pos.y() > maxSize.height()) maxSize.setHeight(pos.y());

  int view_x =mapWidget->viewport()->width();
  int view_y =mapWidget->viewport()->height();
  int newx;
  int newy;                                                             

  if (maxSize.width() > view_x)
  {
    newx = maxSize.width();
  }
  else
  {
    newx = view_x;
  }

  if (maxSize.height() > view_y)
  {
    newy = maxSize.height();
  }
  else
  {
    newy = view_y;
  }

  if (newy != mapWidget->contentsHeight() || newx !=mapWidget->contentsWidth())
  {
    mapWidget->resizeContents(newx + (mapManager->getMapData()->gridSize.width() * 3),newy + (mapManager->getMapData()->gridSize.height() * 3));
  }
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
  cmdFollowMode->setOn(follow);
}

/** Used to get the follow mode */
bool CMapView::getFollowMode(void)
{
  return cmdFollowMode->isOn();
}

int CMapView::getWidth(void)
{
  int width;

  if (mapWidget->contentsWidth()>mapWidget->viewport()->width())
    width = mapWidget->contentsWidth();
  else
    width = mapWidget->viewport()->width();

  return width;
}

/** Used to find out if ctrl is being pressed */
bool CMapView::getCtrlPressed(void)
{
  return bCtrlPressed;
}

int CMapView::getHeight(void)
{
  int height;

  if (mapWidget->contentsHeight()>mapWidget->viewport()->height())
    height = mapWidget->contentsHeight();
  else
    height = mapWidget->viewport()->height();

  return height;
}

/** Used to set the view to active */
void CMapView::setActive(bool active)
{
  CMapViewBase::setActive(active);
  if (active)
    lblActive->setPixmap(activeLed);
  else
       lblActive->setPixmap(inactiveLed);
}

QRect CMapView::getViewArea(void)
{
  QRect rect(mapWidget->contentsX(),mapWidget->contentsY(),mapWidget->contentsWidth(),mapWidget->contentsHeight());
  
  return rect;
}

QPixmap *CMapView::getViewportBackground(void)
{
  return mapWidget->getViewportBackground();
}

/** This is used to generate the conents of the view */
void CMapView::generateContents(void)
{
  return mapWidget->generateContents();
}


void CMapView::redraw(void)
{
  mapWidget->redraw();
}

void CMapView::setCursor ( const QCursor & cursor)
{
  if (mapWidget)
    mapWidget->setCursor(cursor);
}

/** Called when a key is pressed */
void CMapView::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) bCtrlPressed = true;

  mapManager->getCurrentTool()->keyPressEvent(e);
}

/** Called when a key is released */
void CMapView::keyReleaseEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Control) bCtrlPressed = false;

  mapManager->getCurrentTool()->keyReleaseEvent(e);
}
