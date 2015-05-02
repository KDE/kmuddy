/****************************************************************f***********
                               cmapelement.cpp
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

#include "cmapelement.h"

#include <stdlib.h>

#include <kdebug.h>
#include <klocale.h>
#include <kconfiggroup.h>

#include "cmaplevel.h"
#include "cmapmanager.h"
#include "cmapcmdelementproperties.h"
#include "cmapviewbase.h"

#define RESIZE_SIZE         10
#define RESIZE_SIZE_HALF    5

CMapElement::CMapElement(CMapManager *manager,QRect rect,CMapLevel *level) : QObject (NULL)
{
  mapManager = manager;
  position = rect;
  mapLevel = level;
  selected = false;
  editing = false;
  doPaint = true;

  mapManager->addedElement(this);
  connect(this,SIGNAL(deleteElement(CMapElement *,bool)),manager,SLOT(deleteElement(CMapElement *,bool)));
}

CMapElement::CMapElement(CMapManager *manager,CMapLevel *level)
{
  mapManager = manager;
  mapLevel = level;
  selected = false;
  editing = false;
  doPaint = true;

  mapManager->addedElement(this);
}

CMapElement::~CMapElement()
{
  if (mapLevel)
    mapManager->getActiveView()->deletedElement(mapLevel);
}

/** This method is used to get the level that the element is in */
CMapLevel *CMapElement::getLevel(void)
{
	return mapLevel;
}

/** This method is used to set the level that the element is in */
void CMapElement::setLevel(CMapLevel *level)
{
  mapLevel = level;
  mapManager->changedElement(this);
}

void CMapElement::paint(QPainter *p,CMapZone *zone)
{
	paintElementResize(p,getLowPos(),getSize(),zone);

	// If the room is selected then draw the selection indicator
	if (getSelected())
	{
		//FIXME_jp: Move this somewere else so that it's only caled when the element position changes
		generateResizePositions();
		paintResizeHandles (p, resizePos);
	}
}

/** This method is used to paint the resize handles
  * @param p The painter used to do the painting
  * @param resizePos The positions of the handles to be painted */
void CMapElement::paintResizeHandles(QPainter *p, QList<QRect> &resizePos)
{
  p->setPen(getManager()->getMapData()->selectedColor);
  p->setBrush(getManager()->getMapData()->selectedColor);

  foreach (QRect rect, resizePos)
    p->drawRect(rect);
}

CMapZone *CMapElement::getZone(void)
{
	if (mapLevel)
		return mapLevel->getZone();
	else
		return NULL;
}

/** Used to find out if the mouse is in the element */
bool CMapElement::mouseInElement(QPoint mousePos)
{
	QRect rect = position;

	if (rect.left() == rect.right())
	{
		rect.setLeft(rect.left()-5);
		rect.setRight(rect.right()+5);
	}

	if (rect.top() == rect.bottom())
	{
		rect.setTop(rect.top()-5);
		rect.setBottom(rect.bottom()+5);
	}

	QRegion region(rect,QRegion::Rectangle);

	return region.contains(mousePos);
}

/** This method is used to set the elements edit mode state */
void CMapElement::setEditMode(bool edit)
{
	bool oldEdit = editing;
	editing = edit;

	if (oldEdit!=editing)
	{
		if (edit)
			editModeSetEvent();
		else
			editModeUnsetEvent();
	}
}

/** This method is used to find out if the element is in edit mode */
bool CMapElement::getEditMode(void)
{
	return editing;
}

/** Used to find out if the mouse is in the resize box
 * @param mousePos The position of the mouse pointer
 * @param currentZone A pointer to the current zone
 * @return the ID of the resize box, or 0 if not in any */
int CMapElement::mouseInResize(QPoint mousePos,CMapZone *)
{
	int num = 1;

	foreach (QRect rect, resizePos)
	{
		QRegion region(rect,QRegion::Rectangle);
		if (region.contains(mousePos))
			return num;
		num++;
	}

	return 0;
}

/** Used to find out if the mouse is in the resize box
  * @param mousePos The position of the mouse pointer
  * @param currentZone A pointer to the current zone
  * @return the ID of the resize box, or 0 if not in any */
bool CMapElement::elementIn(QRect rect,CMapZone *)
{
	QRegion region( rect,QRegion::Rectangle);

	return region.contains(position);
}

/** Used to move the element relative to it's current position */
void CMapElement::moveBy(QPoint offset)
{
	position.translate(offset.x(),offset.y());
}

/** This is used to paint a element while it is being resized
  * @param offset The offset of the mouse pointer since the start of the resize operation
  * @param p The painter to paint the element too
  * @param currentZone The current zone being viewed
  * @param resizeId The id of the resize handle being moved */
void CMapElement::resizePaint(QPoint offset,QPainter *p,CMapZone *currentZone,int resizeId)
{
	signed int offsetx = offset.x();
	signed int offsety = offset.y();
	QPoint pos = getLowPos();
	QSize size = getSize();

	// Calc new positions
	calcResizeCords(&size,&pos,&offsetx,&offsety,&offset,resizeId);

	// Paint the element
	paintElementResize(p,pos,size,currentZone);
	
	// Paint resize handles
	QList<QRect> tmpResizePos;
	generateResizePositions(QRect(pos,size), tmpResizePos);
	paintResizeHandles(p, tmpResizePos);
}

void CMapElement::calcResizeCords(QSize *size,QPoint *pos,signed int *offsetx,signed int *offsety,QPoint *offset,int resizeId)
{
	int gridWidth = getManager()->getMapData()->gridSize.width();
	int gridHeight = getManager()->getMapData()->gridSize.height();

	switch (resizeId)
	{
		case 1 : if (size->height()-*offsety<=0)              // NORTH
		         {
		         	offset->setY(size->height()-gridHeight);
		         	*offsety=size->height()-gridHeight;
		         }
		         if (size->height()-*offsety >= gridHeight)
		         {
		         	pos->setY(pos->y()+*offsety);
		         	if (*offsety>0)
						*size-=QSize(0,abs(*offsety));
					else
						*size+=QSize(0,abs(*offsety));
		         }
		         break;
		case 2 : if (size->height()-*offsety<=0)              // NORTHEAST
		         {
		         	offset->setY(size->height()-gridHeight);
		         	*offsety=size->height()-gridHeight;
		         }
		         if (size->height()-*offsety >= gridHeight)
		         {
		         	pos->setY(pos->y()+offset->y());
					if (*offsety>0)
						size->setHeight(size->height()-abs(*offsety));
					else
						size->setHeight(size->height()+abs(*offsety));
		         }
		         size->setWidth(size->width()+*offsetx);
		         break;
		case 3 : *size+=QSize(*offsetx,0);                    // EAST
		         break;
		case 4 : *size+=QSize(*offsetx,*offsety);             // SOUTHEAST
		         break;
		case 5 : *size+=QSize(0,*offsety);                    // SOUTH
		         break;
		case 6 : *size+=QSize(0,*offsety);                    // SOUTHWEST
		         if (size->width()-*offsetx<=0)
		         {
		         	offset->setX(size->width()-gridWidth);
		         	*offsetx = size->width()-gridWidth;
		         }
		         if (size->width()-*offsetx >= gridWidth)
		         {
		         	pos->setX(pos->x()+*offsetx);
		         	if (*offsetx>0)
		         		size->setWidth(size->width()-abs(*offsetx));
		         	else
		         		size->setWidth(size->width()+abs(*offsetx));
		         }
		
		         break;
		case 7 : if (size->width()-*offsetx<=0)               // WEST
		         {
		         	offset->setX(size->width()-gridWidth);
		         	*offsetx = size->width()-gridWidth;
		         }
		         if (size->width()-*offsetx >= gridWidth)
		         {
		         	pos->setX(pos->x()+*offsetx);
		         	if (*offsetx>0)
		         		size->setWidth(size->width()-abs(*offsetx));
		         	else
		         		size->setWidth(size->width()+abs(*offsetx));
		         }
		         break;
		case 8 : if (size->width()-*offsetx<=0)               // NORTHWEST
		         {
		         	offset->setX(size->width()-gridWidth);
		         	*offsetx = size->width()-gridWidth;
		         }
		         if (size->height()-*offsety<=0)
		         {
		         	offset->setY(size->height()-gridHeight);
		         	*offsety=size->height()-gridHeight;
		         }
		         if (size->width()-*offsetx >= gridWidth &&
                     size->height()-*offsety >= gridHeight)
		         {	
		         	*pos+=*offset;
		         	if (*offsetx>0)
		         		size->setWidth(size->width()-abs(*offsetx));
		         	else
		         		size->setWidth(size->width()+abs(*offsetx));

		         	if (*offsety>0)
		         		size->setHeight(size->height()-abs(*offsety));
		         	else
		         		size->setHeight(size->height()+abs(*offsety));
		         }
		         break;

		default : break;
	}

	if (size->width() < gridWidth)
	{
		size->setWidth(gridWidth);
	}

	if (size->height() < gridHeight)
	{
		size->setHeight(gridHeight);
	}
}

/** This is used to resize the element */
void CMapElement::resize(QPoint offset,int resizeId)
{
	QSize size = getSize();
	QPoint pos = getLowPos();
	signed int offsetx = offset.x();
	signed int offsety = offset.y();

	// Clac the new positions
	calcResizeCords(&size,&pos,&offsetx,&offsety,&offset,resizeId);


	CMapCmdElementProperties *cmd = new CMapCmdElementProperties(mapManager,i18n("Resize Element"),this);
        KConfigGroup grp = cmd->getOrgProperties();
	grp.writeEntry("X",getX());
	grp.writeEntry("Y",getY());
	grp.writeEntry("Width",getWidth());
	grp.writeEntry("Height",getHeight());
        grp = cmd->getNewProperties();
	grp.writeEntry("X",pos.x());
	grp.writeEntry("Y",pos.y());
	grp.writeEntry("Width",size.width());
	grp.writeEntry("Height",size.height());
	mapManager->addCommand(cmd);
/*
	setLowPos(pos);
	setSize(size);
*/
}

/** Used to set the position of the top left of the element */
void CMapElement::setLowPos(QPoint pos)
{
	int width = position.width();
	int height = position.height();
	position.setX(pos.x());
	position.setY(pos.y());
	position.setWidth(width);
	position.setHeight(height);

	geometryChanged();
}


/** This method is used to calculate the positions of the resize handles */
void CMapElement::generateResizePositions()
{
	generateResizePositions(getRect(), resizePos);
}

/** This method is used to calc the resize handle positions for a rectangle
   * object and add them to a list
   * @param rect The coridantes of the object
   * @param resizePos The list to add the positions too */
void CMapElement::generateResizePositions(QRect rect, QList<QRect> &resizePos)
{
	int width = rect.width();
	int height = rect.height();
	int x1 = rect.left()+1;
	int y1 = rect.top()+1;
	int x2 = rect.right();
	int y2 = rect.bottom();

	resizePos.clear();
	resizePos.append(QRect(x1+(width/2)-5,y1-4,7,7));  // NORTH
	resizePos.append(QRect(x2-4,y1-4,7,7));            // NORTHEAST
	resizePos.append(QRect(x2-4,y1+(height/2)-5,7,7)); // EAST
	resizePos.append(QRect(x2-4,y2-4,7,7));            // SOUTHEAST
	resizePos.append(QRect(x1+(width/2)-5,y2-4,7,7));  // SOUTH
	resizePos.append(QRect(x1-4,y2-4,7,7));            // SOUTHWEST
	resizePos.append(QRect(x1-4,y1+(height/2)-5,7,7)); // WEST
	resizePos.append(QRect(x1-4,y1-4,7,7));            // NORTHWEST
}

/** Used to load the properties of the element from a list of properties */
void CMapElement::loadProperties(KConfigGroup grp)
{
	QPoint lowPos  = getLowPos();
	lowPos.setX(grp.readEntry("X",getX()));
	lowPos.setY(grp.readEntry("Y",getY()));
	setLowPos(lowPos);	
	QSize size = getSize();
	size.setWidth(grp.readEntry("Width",getWidth()));
	size.setHeight(grp.readEntry("Height",getHeight()));
	setSize(size);
}

/** Used to save the properties of the element to a list of properties */
void CMapElement::saveProperties(KConfigGroup grp)
{
		grp.writeEntry("Type",(int)getElementType());
		grp.writeEntry("X",getX());
		grp.writeEntry("Y",getY());
		grp.writeEntry("Width",getWidth());
		grp.writeEntry("Height",getHeight());
		if (getZone())
			grp.writeEntry("Zone",getZone()->getZoneID());
		else
			grp.writeEntry("Zone",-1);

		CMapLevel *level = getLevel();
		if (level)
		{
			grp.writeEntry("Level",level->getLevelID());
		}
}

/** Used to save the element as an XML object
  * @param properties The XML object to save the properties too
  * @param doc The XML Document */
void CMapElement::saveQDomElement(QDomDocument *,QDomElement *properties)
{
	if (properties)
	{
		properties->setAttribute("Type",(int)getElementType());
		properties->setAttribute("X",getX());
		properties->setAttribute("Y",getY());
		properties->setAttribute("Width",getWidth());
		properties->setAttribute("Height",getHeight());

		if (getZone())
			properties->setAttribute("Zone",getZone()->getZoneID());
		else
			properties->setAttribute("Zone",-1);

		CMapLevel *level = getLevel();
		if (level)
		{
			properties->setAttribute("Level",level->getLevelID());
		}
	}
	else
	{
		kDebug() << "CMapElement::saveQDomElement - properties pointer is null";
	}
}

/** Used to load the properties from a XML object */
void CMapElement::loadQDomElement(QDomElement *properties)
{
	if (properties)
	{
		int x = properties->attribute("X",QString::number(getX())).toInt();
		int y = properties->attribute("Y",QString::number(getY())).toInt();
        setLowPos(QPoint (x,y));
		
		setWidth(properties->attribute("Width",QString::number(getWidth())).toInt());
		setHeight(properties->attribute("Height",QString::number(getHeight())).toInt());
	}
}

/** This method is used to put a element in a selected state
  * @param sel The selected state to set the element too */
void CMapElement::setSelected(bool sel)
{
  selected = sel;
  mapManager->changedElement(this);
}

/** This method is used to get the selected state of the element
  * @return The selected state of the element */
bool CMapElement::getSelected(void)
{
	return selected;
}

/**
 * This is used to read a color value from a XML object
 * @param e The XML object
 * @param key The key of the color to read
 * @param defaultCol If the color is not found then this will be used
 * @return The Color value
 */
QColor CMapElement::readColor(QDomElement *e,QString key,QColor defaultCol)
{     
	if (!e)
	{
		return defaultCol;
	}

	// Find Color element
	QDomNode n = e->namedItem(key);
	if (n.isNull())
	{
		return defaultCol;
    }

    QDomElement e2 = n.toElement();
    if (e2.isNull() )
    {
		return defaultCol;
	}

	// Get color values
	QString red = e2.attribute("Red","Not Found");
	QString green = e2.attribute("Green","Not Found");
	QString blue = e2.attribute("Blue","Not Found");

	if (red=="Not Found" || green=="Not Found" || blue=="Not Found")
	{
		return defaultCol;
	}

	QColor col(red.toInt(),green.toInt(),blue.toInt());

	return col;
}

/**
 * This is used to write a color value to a XML object
 * @param doc The XML document
 * @param e The XML object
 * @param key The key of the color to write
 * @param col The color value to write
 */
void CMapElement::writeColor(QDomDocument *doc,QDomElement *e,QString key,QColor col)
{
	QDomElement color = doc->createElement (key);
	
	color.setAttribute("Red",col.red());
	color.setAttribute("Green",col.green());
	color.setAttribute("Blue",col.blue());
	
	e->appendChild(color);
}

/**
 * This method is used to read a int value from a XML object
 * @param e The XML object
 * @param key The key of the int to read
 * @param defaultCol If the int is not found then this will be used
 * @return The int value
 */
int CMapElement::readInt(QDomElement *e,QString key,int i)
{
   return e->attribute(key,QString::number(i)).toInt();
}

/**
 * This metod is used to write a int value to a XML object
 * @param doc The XML document
 * @param e The XML object
 * @param key The key of the int to write
 * @param i The int value to write
 */
void CMapElement::writeInt(QDomDocument *,QDomElement *e,QString key,int i)
{
   e->setAttribute(key,i);
}


/**
 * This method is used to read a boolean value from a XML object
 * @param e The XML object
 * @param key The key of the boolean to read
 * @param defaultCol If the boolean is not found then this will be used
 * @return The boolean value
 */
bool CMapElement::readBool(QDomElement *e,QString key,bool b)
{
	QString defaultStr = b ? "true" : "false";
	return (e->attribute(key,defaultStr)=="true");
}

/**
 * This metod is used to write a boolean value to a XML object
 * @param doc The XML document
 * @param e The XML object
 * @param key The key of the boolean to write
 * @param b The bolean value to write
 */
void CMapElement::writeBool(QDomElement *e,QString key,bool b)
{
  e->setAttribute(key, b ? "true" : "false");
}

                                                  
