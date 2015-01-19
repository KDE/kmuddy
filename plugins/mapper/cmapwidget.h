/***************************************************************************
                               cmapwidget.h
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
	
#ifndef CMAPWIDGET_H
#define CMAPWIDGET_H

#include <qwidget.h>
#include <q3scrollview.h>
#include <qtooltip.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <kaction.h>
#include <klocale.h>


class CMapManager;
class CMapElement;
class CMapZone;
class CMapLevel;
class CMapView;
class CMapViewBase;

/**This is that map widget used for displaying a view of the map
  *@author Kmud Developer Team
  */
class CMapWidget : public QWidget
{
   Q_OBJECT
public:
	CMapWidget(CMapView *view,CMapManager *manager,QWidget *parent=0);
	virtual ~CMapWidget();

	/** Used to get the views */
	CMapViewBase *getView(void);

protected:
        /** This is used to display custop tooltips. */
        bool event (QEvent *e);
	/** This is called when the mouse leaves the widget */
	void leaveEvent(QEvent *e);
	/** This is called when the mouse enters the widget */
	void enterEvent(QEvent *e);
	/** draw the map widget */
	void paintEvent(QPaintEvent *);
	/** The mouse release event */
	void mouseReleaseEvent(QMouseEvent *e);
	/** The mouse press event */
	void mousePressEvent(QMouseEvent *e);
	/** Called when the mouse is being moved */
	void mouseMoveEvent(QMouseEvent *e);

	/** Used to display the text context menu */
	void showTextContextMenu(void);
	/** Used to display the path context menu */
	void showPathContextMenu(void);
	/** Used to display the zone context menu */
	void showZoneContextMenu(void);
	/** Used to display the Room context menu */
	void showRoomContextMenu(void);

	/** Draw the map elements */
	virtual void drawElements(QPainter *p);
	/** Draw the grid if it's visible */
	virtual void drawGrid(QPainter* p);

private:
	/** Show the context menus */
	void showContexMenu(QMouseEvent *e);
	/** Used to create the element context menus */
	void initContexMenus(void);
    /** This method is used to tell the plugins a menu is about to open then open the menu */
	void popupMenu(CMapElement *element,Q3PopupMenu *menu,QPoint pos);
private:
	QPoint selectedPos;
 	/** true when the map is being moved by mouse */
 	bool bMouseDrag;
 	/** y position of last mouse drag event */
 	int nMouseDragPosY;
 	/** x position of last mouse drag event */
 	int nMouseDragPosX;
 	/** The Cursor used when move draging the map */
 	QCursor* mouseDragCursor;

	// Menus
	Q3PopupMenu *room_menu;
	Q3PopupMenu *path_menu;
	Q3PopupMenu *text_menu;
	Q3PopupMenu *zone_menu;

	/** A pointer to the map manager */
	CMapManager *mapManager;
	/** A pointer to the mapview */
	CMapView *viewWidget;
};

#endif
