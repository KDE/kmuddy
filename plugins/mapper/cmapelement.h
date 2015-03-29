/***************************************************************************
                               cmapelement.h
                             -------------------
    begin                : Sat Mar 10 2001
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

#ifndef CMAPELEMENT_H
#define CMAPELEMENT_H

#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
#include <qregion.h>
#include <qobject.h>
#include <qdom.h>
#include <QList>

#include "kmemconfig.h"
#include <kconfiggroup.h>

#include <kmuddy_export.h>

enum directionTyp { NORTH=0 , SOUTH=1 , WEST=2 , EAST=3 , NORTHWEST=4, NORTHEAST=5, SOUTHEAST=6, SOUTHWEST=7 , UP=8, DOWN=9, SPECIAL=30 };
//typedef enum direction_type directionTyp;

enum elementTyp { ROOM=0, PATH, TEXT, ZONE, OTHER };
//typedef enum element_type elementTyp;

/**The base class for all map elements
  *@author Kmud Developer Team
  */

class CMapZone;
class CMapManager;
class CMapLevel;

class KMUDDY_EXPORT CMapElement : public QObject
{
	Q_OBJECT
public:
	CMapElement(CMapManager *manager,CMapLevel *level);
	CMapElement(CMapManager *manager,QRect rect,CMapLevel *level);
	virtual ~CMapElement();

	// Genomerty methods
	/** Used to set the position of the top left of the element */
	void setLowPos(QPoint pos);
	QPoint getLowPos(void)                              { return position.topLeft(); }
	void setHighPos(QPoint pos)                         { position.setBottom(pos.y()); position.setRight(pos.x()); geometryChanged(); }
	QPoint getHighPos(void)                             { return position.bottomRight(); }
	void setSize(QSize size)                            { position.setSize(size); geometryChanged(); }
	void setWidth(int width)                            { position.setWidth(width); geometryChanged(); }
	void setHeight(int height)                          { position.setHeight(height); geometryChanged(); }
	QSize getSize(void)                                 { return position.size(); }
	void setRect(QRect rect)                            { position = rect; geometryChanged(); }
	QRect getRect(void)                                 { return position; }
	int getHighX()                                      { return position.x()+position.width() -1; }
	int getHighY()                                      { return position.y()+position.height() -1; }
	int getX()                                          { return position.x(); }
	int getY()                                          { return position.y(); }
	int getWidth()                                      { return position.width(); }
	int getHeight()                                     { return position.height(); }

	/** Used to load the properties of the element from a list of properties */
	virtual void loadProperties(KConfigGroup grp);
	/** Used to save the properties of the element to a list of properties */
	virtual void saveProperties(KConfigGroup grp);

	/** Used to save the element as an XML object 
	  * @param properties The XML object to save the properties too
	  * @param doc The XML Document */
	virtual void saveQDomElement(QDomDocument *doc,QDomElement *properties);
	/** Used to load the properties from a XML object
	  * @param properties The XML object to load the properties from */
	virtual void loadQDomElement(QDomElement *properties);

	virtual elementTyp getElementType(void)             { return OTHER ; }

	// Painting methods
	/** Used to paint the element to the map */
	virtual void paint(QPainter *p,CMapZone *currentZone);
	/** Used to paint the element while it is being dragged */
	virtual void dragPaint(QPoint offset,QPainter *p,CMapZone *currentZone) =0;
	/** Used to paint the lower level repesnation of the element */
	virtual void lowerPaint(QPainter *p,CMapZone *currentZone)=0;
	/** Used to paint the higher level repesnation of the element */
	virtual void higherPaint(QPainter *p,CMapZone *currentZone)=0;
	/** This is used to paint a element while it is being resized
	  * @param offset The offset of the mouse pointer since the start of the resize operation
	  * @param p The painter to paint the element too
	  * @param currentZone The current zone being viewed
	  * @param resizeId The id of the resize handle being moved */
	virtual void resizePaint(QPoint offset,QPainter *p,CMapZone *currentZone,int resizeId);

	// Mouse functions
	/** Used to find out if the mouse is in the element */
	virtual bool mouseInElement(QPoint mousePos);
	/** Used to find out if the mouse is in the resize box
	  * @param mousePos The position of the mouse pointer
	  * @param currentZone A pointer to the current zone
	  * @return the ID of the resize box, or 0 if not in any */
	virtual int mouseInResize(QPoint mousePos,CMapZone *currentZone);
	/** Used to find out if the element is in a rectangle */
	virtual bool elementIn(QRect rect,CMapZone *currentZone);

	/** This method is used to put a element in a selected state
      * @param sel The selected state to set the element too */
	virtual void setSelected(bool sel);
	/** This method is used to get the selected state of the element
      * @return The selected state of the element */
	virtual bool getSelected(void);

	/** This method is used to set the elements edit mode state */
	virtual void setEditMode(bool edit);
	/** This method is used to find out if the element is in edit mode */
	virtual bool getEditMode(void);

	/** This method is called to return a copy of the element */
	virtual CMapElement *copy(void) =0;
	/** Used to move the element relative to it's current position */
	virtual void moveBy(QPoint offset);

	/** This method is used to set the level that the element is in */
	virtual void setLevel(CMapLevel *level);
	/** This method is used to get the level that the element is in */
	CMapLevel *getLevel(void);

	CMapZone *getZone(void);
	/** This is used to resize the element */
	virtual void resize(QPoint offset,int resizeId);
	/** This is called to set if the element should be painted */
	void setDoPaint(bool paint)                         { doPaint = paint; }
	/** This is called to see if the element is to be painted */
	bool getDoPaint(void)                               { return doPaint; }

signals:
	void deleteElement(CMapElement *,bool);

protected:
	/** Used to paint the element at a given location and size
	  * @param p The painer to paint the element to
	  * @param pos The position to paint the elmenetposition
	  * @param size The size the element should be draw
	  * @param zone The current zone being viewed */
	virtual void paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *zone)=0;
	/** This method is used to calculate the positions of the resize handles */
	virtual void generateResizePositions();
	/** Used to get the map manager */
	CMapManager *getManager(void)                       { return mapManager; }
	void geometryChanged(void)                          { }
	/** This method is called when the element is put into edit mode */
	virtual void editModeSetEvent(void)                 { }
	/** This method is called when the element looses it's edit mode */
	virtual void editModeUnsetEvent(void)               { }
	/** This method is used to paint the resize handles
	  * @param p The painter used to do the painting
	  * @param resizePos The positions of the handles to be painted */
	void paintResizeHandles(QPainter *p,QList<QRect> &resizePos);
	/**
	 * This is used to read a color value from a XML object
	 * @param e The XML object
	 * @param key The key of the color to read
	 * @param defaultCol If the color is not found then this will be used
	 * @return The Color value
	 */
	static QColor readColor(QDomElement *e,QString key,QColor defaultCol);

	/**
	 * This is used to write a color value to a XML object
	 * @param doc The XML document	 
	 * @param e The XML object
	 * @param key The key of the color to write
	 * @param col The color value to write
	 */
	static void writeColor(QDomDocument *doc,QDomElement *e,QString key,QColor col);

	/**
	 * This method is used to read a int value from a XML object
	 * @param e The XML object
	 * @param key The key of the int to read
	 * @param defaultCol If the int is not found then this will be used
	 * @return The int value
	 */
	static int readInt(QDomElement *e,QString key,int i);
	
	/**
	 * This metod is used to write a int value to a XML object
	 * @param doc The XML document
	 * @param e The XML object
	 * @param key The key of the int to write
	 * @param i The int value to write
	 */
	static void writeInt(QDomDocument *doc,QDomElement *e,QString key,int i);

	/**
	 * This method is used to read a boolean value from a XML object
	 * @param e The XML object
	 * @param key The key of the boolean to read
	 * @param defaultCol If the boolean is not found then this will be used
	 * @return The boolean value
	 */
	static bool readBool(QDomElement *e,QString key,bool b);

	/**
	 * This metod is used to write a boolean value to a XML object
	 * @param doc The XML document
	 * @param e The XML object
	 * @param key The key of the boolean to write
	 * @param i The boolean value to write
	 */
	static void writeBool(QDomDocument *doc,QDomElement *e,QString key,bool b);

protected:
	/** This is used to store the cords of the resize handles */
	QList<QRect> resizePos;

private:
	/** This method is used to calc the resize handle positions for a rectangle
    * object and add them to a list
    * @param rect The coridantes of the object
    * @param resizePos The list to add the positions too */
	void generateResizePositions(QRect rect, QList<QRect> &resizePos);

	void calcResizeCords(QSize *size,QPoint *pos,signed int *offsetx,signed int *offsety,QPoint *offset,int resizeId);

private:
	bool doPaint;
	// Position attrubites
	CMapLevel *mapLevel;
	QRect position;

	// States
	bool selected,editing;
	CMapManager *mapManager;

};

#endif
