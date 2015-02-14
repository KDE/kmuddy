/***************************************************************************
                               cmapviewbase.h
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

#ifndef CMAPVIEWBASE_H
#define CMAPVIEWBASE_H

#include <qwidget.h>

#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>
#include <QFocusEvent>

#include <kmuddy_export.h>

class CMapWidget;
class CMapLevel;
class CMapZone;
class CMapManager;
class CMapElement;
class CMapRoom;

/**This widget is the base class that all map views should inherit from.
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapViewBase : public QWidget
{
	Q_OBJECT
public:
	CMapViewBase(CMapManager *manager,QWidget *parent);
	virtual ~CMapViewBase();

	/** This method is called when an element is added */
	virtual void addedElement(CMapElement *element)=0;
	/** This method is called when an element is deleted */
	virtual void deletedElement(CMapLevel *deletedFromLevel)=0;
	/** This method is called when an element is changed */
	virtual void changedElement(CMapElement *element)=0;
	/** This method is called when a map level is changed */
	virtual void changedLevel(CMapLevel *level)=0;
        /** This method is called when something else is changed, it should trigger a repaint */
        virtual void changed()=0;
	/** Tell this map view to display a different level */
	virtual void showPosition(QPoint pos,CMapLevel *level,bool centerView=true)=0;
	/** Tell this map view to display a different level. view wiil
	  * center on the first room */
	void showPosition(CMapLevel *level,bool centerView=true);
	/** Tell this map view to display the level with a given room in it
      * @param room The room to center on
	  * @param centerView used to tell center view
	  */
	void showPosition(CMapRoom *room,bool centerView=true);
	/** Used to find the width of the widget */
	virtual int getWidth(void)=0;
	/** Used to find the height of the widget */
	virtual int getHeight(void)=0;

	/** Used to get the currently viewed zone */
	virtual CMapZone *getCurrentlyViewedZone(void);
	/** Used to get the currently viewed level */
	virtual CMapLevel *getCurrentlyViewedLevel(void);

	/** Used to set the follow mode */
	virtual void setFollowMode(bool follow) =0;
	/** Get the state of the follow combo box */
	virtual bool getFollowMode(void)=0;

	/** Used to let the map manager know if it should register the focus of this widget */
	virtual bool acceptFocus(void);

	/** Used to set the view to active */
	virtual void setActive(bool active)             { viewActive = active; }
	virtual bool getActive(void)                    { return viewActive; }

	/** Used to find out if a element is visible in the view */
	virtual bool isElementVisible(CMapElement *element);
    /** Used to find out if a level is visible in the view */
	virtual bool isLevelVisible(CMapLevel *level);

	/** This is used ensure a location is visiable for views that scroll */
	virtual void ensureVisible(QPoint)              { }

	/** This method is called when the current position of the player changes */
	virtual void playerPositionChanged(CMapRoom *) {}
	/** Used to set the current level. This is for internal use */
	virtual void setLevel(CMapLevel *level);
        virtual void requestPaint() = 0;

protected:
	virtual void slotWidgetBeingClosed();
	virtual void slotDockWindowClose();
	virtual void closeEvent ( QCloseEvent *e);
	/** Used to set the view to active */
	virtual void focusInEvent(QFocusEvent *e);

protected:
	/** A pointer to the map manager */
	CMapManager *mapManager;
	/** A pointer to the current level for this map widget */
	CMapLevel *currentLevel;
	bool viewActive;
};

#endif
