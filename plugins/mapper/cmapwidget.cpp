/***************************************************************************
                               cmapwidget.cpp
                             -------------------
    begin                : Sun Mar 18 2001
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

#include "cmapwidget.h"

#include <QBitmap>
#include <Q3PopupMenu>
#include <QScrollArea>
#include <kapplication.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>


#include "cmapmanager.h"
#include "cmapzone.h"
#include "cmaplevel.h"
#include "cmappath.h"
#include "cmapcmdelementproperties.h"
#include "cmapview.h"
#include "cmaptoolbase.h"
#include "cmappluginbase.h"

static unsigned char move_bits[] = {
   0x80, 0x01, 0xc0, 0x03, 0xe0, 0x07, 0x80, 0x01, 0x80, 0x01, 0x84, 0x21,
   0x86, 0x61, 0xff, 0xff, 0xff, 0xff, 0x86, 0x61, 0x84, 0x21, 0x80, 0x01,
   0x80, 0x01, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01};

CMapWidget::CMapWidget(CMapView *view,CMapManager *manager,QWidget *parent) : QWidget(parent)
{
	// Setup vars
	viewWidget = view;
	bMouseDrag = false;
	QBitmap mouseDragCursorShape = QBitmap::fromData (QSize(16,16), move_bits);
	mouseDragCursor = new QCursor( mouseDragCursorShape, mouseDragCursorShape, -1,-1);
	mapManager = manager;

	initContexMenus();

	// FIXME_jp : set to proper size instead of test size

	// Setup scrollview
	setFocusPolicy(Qt::StrongFocus);   	
	setMouseTracking(true);
	setFocus();
}

CMapWidget::~CMapWidget()
{
	viewWidget = NULL;
}

/** Used to create the element context menus */
void CMapWidget::initContexMenus(void)
{
  room_menu = (Q3PopupMenu *)getView()->guiFactory()->container("room_popup",getView());
  text_menu = (Q3PopupMenu *)getView()->guiFactory()->container("text_popup",getView());
  path_menu = (Q3PopupMenu *)getView()->guiFactory()->container("path_popup",getView());
  empty_menu = (Q3PopupMenu *)getView()->guiFactory()->container("empty_popup",getView());
}

/** Used to get the views */
CMapView *CMapWidget::getView(void)
{
	return viewWidget;
}

/** draw the map widget */
void CMapWidget::paintEvent(QPaintEvent *ev)
{
  QPainter p(this);
  CMapZone *zone = viewWidget->getCurrentlyViewedZone();
  QColor color = zone->getUseDefaultBackground() ? mapManager->getMapData()->backgroundColor : zone->getBackgroundColor();
  p.fillRect(ev->rect(), color);

  drawGrid(&p);
  drawElements(&p);
  mapManager->getCurrentTool()->paint(&p);
}

bool CMapWidget::event(QEvent *e)
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    QPoint point = helpEvent->pos();

    CMapView *view = getView();
    CMapLevel *level = view->getCurrentlyViewedLevel();
    CMapElement *element = level ? level->findElementAt(point) : 0;
    QString s;
    if (element)
    {
      if (element->getElementType() == ROOM)
      {
        s = ((CMapRoom*)element)->getLabel();
      }
      else if (element->getElementType() == ZONE)
      {
        s = ((CMapZone*)element)->getLabel();
      }

      if (!s.trimmed().isEmpty())
        QToolTip::showText (helpEvent->globalPos(), s, this);
      else
        QToolTip::hideText ();
    }
  }
  return QWidget::event(e);
}



/** Draw the grid if it's visible */
void CMapWidget::drawGrid(QPainter* p)
{
  if (!mapManager->getMapData()->gridVisable) return;

  p->setPen(mapManager->getMapData()->gridColor);

  QSize gridSize = mapManager->getMapData()->gridSize;
  int maxx = width();
  int maxy = height();

   // Draw the lines going across
  for (int y = 0; y <= maxy; y += gridSize.width())
    p->drawLine(0, y, maxx, y);
  // Draw the lines going down
  for (int x = 0; x <= maxx; x += gridSize.height())
    p->drawLine(x, 0, x, maxy);
}

/** Used to draw all the elments */
void CMapWidget::drawElements(QPainter *p)
{
  CMapLevel *level = viewWidget->getCurrentlyViewedLevel();
  if (!level) return;

  CMapLevel *lowerLevel = level->getPrevLevel();
  CMapLevel *upperLevel = level->getNextLevel();

  // Mark all paths as undrawn
  foreach (CMapRoom *room, *level->getRoomList())
    foreach (CMapPath *path, *room->getPathList())
      path->setDone(false);

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
  {
    foreach (CMapRoom *room, *lowerLevel->getRoomList())
      foreach (CMapPath *path, *room->getPathList())
        path->setDone(false);
  }

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    foreach (CMapRoom *room, *upperLevel->getRoomList())
      foreach (CMapPath *path, *room->getPathList())
        path->setDone(false);
  }

  // Draw the upper map elements
  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
    foreach (CMapElement *element, lowerLevel->getAllElements())
      element->lowerPaint(p, viewWidget->getCurrentlyViewedZone());

  // Paint the map elements of the current map
  foreach (CMapElement *element, level->getAllElements())
    if (element->getDoPaint())
      element->paint(p, viewWidget->getCurrentlyViewedZone());

  // Draw the upper map elements
  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    foreach (CMapElement *element, upperLevel->getAllElements())
      element->higherPaint(p, viewWidget->getCurrentlyViewedZone());
  }
}


/** The mouse release event */
void CMapWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QCursor* oldCursor;

	switch (e->button())
	{
          case Qt::LeftButton:
			// Send the mouse event to the current tool
			mapManager->getCurrentTool()->mouseReleaseEvent(e->pos(),e,viewWidget->getCurrentlyViewedLevel());
			break;

          case Qt::MidButton:
			bMouseDrag = false;
			oldCursor= new QCursor(cursor());
			setCursor(*mouseDragCursor);
			delete mouseDragCursor;
			mouseDragCursor = oldCursor;
			break;

		default:
			break;
	}

}


void CMapWidget::showRoomContextMenu(void)
{
  CMapRoom *room = (CMapRoom *) getView()->getSelectedElement();

  KActionCollection *acol = getView()->actionCollection();
  QAction *roomSetCurrentPos = acol->action("roomCurrentPos");
  QAction *roomSetLogin = acol->action("roomLoginPoint");	
  KSelectAction *labelMenu=(KSelectAction *) acol->action("labelMenu");

  roomSetCurrentPos->setEnabled(!room->getCurrentRoom());
  roomSetLogin->setEnabled(!room->getLoginRoom());

  switch(room->getLabelPosition())
  {
    case CMapRoom::HIDE      : labelMenu->setCurrentItem(0); break;
    case CMapRoom::NORTH     : labelMenu->setCurrentItem(1); break;
    case CMapRoom::NORTHEAST : labelMenu->setCurrentItem(2); break;
    case CMapRoom::EAST      : labelMenu->setCurrentItem(3); break;
    case CMapRoom::SOUTHEAST : labelMenu->setCurrentItem(4); break;
    case CMapRoom::SOUTH     : labelMenu->setCurrentItem(5); break;
    case CMapRoom::SOUTHWEST : labelMenu->setCurrentItem(6); break;
    case CMapRoom::WEST      : labelMenu->setCurrentItem(7); break;
    case CMapRoom::NORTHWEST : labelMenu->setCurrentItem(8); break;
    case CMapRoom::CUSTOM    : labelMenu->setCurrentItem(9); break;
  }


  showContextMenu (room_menu);
}

void CMapWidget::showPathContextMenu(void)
{
  CMapPath *path = (CMapPath *) getView()->getSelectedElement();

  bool twoWay = path->getOpsitePath();

  KActionCollection *acol = getView()->actionCollection();
  KToggleAction *pathTwoWay = (KToggleAction *)acol->action("pathTwoWay");
  KToggleAction *pathOneWay = (KToggleAction *)acol->action("pathOneWay");
  QAction *pathEditBends = acol->action("pathEditBends");
  QAction *pathDelBend = acol->action("pathDelBend");
  QAction *pathAddBend = acol->action("pathAddBend");

  pathTwoWay->setChecked(twoWay);
  pathOneWay->setChecked(!twoWay);

  CMapView *view = (CMapView *) viewWidget;
  pathDelBend->setEnabled(path->mouseInPathSeg(selectedPos,view->getCurrentlyViewedZone())!=-1);
  pathEditBends->setEnabled(path->getBendCount() > 0);
  pathAddBend->setEnabled(path->getSrcRoom()->getZone()==path->getDestRoom()->getZone());

  showContextMenu (path_menu);
}


void CMapWidget::showTextContextMenu(void)
{
  showContextMenu (text_menu);
}

void CMapWidget::showOtherContextMenu(void)
{
  showContextMenu (empty_menu);
}

void CMapWidget::showContextMenu(Q3PopupMenu *menu)
{
  CMapView *view = mapManager->getActiveView();
  CMapElement *el = view->getSelectedElement();
  popupMenu(el, menu, selectedPos);
}

void CMapWidget::showContexMenu(QMouseEvent *e)
{
  CMapLevel *level = viewWidget->getCurrentlyViewedLevel();
  if (!level) return;

  CMapView *view = mapManager->getActiveView();
  view->setSelectedPos(e->pos());
  selectedPos = e->pos();

  view->setSelectedElement(0);
  CMapElement *element = level->findElementAt (e->pos());
  if (!element) {
    showOtherContextMenu();
    return;
  }

  view->setSelectedElement(element);

  mapManager->unsetEditElement();
  switch(element->getElementType())
  {
    case ROOM : showRoomContextMenu(); break;
    case PATH : showPathContextMenu(); break;
    case TEXT : showTextContextMenu(); break;
    default   : showOtherContextMenu(); break;
  }
}

/** This method is used to tell the plugins a menu is about to open then open the menu */
void CMapWidget::popupMenu(CMapElement *element,Q3PopupMenu *menu,QPoint pos)
{
  if (element) {
    for (CMapPluginBase *plugin = mapManager->getPluginList()->first();plugin!=0;plugin = mapManager->getPluginList()->next())
      plugin->beforeOpenElementMenu(element);
  }
  menu->popup(mapToGlobal(pos));
}

/** This is called when the mouse leaves the widget */
void CMapWidget::leaveEvent(QEvent *)
{
  // Send the mouse event to the current tool
  mapManager->getCurrentTool()->mouseLeaveEvent();
}

/** This is called when the mouse enters the widget */
void CMapWidget::enterEvent(QEvent *)
{
	// Send the mouse event to the current tool
	mapManager->getCurrentTool()->mouseEnterEvent();
}

/** The mouse press event */
void CMapWidget::mousePressEvent(QMouseEvent *e)
{
	QCursor* oldCursor;

	switch (e->button())
	{
          case Qt::RightButton:
			showContexMenu(e);
			break;

          case Qt::MidButton:
			bMouseDrag = true;
			nMouseDragPosX = e->globalX();
			nMouseDragPosY = e->globalY();
			oldCursor = new QCursor(cursor());
			setCursor(*mouseDragCursor);
			delete mouseDragCursor;
			mouseDragCursor = oldCursor;
			break;

          case Qt::LeftButton:
			// Send the mouse event to the current tool
			//p.begin(viewport());
			//p.translate(-contentsX(),-contentsY());
			mapManager->getCurrentTool()->mousePressEvent(e->pos(),e,viewWidget->getCurrentlyViewedLevel());
			//p.end();

		default:
			break;

	}
}

/** Called when the mouse is being moved */
void CMapWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (bMouseDrag)
 	{
		int dx, dy;
 		dx = e->globalX() - nMouseDragPosX;
		dy = e->globalY() - nMouseDragPosY;
		nMouseDragPosX = e->globalX();
		nMouseDragPosY = e->globalY();
		QScrollArea *parent = (QScrollArea *) parentWidget();
                QScrollBar *sx = parent->horizontalScrollBar();
                QScrollBar *sy = parent->verticalScrollBar();
                sx->setValue(sx->value() + dx*3);
                sy->setValue(sy->value() + dy*3);
	}
	else
	{
		// Send the mouse event to the current tool
		mapManager->getCurrentTool()->mouseMoveEvent(e->pos(),e->button(),viewWidget->getCurrentlyViewedLevel());
  	}
}

/** Called when a key is pressed */
void CMapWidget::keyPressEvent(QKeyEvent *e)
{
  mapManager->getCurrentTool()->keyPressEvent(e);
}

/** Called when a key is released */
void CMapWidget::keyReleaseEvent(QKeyEvent *e)
{
  mapManager->getCurrentTool()->keyReleaseEvent(e);
}


