/***************************************************************************
                               cmapview.h
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

#ifndef CMAPVIEW_H
#define CMAPVIEW_H

#include <qwidget.h>
#include <qlayout.h>

#include <klocale.h>

#include "cmapviewbase.h"

class QLabel;
class QScrollArea;
class QPushButton;

class CMapWidget;
class CMapManager;
class CMapElement;
class CMapLevel;
class CMapZone;
class CMapViewStatusbar;
class HVBoxLayout;



/**This widget is used as a view of the map.
  *It should be create only from the map manager.
  *@author Kmud Developer Team
  */
class CMapView : public CMapViewBase
{
   Q_OBJECT

public:
	CMapView(CMapManager *manager,QWidget *parent);

	~CMapView();

	CMapWidget *getMapWidget(void)                                   { return mapWidget; }

	// FIXME: Follow mode needs to be saved/restored somewere
	/** Set the state of the follow combo box */
	void setFollowMode(bool follow);
	/** Get the state of the follow combo box */
	bool getFollowMode(void);

	/** Tell this map widget to display a different zone */
	virtual void showPosition(QPoint pos,CMapLevel *level,bool centerView=true);
	/** This method is called when an element is added */
	virtual void addedElement(CMapElement *element);
	/** This method is called when an element is deleted */
	virtual void deletedElement(CMapLevel *deletedFromLevel);
	/** This method is called when an element is changed */
	virtual void changedElement(CMapElement *element);
	/** This method is called when a map level is changed */
	virtual void changedLevel(CMapLevel *level);
        /** This method is called when something else is changed, it should trigger a repaint */
        virtual void changed();
        /** Asks the widget to repaint itself */
        virtual void requestPaint();
	/** Get the max x cord of all elements */
	int getMaxX(void);
	/** Get the max y cord of all elements */
	int getMaxY(void);

    /* Used to calculate the correct size for the widget */
	void checkSize(QPoint pos);

	/** Used to set the view to active */
	virtual void setActive(bool active);

	/** Used to find out if ctrl is being pressed */
	virtual bool getCtrlPressed(void);
	/** This is used ensure a location is visiable for views that scroll */
	virtual void ensureVisible(QPoint pos);

	/** This will pass cursor changes to the mapWidet */
	void setCursor ( const QCursor & cursor);

protected:
	/** Used to get the width of the widget */
	virtual int getWidth(void);
	/** Used to gt the height of the widget */
	virtual int getHeight(void);
	/** Called when a key is pressed */
	virtual void keyPressEvent(QKeyEvent *e);
	/** Called when a key is released */
	virtual void keyReleaseEvent(QKeyEvent *e);
        virtual void resizeEvent (QResizeEvent *);

private:
	/** This is true if the ctrl key is being pressed */
	bool bCtrlPressed;
	HVBoxLayout *layout;
	/** The active view indicatior */
	QPixmap activeLed;
	/** The inactive view pixmap */
	QPixmap inactiveLed;
	/** The active view indicator */
	QLabel *lblActive;
	/** A toggle button to tell the map view to follow the player */
	QPushButton *cmdFollowMode;
        /** The scrolling area wrapping the map widget */
        QScrollArea *scroller;
	/** The Widget used to draw the map */
	CMapWidget *mapWidget;
	/** The status bar */
	CMapViewStatusbar *statusbar;
	/** The max size of all the components */
	QSize maxSize;
};

#endif

