/***************************************************************************
                               cmapviewbase.cpp
                             -------------------
    begin                : Mon Mar 19 2001
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

#include "cmapviewbase.h"

#include "cmapwidget.h"
#include "cmapmanager.h"
#include "cmapzone.h"
#include "cmaplevel.h"
#include "cmappath.h"
#include "cmapelement.h"
#include "cmaproom.h"
//Added by qt3to4:
#include <QCloseEvent>
#include <QFocusEvent>
#include <kvbox.h>

CMapViewBase::CMapViewBase(CMapManager *manager,QWidget *parent, const char *name) : QWidget(parent,name)
{
  mapManager = manager;
  viewActive = false;

  setFocusPolicy(Qt::StrongFocus);

}

CMapViewBase::~CMapViewBase()
{
}

/** Used to get the currently viewed zone */
CMapZone *CMapViewBase::getCurrentlyViewedZone(void)
{
  if (currentLevel)
  {
    return currentLevel->getZone();
  }    
  else
  {
    return NULL;
  }    
}

/** Used to get the currently viewed level */
CMapLevel *CMapViewBase::getCurrentlyViewedLevel(void)
{
  return currentLevel;
}

/** Used to draw all the elments */
void CMapViewBase::drawElements(QPainter *p)
{
  if (!getCurrentlyViewedLevel())
    return;

  CMapLevel *lowerLevel = getCurrentlyViewedLevel()->getPrevLevel();
  CMapLevel *upperLevel = getCurrentlyViewedLevel()->getNextLevel();

  // Mark all paths as undrawn
  for ( CMapRoom *room =getCurrentlyViewedLevel()->getRoomList()->first();
        room!=0;
        room =getCurrentlyViewedLevel()->getRoomList()->next())
  {
    for (CMapPath *path = room->getPathList()->first();path!=0; path = room->getPathList()->next())
    {
      path->setDone(false);
    }
  }

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
  {
    for ( CMapRoom *room =lowerLevel->getRoomList()->first();
              room!=0;
               room =lowerLevel->getRoomList()->next())
    {
      for (CMapPath *path = room->getPathList()->first();path!=0; path = room->getPathList()->next())
      {
        path->setDone(false);
      }
    }
  }

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    for ( CMapRoom *room =upperLevel->getRoomList()->first();
              room!=0;
               room =upperLevel->getRoomList()->next())
    {
      for (CMapPath *path = room->getPathList()->first();path!=0; path = room->getPathList()->next())
      {
        path->setDone(false);
      }
    }
  }

  // Draw the upper map elements
  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
  {
    for ( CMapElement *element = lowerLevel->getFirstElement();
              element != NULL;
               element = lowerLevel->getNextElement())
    {
      element->lowerPaint(p,getCurrentlyViewedZone());
    }
  }

  // Paint the map elements of the current map
  for ( CMapElement *element = getCurrentlyViewedLevel()->getFirstElement();
             element != NULL;
             element = getCurrentlyViewedLevel()->getNextElement())
  {
    if (element->getDoPaint())
    {
      element->paint(p,getCurrentlyViewedZone());
    }
  }

  // Draw the upper map elements
  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    for ( CMapElement *element = upperLevel->getFirstElement();
             element != NULL;
              element = upperLevel->getNextElement())
    {
      element->higherPaint(p,getCurrentlyViewedZone());
    }
  }
}

/** Draw the grid if it's visable */
void CMapViewBase::drawGrid(QPainter* p)
{
  int x=0,y=0;
  signed int x1,x2,y1,y2;

  int maxx = getWidth();
  int maxy = getHeight();

  // Is the grid visable
  // FIXME_jp: had var for the gird
  if (mapManager->getMapData()->gridVisable)
  {
    p->setPen(mapManager->getMapData()->gridColor);

     // Draw the lines going across
    for (y=0;y<=maxy;y+=mapManager->getMapData()->gridSize.width())
    {
      x1 = 0;
      y1 = y;
      x2 = maxx;
      y2 = y;

      p->drawLine(x1,y1,x2,y2);
    }

    // Draw the lines going down
    for (x=0;x<=maxx;x+=mapManager->getMapData()->gridSize.height())
    {
      x1 = x;
      y1 = 0;
      x2 = x;
      y2 = maxy;

      p->drawLine(x1,y1,x2,y2);
    }
  }
}


/** Used to set the current level. This is for internal use */
void CMapViewBase::setLevel(CMapLevel *level)
{
  currentLevel = level;
}

/** Used to let the map manager know if it should register the focus of this widget */
bool CMapViewBase::acceptFocus(void)
{
  return true;
}

/** Tell this map widget to display a different level. view wiil
  * center on the first room */
void CMapViewBase::showPosition(CMapLevel *level,bool centerView)
{
  QPoint pos(0,0);
  CMapRoom *room =level->getRoomList()->first();
  if (room)  
  {
    pos.setX(room->getX());
    pos.setY(room->getY());
  }
  setLevel(level);
  showPosition(pos,level,centerView);
}

void CMapViewBase::showPosition(CMapRoom *room,bool centerView)
{
  QPoint pos(0,0);  
  if (room)  
  {
    pos.setX(room->getX());
    pos.setY(room->getY());
    setLevel(room->getLevel());
    showPosition(pos,room->getLevel(),centerView);
  }  
}

/** Used to find out if a level is visiable in the view */
bool CMapViewBase::isLevelVisibale(CMapLevel *level)
{
  if (getCurrentlyViewedLevel()==NULL)
    return false;

  bool found = false;

  if (level == getCurrentlyViewedLevel())
    found = true;

  if (getCurrentlyViewedLevel()->getPrevLevel()!=NULL)
    if (level == getCurrentlyViewedLevel()->getPrevLevel())
      found = true;

  if (getCurrentlyViewedLevel()->getNextLevel()!=NULL)
    if (level == getCurrentlyViewedLevel()->getNextLevel())
      found = true;

  return found;
}

/** Used to find out if a element is visiable in the view */
bool CMapViewBase::isElementVisibale(CMapElement *element)
{
  if (getCurrentlyViewedLevel()==NULL)
    return false;

  bool found = false;

  if (element->getElementType()!=PATH)
  {
    if (element->getLevel()==getCurrentlyViewedLevel())
    {
      found = true;
    }

    if (mapManager->getMapData()->showLowerLevel)
    {
      if (element->getLevel()==getCurrentlyViewedLevel()->getPrevLevel())
      {
        found = true;
      }
    }

    if (mapManager->getMapData()->showUpperLevel)
    {
      if (element->getLevel()==getCurrentlyViewedLevel()->getNextLevel())
      {
        found = true;
      }
    }

    if (element->getElementType()==ZONE)
    {
      if (element==getCurrentlyViewedZone())
      {
        found = true;
      }
    }
  }
  else
  {
    CMapPath *path = (CMapPath *)element;
    if (mapManager->getMapData()->showLowerLevel)
    {
      if (path->getSrcRoom()->getLevel() == getCurrentlyViewedLevel()->getPrevLevel())
      {
        found = true;
      }
    }

    if (mapManager->getMapData()->showUpperLevel)
    {
      if (path->getSrcRoom()->getLevel() == getCurrentlyViewedLevel()->getNextLevel())
      {
        found = true;
      }
    }

    if (path->getSrcRoom()->getLevel() == getCurrentlyViewedLevel())
    {
      found = true;
    }
  }

  return found;
}

/** Used to set the view to active */
void CMapViewBase::focusInEvent(QFocusEvent *)
{
//  if (acceptFocus())
//    mapManager->setActiveView(this);
}

void CMapViewBase::closeEvent ( QCloseEvent *)
{
}

void CMapViewBase::slotWidgetBeingClosed()
{
}

void CMapViewBase::slotDockWindowClose()
{
}
