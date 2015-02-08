/***************************************************************************
                               cmapzone.cpp
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

#include "cmapzone.h"

#include <klocale.h>

#include <qfontmetrics.h>

#include "cmapmanager.h"
#include "cmaplevel.h"
#include "cmaptext.h"
#include "cmappath.h"

#include <kdebug.h>

CMapZone::CMapZone(CMapManager *manager,QRect rect,CMapLevel *level) : CMapElement(manager,rect,level)
{
  label  = i18n("Unnamed Zone");
  m_room_id_count = 0;
  m_text_id_count = 0;	
  manager->m_zoneCount ++;
  m_ID = manager->m_zoneCount;
  description = "";

  color = QColor(192,192,192);
  backgroundCol = QColor(192,192,192);
  useDefaultCol = true;
  useDefaultBackground = true;

  textRemove();

  if (level) level->getZoneList()->append(this);

  // Set the root zone if not already set
  if (!manager->getMapData()->rootZone)
    manager->getMapData()->rootZone = this;
}

CMapZone::~CMapZone()
{
  //FIXME_jp : when this is undone there are extra levels, this needs fixing
  // Delete the levels in the zone
  while (!mapLevelList.isEmpty())
  {
    kWarning() << "deleteing a zone and found levels that should already have been deleted!!";
    delete mapLevelList.first();
  }

  if (getLevel()) getLevel()->getZoneList()->removeAll(this);
  getManager()->updateZoneListCombo();
 
  if (textElement)
    getManager()->deleteElement(textElement);
}

void CMapZone::setLevel(CMapLevel *level)
{
  if (getLevel()) getLevel()->getZoneList()->remove(this);
  if (level) level->getZoneList()->append(this);
  CMapElement::setLevel(level);
}


void CMapZone::setLabel(QString zoneLabel)
{
	label = zoneLabel;
	if (textElement)
	{
		textElement->setText(zoneLabel);
	}
}

void CMapZone::dragPaint(QPoint offset,QPainter *p,CMapZone *)
{
  p->setPen(QColor(255, 255, 255, 128));
  p->setBrush(QColor(0, 255, 255, 64));
	p->drawRect(getX() + offset.x(),getY() + offset.y(),getWidth(),getHeight());
}

/** This is used to paint the sub boxes displayed when the element is painted */
void CMapZone::paintSubBox(QPainter *p,int x,int y,int width,int height)
{
	QColor background;

	if (getUseDefaultCol())
	{
		background = getManager()->getMapData()->defaultZoneColor;
	}
	else
	{
		background = getColor();
	}

	p->setPen(Qt::white);
	p->drawLine(x,y,x+width,y);
	p->drawLine(x,y,x,y+height);
	p->setPen(Qt::black);
	p->drawLine(x+1,y+height,x+width,y+height);
	p->drawLine(x+width,y+1,x+width,y+height);

	p->setBrush(background);
	p->setPen(background);

	p->drawRect(x+1,y+1,width-1,height-1);
}

/** Used to paint the element at a given location and size
  * @param p The painer to paint the element to
  * @param pos The position to paint the elmenet
  * @param size The size the element should be draw
  * @param zone The current zone being viewed */
void CMapZone::paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *)
{
	signed int y1,x1;

	x1 = pos.x()+1;
	y1 = pos.y()+1;

	int boxWidth = size.width()/2;
	int boxHeight = size.height()/2;
	int boxOffsetX = boxWidth/2-1;
	int boxOffsetY = boxHeight/2-1;
	boxWidth--;
	boxHeight--;

	paintSubBox(p,x1,y1,boxWidth,boxHeight);
	paintSubBox(p,x1+boxOffsetX,y1+boxOffsetY,boxWidth,boxHeight);
	paintSubBox(p,x1+boxOffsetX+boxOffsetX,y1+boxOffsetY+boxOffsetY,boxWidth,boxHeight);
}

void CMapZone::lowerPaint(QPainter *p,CMapZone *)
{
	signed int y1,x1;

	x1 = getX()+1-5;
	y1 = getY()+1-5;
	int boxWidth = getSize().width()/2;
	int boxHeight = getSize().height()/2;
	int boxOffsetX = boxWidth/2-1;
	int boxOffsetY = boxHeight/2-1;
	boxWidth--;
	boxHeight--;

	p->setPen(getManager()->getMapData()->lowerZoneColor);
	QBrush brush(getManager()->getMapData()->lowerZoneColor);
	brush.setStyle(Qt::Dense3Pattern);
	p->setBrush(brush);
	p->drawRect(x1,y1,boxWidth,boxHeight);
	p->drawRect(x1+boxOffsetX,y1+boxOffsetY,boxWidth,boxHeight);
	p->drawRect(x1+boxOffsetX+boxOffsetX,y1+boxOffsetY+boxOffsetY,boxWidth,boxHeight);
}

void CMapZone::higherPaint(QPainter *p,CMapZone *)
{
	signed int y1,x1;

	x1 = getX()+6;
	y1 = getY()+6;
	int boxWidth = getSize().width()/2;
	int boxHeight = getSize().height()/2;
	int boxOffsetX = boxWidth/2-1;
	int boxOffsetY = boxHeight/2-1;
	boxWidth--;
	boxHeight--;

	p->setPen(getManager()->getMapData()->higherZoneColor);
	QBrush brush(getManager()->getMapData()->higherZoneColor);
	brush.setStyle(Qt::Dense3Pattern);
	p->setBrush(brush);
	p->drawRect(x1,y1,boxWidth,boxHeight);
	p->drawRect(x1+boxOffsetX,y1+boxOffsetY,boxWidth,boxHeight);
	p->drawRect(x1+boxOffsetX+boxOffsetX,y1+boxOffsetY+boxOffsetY,boxWidth,boxHeight);
}

CMapElement *CMapZone::copy(void)
{
	// Copy all but the paths
	CMapZone *newZone = copyZone();

	// Copy the paths
	copyPaths();

	return newZone;
}

void CMapZone::copyPaths(void)
{
  foreach (CMapLevel *level, mapLevelList)
  {
    foreach (CMapZone *zone, *level->getZoneList())
      zone->copyPaths();

    foreach (CMapRoom *room, *level->getRoomList())
    {
      foreach (CMapPath *path, *room->getPathList())
      {
        CMapPath *newPath = (CMapPath *)path->copy();
        newPath->setSrcRoom(path->getSrcRoom()->getCopiedRoom());
        newPath->setDestRoom(path->getDestRoom()->getCopiedRoom());
      }
    }
  }
}

CMapZone *CMapZone::copyZone(void)
{
  CMapZone *newZone = new CMapZone(getManager(),getRect(),getLevel());
  newZone->setLabel(getLabel());
  newZone->setBackgroundColor(getBackgroundColor());
  newZone->setColor(getColor());
  newZone->setDescription(getDescription());
  newZone->setUseDefaultBackground(getUseDefaultBackground());
  newZone->setUseDefaultCol(getUseDefaultCol());

  foreach (CMapLevel *level, mapLevelList)
  {
    CMapLevel *newLevel = getManager()->createLevel(UP,newZone);

    foreach (CMapElement *el, level->getAllElements()) {
      CMapElement *el2 = el->copy();
      el2->setLevel(newLevel);
    }
  }

  //FIXME_jp: Copy text label position

  return newZone;
}

void CMapZone::setLabelPosition(labelPosTyp pos)
{
	if (getLabel()=="") pos=HIDE;

	if (pos!=HIDE)
	{
		labelPosition = pos;
	    QPoint p;

		QFont font;
		if (textElement)
		{
			font = textElement->getFont();
		}
		else
		{
			font = kapp->font();
		}

		QFontMetrics fm(font);
		int width = fm.width(getLabel());
		int height = fm.height();

		switch (pos)
		{
			case CUSTOM    : p = textElement->getLowPos(); break;
			case NORTH     : p.setX((getX()+(getWidth()/2)) -(width /2)); p.setY(getY() - height -  10); break;
			case NORTHEAST : p.setX(getHighX()+10); p.setY(getY() - height - 10); break;
			case EAST      : p.setX(getHighX()+10); p.setY((getY()+(getHeight()/2)) - (height/2)); break;
			case SOUTHEAST : p.setX(getHighX()+10); p.setY(getHighY() + 10); break;
			case SOUTH     : p.setX((getX()+(getWidth()/2)) -(width /2)); p.setY(getHighY() + 10); break;
			case SOUTHWEST : p.setX(getX()-width-10); p.setY(getHighY() + 10); break;
			case WEST      : p.setX(getX()-width-10); p.setY((getY()+(getHeight()/2)) - (height/2)); break;
			case NORTHWEST : p.setX(getX()-width-10); p.setY(getY() - height -  10); break;
			default        : if (textElement)
			                 {
			                 	getManager()->deleteElement(textElement);
		                     }
			                 textRemove();
			                 return;
		}

		if (!textElement)
		{
			textElement = getManager()->createText(p,getLevel(),getLabel());
			textElement->setLinkElement(this);
		}
		else
		{
			QRect rect;
			rect.setX(p.x());
			rect.setY(p.y());
			rect.setWidth(width);
			rect.setHeight(height);

			textElement->setRect(rect);
		}

	}
	else
	{
		if (textElement)
		{
			getManager()->deleteElement(textElement);

		}
		textRemove();
	}
}

void CMapZone::setLabelPosition(labelPosTyp pos,CMapText *text)
{
	if (getLabel()=="" || text == NULL) pos=HIDE;

	if (textElement)
	{
		getManager()->deleteElement(textElement);
    }
	textRemove();
	textElement = text;	
	textElement->setLinkElement(this);

	setLabelPosition(pos);
}

void CMapZone::geometryChanged(void)
{
	setLabelPosition(getLabelPosition());
}

void CMapZone::textRemove(void)
{
	textElement=NULL;
	labelPosition = HIDE;
}

/** Used to load the properties of the element from a list of properties */
void CMapZone::loadProperties(KConfigGroup properties)
{
	CMapElement::loadProperties(properties);
	setLabel(properties.readEntry("Label",getLabel()));
	setDescription(properties.readEntry("Description",getDescription()));

	QColor color=getColor();
	color=properties.readEntry("Color",color);
	setColor(color);
	setUseDefaultCol(properties.readEntry("DefaultColor",getUseDefaultCol()));

	QColor bgcol=getBackgroundColor();
	bgcol=properties.readEntry("BackgroundColor",bgcol);
	setBackgroundColor(bgcol);
	setUseDefaultBackground(properties.readEntry("DefaultBackground",getUseDefaultBackground()));

	setZoneID(properties.readEntry("ZoneID",m_ID));

	setLabelPosition((CMapZone::labelPosTyp)properties.readEntry("LabelPos",(int)getLabelPosition()));
}

/** Used to save the properties of the element to a list of properties */
void CMapZone::saveProperties(KConfigGroup properties)
{
	CMapElement::saveProperties(properties);
	properties.writeEntry("Label",getLabel());
	properties.writeEntry("Description",getDescription());
	properties.writeEntry("Color",getColor());
	properties.writeEntry("DefaultColor",getUseDefaultCol());
	properties.writeEntry("BackgroundColor",getBackgroundColor());
	properties.writeEntry("LabelPos",(int)getLabelPosition());
	properties.writeEntry("ZoneID",getZoneID());
}

/** Used to save the element as an XML object 
  * @param properties The XML object to save the properties too */
void CMapZone::saveQDomElement(QDomDocument *doc,QDomElement *properties)
{
	CMapElement::saveQDomElement(doc,properties);
	
	properties->setAttribute("Label",getLabel());
	properties->setAttribute("Description",getDescription());
	writeColor(doc,properties,"Color",getColor());	
	writeColor(doc,properties,"BackgroundColor",getBackgroundColor());
	properties->setAttribute("DefaultColor",getUseDefaultCol());
	properties->setAttribute("LabelPos",(int)getLabelPosition());
	properties->setAttribute("ZoneID",getZoneID());
	properties->setAttribute("NumLevels",mapLevelList.count());
	if (getUseDefaultCol())
	{
		properties->setAttribute("UseDefaultCol","true");
	}
	else
	{
		properties->setAttribute("UseDefaultCol","false");
	}

}

/** Used to load the properties from a XML object
  * @param properties The XML object to load the properties from */  
void CMapZone::loadQDomElement(QDomElement *properties)
{
	CMapElement::loadQDomElement(properties);

	setLabel(properties->attribute("Label",getLabel()));
	setDescription(properties->attribute("Description",getDescription()));	
	setUseDefaultCol(readBool(properties,"UseDefaultCol",getUseDefaultCol()));
	// setLabelPosition((labelPosTyp)readInt(properties,"LabelPos",(int)getLabelPosition()));
	setZoneID(readInt(properties,"ZoneID",getZoneID()));
	setColor(readColor(properties,"Color",getColor()));
	setBackgroundColor(readColor(properties,"BackgroundColor",getBackgroundColor()));

}


void CMapZone::setZoneID(unsigned int id)
{
	if (id > getManager()->m_zoneCount)
	  getManager()->m_zoneCount = id;

	m_ID = id;
}

void CMapZone::setUseDefaultCol(bool b)
{
	useDefaultCol = b;
}

bool CMapZone::getUseDefaultCol(void)
{
	return useDefaultCol;
}

void CMapZone::setUseDefaultBackground(bool b)
{
	useDefaultBackground = b;
}

bool CMapZone::getUseDefaultBackground(void)
{
	return true;
}

void CMapZone::setColor(QColor col)
{
	color = col;
}

QColor CMapZone::getColor(void)
{
	return color;
}

void CMapZone::setBackgroundColor(QColor col)
{
	backgroundCol = col;
}

QColor CMapZone::getBackgroundColor(void)
{
	return backgroundCol;
}

CMapLevel *CMapZone::firstLevel() const
{
  if (mapLevelList.isEmpty()) return 0;
  return mapLevelList.first();
}

