/***************************************************************************
                               cmaproom.cpp
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

#include "cmaproom.h"

#include <math.h>

#include <qfontmetrics.h>
#include <qregion.h>
#include <qsize.h>
#include <QDebug>

#include "cmapmanager.h"
#include "cmappath.h"
#include "cmaptext.h"
#include "cmaplevel.h"
#include "cmapelementutil.h"
#include "cmapview.h"


CMapRoom::CMapRoom(CMapManager *manager,QRect rect,CMapLevel *level) : CMapElement(manager,rect,level)
{
  color = QColor(192,192,192);
  useDefaultCol = true;
  login = false;
  label = "";
  labelPosition = HIDE;
  description = "";
  current = false;
  getZone()->m_room_id_count=getZone()->m_room_id_count+1;
  m_ID = getZone()->m_room_id_count;

  level->getRoomList()->append(this);

  textRemove();
}

CMapRoom::~CMapRoom()
{
  CMapLevel *l = getLevel();
  CMapManager *manager = getManager();

  CMapRoom *swapRoom = manager->findFirstRoom(this);
  if (l->getRoomList()->count() > 1) {
    CMapRoom *firstRoom = l->getRoomList()->first();
    CMapRoom *lastRoom = l->getRoomList()->last();
    swapRoom = (firstRoom == this) ? lastRoom : firstRoom;
  }

  if (current)
    manager->setCurrentRoom(swapRoom);

  if (login)
    manager->setLoginRoom(swapRoom);

  // Delete the paths for the room
  // First make a copy, as deleting rooms alters this list
  QList<CMapPath *> paths = pathList;
  for (CMapPath *path : paths) {
    path->setOpsitePath(nullptr);  // prevents a crash
    delete path;
  }

  // Same for paths connecting with this room
  paths = connectingPaths;
  for (CMapPath *path : paths) {
    path->setOpsitePath(nullptr);  // prevents a crash
    delete path;
  }

  l->getRoomList()->removeAll(this);

  if (textElement)
  {
    qDebug() << "CMapRoom room delete so delete text element";
    getManager()->deleteElement(textElement);
  }
}

void CMapRoom::setLevel(CMapLevel *level)
{
  if (getLevel()) getLevel()->getRoomList()->removeAll(this);
  level->getRoomList()->append(this);
  CMapElement::setLevel(level);
}

/** This is used to resize the element */
void CMapRoom::resize(QPoint offset,int resizeId)
{
  CMapElement::resize(offset,resizeId);

  for (CMapPath *path : pathList)
//    if (!path->getSelected())
      path->setCords();

  for (CMapPath *path : connectingPaths)
//    if (!path->getSelected())
      path->setCords();
}

/** Used to paint the element at a given location and size
  * @param p The painer to paint the element to
  * @param pos The position to paint the elmenet
  * @param size The size the element should be draw 
  * @param zone The current zone being viewed */
void CMapRoom::paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *)
{
	signed int y1,x1,x2,y2;

	x1 = pos.x()+1;
	y1 = pos.y()+1;
	x2 = pos.x()+size.width()-1;
	y2 = pos.y()+size.height()-1;

	// Draw the room
	p->setPen(Qt::white);
	p->drawLine(x1,y2,x1,y1);
	p->drawLine(x1,y1,x2-1,y1);
	p->setPen(Qt::black);
	p->drawLine(x1,y2,x2,y2);
	p->drawLine(x2,y2,x2,y1);


	// Findout the color to used to draw the room
	if (login)
	{
		p->setBrush(getManager()->getMapData()->loginColor);
		p->setPen(getManager()->getMapData()->loginColor);
	}
	else
	{
		if (getUseDefaultCol())
		{
			p->setBrush(getManager()->getMapData()->defaultRoomColor);
			p->setPen(getManager()->getMapData()->defaultRoomColor);

		}
		else
		{
			p->setBrush(getColor());
			p->setPen(getColor());
    }
	}

	// Draw the background of the room
	p->drawRect(x1+1,y1+1,size.width()-3,size.height()-3);

}

void CMapRoom::paint(QPainter *p,CMapZone *currentZone)
{
  // This will paint the room
  CMapElement::paint(p,currentZone);

  signed int y1,x1;

  x1 = getX()+1;
  y1 = getY()+1;

  // If this is the current room the user is in
  // then draw the symbol to show that.
  if (getCurrentRoom())
  {
    QColor curColor = getManager()->getMapData()->currentColor;
    // If the marker color is similar to the background one, use a different one to ensure that it's seen
    if ((!getUseDefaultCol()) && abs (getColor().hue() - curColor.hue()) < 30) curColor.setHsl((curColor.hue() + 180) % 360, curColor.saturation(), curColor.lightness());
    p->setPen(curColor);
    p->setBrush(curColor);
    p->drawEllipse(x1+4,y1+4,getWidth() - 9,getHeight() -9);
  }

  // Draw exits
  for (CMapPath *path : pathList)
  {
    path->paint(p, currentZone);
    if (path->getSrcDir() == UP)
    {
      p->setPen(Qt::black);
      p->setBrush(Qt::black);

      p->drawPoint(x1+4,y1+3);
      p->drawPoint(x1+3,y1+4);
      p->drawPoint(x1+4,y1+4);
      p->drawPoint(x1+5,y1+4);
    }

    if (path->getSrcDir() == DOWN)
    {
      p->setPen(Qt::black);
      p->setBrush(Qt::black);

      p->drawPoint(x1+4,y1+getHeight()-5);
      p->drawPoint(x1+3,y1+getHeight()-6);
      p->drawPoint(x1+4,y1+getHeight()-6);
      p->drawPoint(x1+5,y1+getHeight()-6);
    }

    if (path->getSrcDir() == SPECIAL)
    {
      p->setPen(getManager()->getMapData()->specialColor);
      p->setBrush(getManager()->getMapData()->specialColor);

      p->drawEllipse(x1+getWidth()-10,y1+5,5,getHeight()-10);
    }
  }
}

void CMapRoom::dragPaint(QPoint offset,QPainter *p,CMapZone *)
{
  p->setPen(QColor(255, 255, 255, 128));
  p->setBrush(QColor(0, 255, 255, 64));
	p->drawRect(getX() + offset.x(),getY() + offset.y(),getWidth(),getHeight());
}

void CMapRoom::lowerPaint(QPainter *p,CMapZone *z)
{
	signed int y1,x1;

	x1 = getX()+1-5;
	y1 = getY()+1-5;

	p->setPen(getManager()->getMapData()->lowerRoomColor);
	QBrush brush(getManager()->getMapData()->lowerRoomColor);
	brush.setStyle(Qt::Dense3Pattern);
	p->setBrush(brush);
	p->drawRect(x1,y1,getWidth()-2,getHeight()-2);

        for (CMapPath *path : *getPathList())
          path->lowerPaint(p, z);
}

void CMapRoom::higherPaint(QPainter *p,CMapZone *z)
{
	signed int y1,x1;

	x1 = getX()+6;
	y1 = getY()+6;

	p->setPen(getManager()->getMapData()->higherRoomColor);
	QBrush brush(getManager()->getMapData()->higherRoomColor);
	brush.setStyle(Qt::Dense7Pattern);
	p->setBrush(brush);
	p->drawRect(x1,y1,getWidth()-2,getHeight()-2);

        for (CMapPath *path : *getPathList())
          path->higherPaint(p, z);
}

void CMapRoom::setLabel(QString str)
{
	label = str;
	if (textElement)
	{
		textElement->setText(str);
	}
}

CMapElement *CMapRoom::copy(void)
{
	CMapRoom *room = new CMapRoom (getManager(),getRect(),getLevel());

	room->setColor(color);
	room->setDescription(description);
	room->setLabel(label);
	room->setUseDefaultCol(useDefaultCol);

	QStringList *oldContents = getContentsList();
	QStringList *newContents = room->getContentsList();

	for ( QStringList::Iterator it = oldContents->begin(); it != oldContents->end(); ++it )
		(*newContents)+=*it;

	room->setLabelPosition(getLabelPosition());

	setCopiedRoom(room);

	return room;
}

void CMapRoom::addPath (CMapPath *path)
{
	pathList.append(path);
}

CMapPath *CMapRoom::getPathDirection (directionTyp dir,QString specialCmd)
{
  for (CMapPath *path : pathList) {
    if (path->getSrcDir() != dir) continue;
    if (dir != SPECIAL) return path;
    if (path->getSpecialCmd() == specialCmd) return path;
  }
  return nullptr;
}

CMapRoom *CMapRoom::getPathTarget(directionTyp dir,QString specialCmd)
{
  CMapPath *path = getPathDirection(dir, specialCmd);
  if (!path) return nullptr;
  return path->getDestRoom();
}

directionTyp CMapRoom::bestDirectionToRoom(CMapRoom *room)
{
  int x1 = (getHighX() + getX()) / 2;
  int y1 = (getHighY() + getY()) / 2;
  int x2 = (room->getHighX() + room->getX()) / 2;
  int y2 = (room->getHighY() + room->getY()) / 2;
  CMapLevel *level1 = getLevel();
  CMapLevel *level2 = room->getLevel();

  directionTyp res = SPECIAL;
  if (level1 != level2) {
    if ((x1 != x2) || (y1 != y2)) res = ::SPECIAL;
    else if (level1->getPrevLevel() == level2) res = ::DOWN;
    else if (level1->getNextLevel() == level2) res = ::UP;
    return res;
  }

  double angle = atan2 (x2 - x1, y2 - y1) * 180 / M_PI;
  if ((angle <= 22.5) && (angle >= -22.5)) res = ::SOUTH;
  else if ((angle <= 67.5) && (angle >= 22.5)) res = ::SOUTHEAST;
  else if ((angle <= 112.5) && (angle >= 67.5)) res = ::EAST;
  else if ((angle <= 157.5) && (angle >= 112.5)) res = ::NORTHEAST;
  else if ((angle <= -22.5) && (angle >= -67.5)) res = ::SOUTHWEST;
  else if ((angle <= -67.5) && (angle >= -112.5)) res = ::WEST;
  else if ((angle <= -112.5) && (angle >= -157.5)) res = ::NORTHWEST;
  else res = ::NORTH;
  return res;
}

CMapRoom::labelPosTyp CMapRoom::getLabelPosition(void)
{
	return labelPosition;
}

void CMapRoom::setLabelPosition(labelPosTyp pos)
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
			// FIXME_jp : Needs to be configurable
			// font = kapp->font();
			font = QFont("times");
		}

		QFontMetrics fm(font);
		int width = fm.horizontalAdvance(getLabel());
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
			textElement = CMapElementUtil::createText(getManager(), p, getLevel(), getLabel());
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

void CMapRoom::setLabelPosition(labelPosTyp pos,CMapText *text)
{
	if (getLabel()=="") pos=HIDE;

	if (textElement)
	{
		getManager()->deleteElement(textElement);
    }
	textRemove();

	textElement = text;	
	textElement->setLinkElement(this);

	setLabelPosition(pos);
}

void CMapRoom::geometryChanged(void)
{
	setLabelPosition(getLabelPosition());
}

void CMapRoom::textRemove(void)
{
	textElement=nullptr;
	labelPosition = HIDE;
}

/** Used to load the properties of the element from a list of properties */
void CMapRoom::loadProperties(KConfigGroup properties)
{
	CMapElement::loadProperties(properties);

	setLabel(properties.readEntry("Label",getLabel()));

	setDescription(properties.readEntry("Description",getDescription()));
	QColor color=getColor();
	color=properties.readEntry("Color",color);

	setColor(color);
	setUseDefaultCol(properties.readEntry("DefaultColor",getUseDefaultCol()));
	setLabelPosition((CMapRoom::labelPosTyp)properties.readEntry("LabelPos",(int)getLabelPosition()));

	if (properties.hasKey("Current"))
	{
		bool current = properties.readEntry("Current",getCurrentRoom());
		setCurrentRoom(current);
	
		if (current)
		{
			getManager()->getActiveView()->playerPositionChanged(this);
		}
	}

	if (properties.hasKey("Login"))
	{
		bool login = properties.readEntry("Login",getLoginRoom());
		setLoginRoom(login);
	}
	setRoomID(properties.readEntry("RoomID",m_ID));

	if (properties.hasKey("Contents"))
	{
		*getContentsList()=properties.readEntry("Contents",QStringList());
	}
}

/** Used to this rooms current room status */
void CMapRoom::setCurrentRoom(bool currentRoom)
{
	current = currentRoom;
	if (currentRoom)
		getManager()->setCurrentRoomWithoutUndo(this);

	getManager()->changedElement(this);
}

/** Used to this rooms current room status */
void CMapRoom::setLoginRoom(bool loginRoom)
{
	login = loginRoom;
	if (loginRoom)
	{
		getManager()->setLoginRoomWithoutUndo(this);
	}

	getManager()->changedElement(this);
}

/** Used to save the properties of the element to a list of properties */
void CMapRoom::saveProperties(KConfigGroup properties)
{
	CMapElement::saveProperties(properties);
	properties.writeEntry("Label",getLabel());
	properties.writeEntry("Description",getDescription());
	properties.writeEntry("Color",getColor());
	properties.writeEntry("DefaultColor",getUseDefaultCol());
	//FIXME_jp : For some reason this was comment out, there must have reason. It needs
	//           to be there so there must be a bug somewere else now that it is uncommented
	properties.writeEntry("LabelPos",(int)getLabelPosition());
	properties.writeEntry("RoomID",getRoomID());
	properties.writeEntry("Current",getCurrentRoom());
	properties.writeEntry("Login",getLoginRoom());

	if (getContentsList()->count()>0)
		properties.writeEntry("Contents",*getContentsList());
}

/** Used to save the element as an XML object
  * @param properties The XML object to save the properties too
  * @param doc The XML Document */
void CMapRoom::saveQDomElement(QDomDocument *doc,QDomElement *properties)
{
	CMapElement::saveQDomElement(doc,properties);

	writeColor(doc,properties,"Color",getColor());
	properties->setAttribute("Label",getLabel());
	properties->setAttribute("Description",getDescription());
	properties->setAttribute("DefaultColor",getUseDefaultCol());
	properties->setAttribute("LabelPos",(int)getLabelPosition());	
	properties->setAttribute("RoomID",getRoomID());
	properties->setAttribute("Login",getLoginRoom());
	if (getLoginRoom())
	{
		properties->setAttribute("LoginRoom","true");
	}
	else
	{
		properties->setAttribute("LoginRoom","false");
	}
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
void CMapRoom::loadQDomElement(QDomElement *properties)
{
	CMapElement::loadQDomElement(properties);

	setLabel(properties->attribute("Label",getLabel()));
	setDescription(properties->attribute("Description",getDescription()));
	setUseDefaultCol(readBool(properties,"UseDefaultCol",getUseDefaultCol()));	
	setRoomID(readInt(properties,"RoomID",getRoomID()));
	setColor(readColor(properties,"Color",getColor()));
	setLoginRoom(readBool(properties,"LoginRoom",getLoginRoom()));
}

/** Used to move the element relative to it's current position */
void CMapRoom::moveBy(QPoint offset)
{
	CMapElement::moveBy(offset);

	for (CMapPath *path : pathList)
		path->setCords();

	for (CMapPath *path : connectingPaths)
		path->setCords();
}

void CMapRoom::setRoomID(unsigned int id)
{
	if (id > getZone()->m_room_id_count)
	  getZone()->m_room_id_count = id;
	  
	m_ID = id;
}

unsigned int CMapRoom::getRoomID(void)
{
	return m_ID;
}

