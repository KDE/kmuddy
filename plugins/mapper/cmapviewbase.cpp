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

CMapViewBase::CMapViewBase(CMapManager *manager,QWidget *parent) : QWidget(parent)
{
  mapManager = manager;
  currentLevel = 0;

  setFocusPolicy(Qt::StrongFocus);

}

CMapViewBase::~CMapViewBase()
{
}

/** Used to get the currently viewed zone */
CMapZone *CMapViewBase::getCurrentlyViewedZone(void)
{
  return currentLevel ? currentLevel->getZone() : 0;
}

/** Used to get the currently viewed level */
CMapLevel *CMapViewBase::getCurrentlyViewedLevel(void)
{
  return currentLevel;
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
  if (!level->getRoomList()->isEmpty())
  {
    CMapRoom *room = level->getRoomList()->first();
    pos.setX(room->getX());
    pos.setY(room->getY());
  }
  showPosition(pos,level,centerView);
}

void CMapViewBase::showPosition(CMapRoom *room,bool centerView)
{
  QPoint pos(0,0);  
  if (room)  
  {
    pos.setX(room->getX());
    pos.setY(room->getY());
    showPosition(pos,room->getLevel(),centerView);
  }  
}

void CMapViewBase::playerPositionChanged(CMapRoom *room)
{
  if (!room) return;
  if (room->getLevel() != currentLevel)
    showPosition(room->getLevel(), false);
}

/** Used to find out if a level is visible in the view */
bool CMapViewBase::isLevelVisible(CMapLevel *level)
{
  if (getCurrentlyViewedLevel()==NULL)
    return false;

  if (level == getCurrentlyViewedLevel())
    return true;

  if (getCurrentlyViewedLevel()->getPrevLevel())
    if (level == getCurrentlyViewedLevel()->getPrevLevel())
      return true;

  if (getCurrentlyViewedLevel()->getNextLevel())
    if (level == getCurrentlyViewedLevel()->getNextLevel())
      return true;

  return false;
}

/** Used to find out if a element is visiable in the view */
bool CMapViewBase::isElementVisible(CMapElement *element)
{
  return isLevelVisible(element->getLevel());
}

/** Used to set the view to active */
void CMapViewBase::focusInEvent(QFocusEvent *)
{
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
