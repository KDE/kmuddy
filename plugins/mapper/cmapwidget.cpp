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

#include <qbitmap.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <Q3Frame>
#include <QPixmap>
#include <QMouseEvent>
#include <Q3PopupMenu>
#include <QEvent>
#include <kapplication.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kvbox.h>


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

CMapWidget::CMapWidget(CMapView *view,CMapManager *manager,QWidget *parent, const char *name )
	: Q3ScrollView(parent,name,Qt::WNorthWestGravity | Qt::WResizeNoErase | Qt::WRepaintNoErase)
{
	// Setup vars
	viewWidget = view;
	bMouseDrag = false;
	QBitmap mouseDragCursorShape(16,16, move_bits,TRUE);
	mouseDragCursor = new QCursor( mouseDragCursorShape, mouseDragCursorShape, -1,-1);
	mapManager = manager;
	buffer = NULL;

	initContexMenus();

	// FIXME_jp : set to proper size instead of test size


	// Setup scrollview
	setFocusPolicy(Qt::StrongFocus);   	
	setFocusProxy(parent);
	viewport()->setFocusProxy(parent);
	viewport()->setFocusPolicy(Qt::StrongFocus);
	viewport()->setMouseTracking(true);
	setMouseTracking(true);
	setFocus();
//	viewport()->setBackgroundMode(QWidget::NoBackground);
	setFrameStyle(Q3Frame::NoFrame);
	setVScrollBarMode(Q3ScrollView::AlwaysOn);
	setHScrollBarMode(Q3ScrollView::AlwaysOn);

	redraw();
}

CMapWidget::~CMapWidget()
{
	if (buffer)
		delete buffer;

	viewWidget = NULL;
}

void CMapWidget::redraw(void)
{
	update();
	viewport()->update();
}

/** Used to create the element context menus */
void CMapWidget::initContexMenus(void)
{
        room_menu = (Q3PopupMenu *)mapManager->guiFactory()->container("room_popup",mapManager);
	text_menu = (Q3PopupMenu *)mapManager->guiFactory()->container("text_popup",mapManager);
	zone_menu = (Q3PopupMenu *)mapManager->guiFactory()->container("zone_popup",mapManager);
	path_menu = (Q3PopupMenu *)mapManager->guiFactory()->container("path_popup",mapManager);	
}

/** Used to get the views */
CMapViewBase *CMapWidget::getView(void)
{
	return viewWidget;
}

/** draw the map widget */
void CMapWidget::viewportPaintEvent (QPaintEvent *)
{
	paint();
}

bool CMapWidget::event (QEvent *e)
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    QPoint point = helpEvent->pos();

    CMapViewBase *view = getView();
    QPoint pos1 = viewportToContents(point);
    CMapElement *element = mapManager->findElementAt(pos1,view->getCurrentlyViewedLevel());
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

void CMapWidget::paint()
{
	generateContents();

	bitBlt(viewport(), 0, 0, buffer);
}

void CMapWidget::generateContents(void)
{
	QRect drawArea(contentsX(), contentsY(),viewport()->width(), viewport()->height());

	// delete the buffer only when we need one with a different size
	if (buffer && (buffer->size() != drawArea.size()))
	{
		delete buffer;
		buffer = NULL;
	}

	if (!buffer)
	{
		buffer = new QPixmap(drawArea.size());
	}

	QPainter p;

	p.begin(buffer);

	p.translate(-drawArea.x(), -drawArea.y());
	
	if (viewWidget->getCurrentlyViewedZone()->getUseDefaultBackground())
	{
		p.fillRect(drawArea,mapManager->getMapData()->backgroundColor);
	}
	else
	{
		p.fillRect(drawArea,viewWidget->getCurrentlyViewedZone()->getBackgroundColor());
	}

	viewWidget->drawGrid(&p);
	viewWidget->drawElements(&p);

	p.end();
}

/** Get the background of the view port */
QPixmap *CMapWidget::getViewportBackground(void)
{
	return buffer;
}

/** The mouse release event */
void CMapWidget::viewportMouseReleaseEvent(QMouseEvent *e)
{
	int x, y;
	viewportToContents( e->x(),  e->y(), x, y );
    QCursor* oldCursor;
	//QPainter p;

	switch (e->button())
	{
          case Qt::LeftButton:
			// Send the mouse event to the current tool
			//p.begin(viewport());
			//p.translate(-contentsX(), -contentsY());
			mapManager->getCurrentTool()->mouseReleaseEvent(QPoint (x,y),viewWidget->getCurrentlyViewedLevel());
			//p.end();
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
    CMapRoom *room = (CMapRoom *)mapManager->getSelectedElement();

	QAction *roomSetCurrentPos = mapManager->actionCollection()->action("roomCurrentPos");
	QAction *roomSetLogin = mapManager->actionCollection()->action("roomLoginPoint");	
	KSelectAction *labelMenu=(KSelectAction *)mapManager->actionCollection()->action("labelMenu");

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
	

	popupMenu((CMapElement *)room,room_menu,selectedPos);
}

void CMapWidget::showZoneContextMenu(void)
{	
	CMapZone *zone = (CMapZone*)mapManager->getSelectedElement();

	KSelectAction *labelMenu= (KSelectAction *)mapManager->actionCollection()->action("labelMenu");
	
	switch(zone->getLabelPosition())
	{
		case CMapZone::HIDE      : labelMenu->setCurrentItem(0); break;
		case CMapZone::NORTH     : labelMenu->setCurrentItem(1); break;
		case CMapZone::NORTHEAST : labelMenu->setCurrentItem(2); break;
		case CMapZone::EAST      : labelMenu->setCurrentItem(3); break;
		case CMapZone::SOUTHEAST : labelMenu->setCurrentItem(4); break;
		case CMapZone::SOUTH     : labelMenu->setCurrentItem(5); break;
		case CMapZone::SOUTHWEST : labelMenu->setCurrentItem(6); break;
		case CMapZone::WEST      : labelMenu->setCurrentItem(7); break;
		case CMapZone::NORTHWEST : labelMenu->setCurrentItem(8); break;
		case CMapZone::CUSTOM    : labelMenu->setCurrentItem(9); break;
	}

	popupMenu((CMapElement *)zone,zone_menu,selectedPos);
}

void CMapWidget::showPathContextMenu(void)
{
	CMapPath *path = (CMapPath *)mapManager->getSelectedElement();
	
	bool twoWay = path->getOpsitePath();

	KToggleAction *pathTwoWay = (KToggleAction *)mapManager->actionCollection()->action("pathTwoWay");
	KToggleAction *pathOneWay = (KToggleAction *)mapManager->actionCollection()->action("pathOneWay");
    QAction *pathEditBends = mapManager->actionCollection()->action("pathEditBends");
	QAction *pathDelBend = mapManager->actionCollection()->action("pathDelBend");
	QAction *pathAddBend = mapManager->actionCollection()->action("pathAddBend");
	
	pathTwoWay->setChecked(twoWay);
	pathOneWay->setChecked(!twoWay);

	CMapView *view = (CMapView *)parentWidget();
	pathDelBend->setEnabled(path->mouseInPathSeg(viewportToContents( selectedPos ),view->getCurrentlyViewedZone())!=-1);
	pathEditBends->setEnabled(path->getBendCount() > 0);
	pathAddBend->setEnabled(path->getSrcRoom()->getZone()==path->getDestRoom()->getZone());

	popupMenu((CMapElement *)path,path_menu,selectedPos);
}


void CMapWidget::showTextContextMenu(void)
{
	CMapText *text = (CMapText *)mapManager->getSelectedElement();
	popupMenu((CMapElement *)text,text_menu,selectedPos);
}


void CMapWidget::showContexMenu(QMouseEvent *e)
{
	int x, y;
	viewportToContents( e->x(),  e->y(), x, y );

	bool found = false;

	for ( CMapElement *element = viewWidget->getCurrentlyViewedLevel()->getFirstElement();
          element != NULL;
          element = viewWidget->getCurrentlyViewedLevel()->getNextElement())
	{
		if (element->mouseInElement(QPoint(x,y),viewWidget->getCurrentlyViewedZone()))
		{
			mapManager->setSelectedElement(element);
			selectedPos = QPoint(e->x(),e->y());
			mapManager->setSelectedPos(QPoint(x,y));

			mapManager->unsetEditElement();
			switch(element->getElementType())
			{
				case ROOM : showRoomContextMenu(); break;
				case ZONE : showZoneContextMenu(); break;
				case PATH : showPathContextMenu(); break;
				case TEXT : showTextContextMenu(); break;
				default   : break;
			}

			found = true;
			break;

		}
 	}
}

/** This method is used to tell the plugins a menu is about to open then open the menu */
void CMapWidget::popupMenu(CMapElement *element,Q3PopupMenu *menu,QPoint pos)
{
	for (CMapPluginBase *plugin = mapManager->getPluginList()->first();plugin!=0;plugin = mapManager->getPluginList()->next())
	{
		plugin->beforeOpenElementMenu(element);
	}
	menu->popup(mapToGlobal(pos));
}

/** This is called when the mouse leaves the widget */
void CMapWidget::leaveEvent(QEvent *)
{
	// Send the mouse event to the current tool
	//QPainter p;
	//p.begin(viewport());
	//p.translate(-contentsX(), -contentsY());
	mapManager->getCurrentTool()->mouseLeaveEvent();
	//p.end();
}

/** This is called when the mouse enters the widget */
void CMapWidget::enterEvent(QEvent *)
{
	// Send the mouse event to the current tool
	//QPainter p;
	//p.begin(viewport());
	//p.translate(-contentsX(), -contentsY());
	mapManager->getCurrentTool()->mouseEnterEvent();
	//p.end();
}

/** The mouse press event */
void CMapWidget::viewportMousePressEvent(QMouseEvent *e)
{
	QCursor* oldCursor;
	int x, y;
	//QPainter p;
	viewportToContents( e->x(),  e->y(), x, y );

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
			mapManager->getCurrentTool()->mousePressEvent(QPoint(x,y),viewWidget->getCurrentlyViewedLevel());
			//p.end();

		default:
			break;

	}
}

/** Called when the mouse is being moved */
void CMapWidget::viewportMouseMoveEvent(QMouseEvent *e)
{
	int x, y;
	viewportToContents( e->x(),  e->y(), x, y );

	if (bMouseDrag)
 	{
		int dx, dy;
 		dx = e->globalX() - nMouseDragPosX;
		dy = e->globalY() - nMouseDragPosY;
		nMouseDragPosX += dx;
		nMouseDragPosY += dy;
		scrollBy(dx*3, dy*3);
	}
	else
	{
		// Send the mouse event to the current tool
		//QPainter p;
		//p.begin(viewport());
		//p.translate(-contentsX(), -contentsY());
		mapManager->getCurrentTool()->mouseMoveEvent(QPoint (x,y),e->button(),viewWidget->getCurrentlyViewedLevel());
		//p.end();
  	}
}

/** the resize event which has been over riddent to risze the map correctly */
void CMapWidget::resizeEvent(QResizeEvent *e)
{
	viewWidget->checkSize(QPoint(viewWidget->getMaxX(),viewWidget->getMaxY()));
	Q3ScrollView::resizeEvent(e);
}


