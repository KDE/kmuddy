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
#include "cmapelementutil.h"

#include <kdebug.h>

CMapZone::CMapZone(CMapManager *manager) : CMapElement(manager,QRect(-1,-1,0,0),nullptr)
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

  // Set the root zone if not already set
  if (!manager->getMapData()->rootZone)  // can't use getZone() here, as we'd end up in a loop
    manager->getMapData()->rootZone = this;
  manager->createLevel(UP);
}

CMapZone::~CMapZone()
{
  //FIXME_jp : when this is undone there are extra levels, this needs fixing
  // Delete the levels in the zone
  while (levelCount()) {
    kWarning() << "deleteing a zone and found levels that should already have been deleted!!";
    delete firstLevel();
  }
}

void CMapZone::setLevel(CMapLevel *level)
{
  CMapElement::setLevel(level);
}


void CMapZone::setLabel(QString zoneLabel)
{
	label = zoneLabel;
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
  return nullptr;
}

void CMapZone::setLabelPosition(labelPosTyp pos)
{
	if (getLabel()=="") pos=HIDE;

	if (pos!=HIDE)
	{
		labelPosition = pos;
	    QPoint p;

		QFont font;
		font = kapp->font();

		QFontMetrics fm(font);
		int width = fm.width(getLabel());
		int height = fm.height();

		switch (pos)
		{
			case CUSTOM    : break;
			case NORTH     : p.setX((getX()+(getWidth()/2)) -(width /2)); p.setY(getY() - height -  10); break;
			case NORTHEAST : p.setX(getHighX()+10); p.setY(getY() - height - 10); break;
			case EAST      : p.setX(getHighX()+10); p.setY((getY()+(getHeight()/2)) - (height/2)); break;
			case SOUTHEAST : p.setX(getHighX()+10); p.setY(getHighY() + 10); break;
			case SOUTH     : p.setX((getX()+(getWidth()/2)) -(width /2)); p.setY(getHighY() + 10); break;
			case SOUTHWEST : p.setX(getX()-width-10); p.setY(getHighY() + 10); break;
			case WEST      : p.setX(getX()-width-10); p.setY((getY()+(getHeight()/2)) - (height/2)); break;
			case NORTHWEST : p.setX(getX()-width-10); p.setY(getY() - height -  10); break;
                        default :
			                 return;
		}

	}
}

void CMapZone::setLabelPosition(labelPosTyp pos,CMapText *text)
{
	if (getLabel()=="" || text == nullptr) pos=HIDE;
	setLabelPosition(pos);
}

void CMapZone::geometryChanged(void)
{
	setLabelPosition(getLabelPosition());
}

void CMapZone::textRemove(void)
{
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
	properties->setAttribute("NumLevels",levelCount());
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

unsigned int CMapZone::levelCount() const
{
  return mapLevelModel.rowCount();
}

CMapLevel *CMapZone::getLevel(int idx) const
{
  QModelIndex index = mapLevelModel.index(idx, 0);
  if (!index.isValid()) return nullptr;
  CMapLevel *lvl = static_cast<CMapLevel*>(index.data(Qt::UserRole + 1).value<void *>());
  return lvl;
}

int CMapZone::levelIndex(const CMapLevel *level) const
{
  for (unsigned int i = 0; i < levelCount(); ++i)
    if (getLevel(i) == level)
      return i;
  return -1;
}

void CMapZone::addLevel(CMapLevel *level)
{
  insertLevel(level, -1);
}

void CMapZone::insertLevel(CMapLevel *level, int pos)
{
  QStandardItem *item = new QStandardItem(level->getName());
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  item->setData(QVariant(qVariantFromValue((void *) level)), Qt::UserRole + 1);

  if ((pos >= 0) && (pos < (int)levelCount()))
    mapLevelModel.insertRow(pos, item);
  else
    mapLevelModel.appendRow(item);
}

void CMapZone::removeLevel(CMapLevel *level)
{
  int idx = levelIndex(level);
  if (idx < 0) return;
  mapLevelModel.removeRows(idx, 1);
}

void CMapZone::setLevelName(CMapLevel *level, const QString &name)
{
  int idx = levelIndex(level);
  if (idx < 0) return;

  QStandardItem *item = mapLevelModel.item(idx);
  if (item && (item->text() != name)) item->setText(name);
}

