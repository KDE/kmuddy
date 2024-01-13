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

#include <QWidget>
#include <QMenu>

#include <kaction.h>


class CMapManager;
class CMapElement;
class CMapLevel;
class CMapView;

/**This is that map widget used for displaying a view of the map
  *@author Kmud Developer Team
  */
class CMapWidget : public QWidget
{
   Q_OBJECT
public:
	CMapWidget(CMapView *view,CMapManager *manager,QWidget *parent=nullptr);
	~CMapWidget() override;

	/** Used to get the views */
	CMapView *getView(void);

protected:
        /** This is used to display custop tooltips. */
        bool event (QEvent *e) override;
	/** This is called when the mouse leaves the widget */
	void leaveEvent(QEvent *e) override;
	/** This is called when the mouse enters the widget */
	void enterEvent(QEvent *e) override;
	/** draw the map widget */
	void paintEvent(QPaintEvent *) override;
	/** The mouse release event */
	void mouseReleaseEvent(QMouseEvent *e) override;
	/** The mouse press event */
	void mousePressEvent(QMouseEvent *e) override;
	/** Called when the mouse is being moved */
	void mouseMoveEvent(QMouseEvent *e) override;
        /** double click event */
        void mouseDoubleClickEvent(QMouseEvent *e) override;
	/** Called when a key is pressed */
	void keyPressEvent(QKeyEvent *e) override;
	/** Called when a key is released */
	void keyReleaseEvent(QKeyEvent *e) override;

	/** Used to display the text context menu */
	void showTextContextMenu(void);
	/** Used to display the path context menu */
	void showPathContextMenu(void);
	/** Used to display the Room context menu */
	void showRoomContextMenu(void);
	/** Used to display the context menu for other / no elements */
	void showOtherContextMenu(void);
	void showContextMenu(QMenu *menu);

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
	void popupMenu(CMapElement *element,QMenu *menu,QPoint pos);
        friend class CMapView;
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
	QMenu *room_menu;
	QMenu *path_menu;
	QMenu *text_menu;
	QMenu *empty_menu;

	/** A pointer to the map manager */
	CMapManager *mapManager;
	/** A pointer to the mapview */
	CMapView *viewWidget;
};

#endif
