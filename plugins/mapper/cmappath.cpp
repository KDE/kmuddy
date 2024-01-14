/***************************************************************************
                               cmappath.cpp
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

#include "cmappath.h"

#include "cmapmanager.h"

#include <KLocalizedString>
#include <kconfiggroup.h>

#include <qregion.h>
#include <QPolygon>
#include <QDebug>

#include <math.h>

#include "cmaplevel.h"
#include "cmapcmdelementproperties.h"

CMapPath::CMapPath(CMapManager *manager,CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir, bool twoWay) : CMapElement(manager,nullptr)
{
  setSrcRoom(srcRoom);
  setDestRoom(destRoom);
  setSrcDir(srcDir);
  setDestDir(destDir);
  setCords();
  bSpecialExit = false;
  specialCmd = "";

  setLevel(srcRoom->getLevel());
  srcRoom->addPath(this);
  destRoom->getConnectingPathList()->append(this);

  // Check to see if there is a a path in the opsite directon, if so make this a two way path
  bool found = false;
  foreach (CMapPath *path, *destRoom->getPathList())
  {
    // FIXME_jp : Fix this for multiple special paths between the same rooms with different cmd's
    if (path->getDestRoom()==srcRoom &&
        path->getSrcDir() == destDir &&
        path->getDestDir() == srcDir &&
        path->getSpecialCmd() == "")
    {
      setOpsitePath(path);
      path->setOpsitePath(this);
      found = true;
    }
  }
  if (twoWay && (!found)) makeTwoWay();

  beforeCommand = "";
  afterCommand = "";
  done = false;
  opsitePath = nullptr;

  m_dontPaintBend=0;
}

CMapPath::CMapPath(CMapManager *manager,CMapRoom *srcRoom,CMapRoom *destRoom)  : CMapElement(manager,nullptr)
{
	setSrcRoom(srcRoom);
	setDestRoom(destRoom);
	setSrcDir(NORTH);
	setDestDir(SOUTH);

	beforeCommand = "";
	afterCommand = "";
	bSpecialExit = false;
	specialCmd = "";
	done = false;
	opsitePath = nullptr;
}

CMapPath::~CMapPath()
{
  if (opsitePath) {
    opsitePath->setOpsitePath(nullptr);  // needed to prevent endless looping
    delete opsitePath;
  }

  if (destRoom)
    destRoom->getConnectingPathList()->removeAll(this);
  if (srcRoom)
    srcRoom->getPathList()->removeAll(this);
}

void CMapPath::setDontPaintBend(int bend)
{
	m_dontPaintBend = bend;
}

/** This method is used to set the cords of the path */
void CMapPath::setCords(void)
{
	QPoint pos1,pos2;
	CMapRoom *srcRoom = getSrcRoom();
	CMapRoom *destRoom = getDestRoom();
	directionTyp srcDir = getSrcDir();
	directionTyp destDir = getDestDir();

	QPoint start;
	QSize halfRoomSize;

	start.setX(srcRoom->getX() + (srcRoom->getWidth() /  2));
	start.setY(srcRoom->getY() + (srcRoom->getHeight() /  2));

	halfRoomSize = QSize(srcRoom->getSize().width()/2,srcRoom->getSize().height()/2);
	getManager()->directionToCord(srcDir,halfRoomSize,&pos1);

	pos1+=start;

	QPoint end;

	end.setX(destRoom->getX() + (destRoom->getWidth() /  2));
	end.setY(destRoom->getY() + (destRoom->getHeight() /  2));
	halfRoomSize = QSize(destRoom->getSize().width()/2,destRoom->getSize().height()/2);

	getManager()->directionToCord(destDir,halfRoomSize,&pos2);

	pos2+=end;

	QRect rect(pos1,pos2);
	setRect(rect);
        setDone(false);
}

/** Used to draw an arrow head */
void CMapPath::drawArrow(directionTyp dir,QPainter *p,QPoint end)
{
	int x1=0,x2=0,y1=0,y2=0;
	QPolygon cords(3);

	switch (dir)
	{
		case NORTH     : x1=end.x()-3; x2=end.x()+3; y1=end.y()-3; y2=end.y()-3; break;
		case SOUTH     : x1=end.x()-3; x2=end.x()+3; y1=end.y()+3; y2=end.y()+3; break;
		case WEST      : x1=end.x()-3; x2=end.x()-3; y1=end.y()-3; y2=end.y()+3; break;
		case EAST      : x1=end.x()+3; x2=end.x()+3; y1=end.y()-3; y2=end.y()+3; break;
		case NORTHEAST : x1=end.x(); x2=end.x()+4; y1=end.y()-4; y2=end.y(); break;
		case SOUTHEAST : x1=end.x(); x2=end.x()+4; y1=end.y()+4; y2=end.y(); break;
		case SOUTHWEST : x1=end.x(); x2=end.x()-4; y1=end.y()+4; y2=end.y(); break;
		case NORTHWEST : x1=end.x(); x2=end.x()-4; y1=end.y()-4; y2=end.y(); break;
		case UP		   : break;
		case DOWN      : break;
		case SPECIAL   : break;
	}

	// Draw the arrow head to indicate the direction of the path
	cords.setPoint(0,end);
	cords.setPoint(1,x1,y1);
	cords.setPoint(2,x2,y2);
        p->drawPolygon(cords.constData(), 3, Qt::OddEvenFill);
}

QPoint CMapPath::getIndent(directionTyp dir,QPoint pos)
{
	int x=0,y=0;
	int posx = pos.x();
	int posy = pos.y();

	switch (dir)
	{
		case NORTH     : x = posx  ; y=posy-5; break;
		case SOUTH     : x = posx  ; y=posy+5; break;
		case WEST      : x = posx-5; y=posy  ; break;
		case EAST      : x = posx+5; y=posy  ; break;
		case NORTHEAST : x = posx+5; y=posy-5; break;
		case SOUTHEAST : x = posx+5; y=posy+5; break;
		case SOUTHWEST : x = posx-5; y=posy+5; break;
		case NORTHWEST : x = posx-5; y=posy-5; break;
		case UP		   : break;
		case DOWN      : break;
		case SPECIAL   : break;
	}

	return QPoint(x,y);
}

/** This method is used to calcualte the distance from a path segment */
int CMapPath::getDistance (int x,int y,int x1,int x2,int y1,int y2)
{
	int a = y1 - y2;
	int b = -(x1 - x2);

	int d = (int)(( (x - x2) * a + (y-y2) * b) / sqrt (a*a + b*b));

	return abs(d);
}

/** Set a pointer to the destination room */
void CMapPath::setDestRoom(CMapRoom *DestRoom)
{
	destRoom = DestRoom;
}

/** Set a pointer to the source room */
void CMapPath::setSrcRoom(CMapRoom *SrcRoom)
{
	srcRoom = SrcRoom;
}

/** Set the start and finish directrions of the path */
void CMapPath::setSrcDir(directionTyp SrcDir)
{
	srcDir = SrcDir;
}

void CMapPath::setDestDir(directionTyp DestDir)
{
	destDir = DestDir;
}

void CMapPath::makeOneWay() {
  CMapPath *path = getOpsitePath();
  if (!path) return;
  path->setOpsitePath(nullptr);  // so it doesn't delete us too
  delete path;
}

void CMapPath::makeTwoWay()
{
  if (opsitePath) return;

  CMapPath *newPath = new CMapPath(getManager(), destRoom, destDir, srcRoom, srcDir, false);
  setOpsitePath(newPath);
  newPath->setOpsitePath(this);
  if (bSpecialExit)
    newPath->setSpecialCmd(specialCmd);
}

directionTyp CMapPath::generatePath()
{
	tempPathCords.clear();

	QPoint start = getLowPos();
        QPoint end = getHighPos();

        directionTyp destDir = getDestDir();

	QPoint indent1 = getIndent(getSrcDir(),start);

	QPoint indent2 = getIndent(destDir,end);

    tempPathCords.append(start);
	tempPathCords.append(indent1);

	for( PointList::Iterator point = bendList.begin(); point != bendList.end(); ++point )
	{
		tempPathCords.append(*point);
	}

	tempPathCords.append(indent2);
	tempPathCords.append(end);

	return destDir;
}

/** Used to draw the path */
void CMapPath::drawPath(QPainter *p,CMapZone *zone,QPoint offset,QColor color,int editBox,QPoint editPos)
{
	directionTyp tempDestDir = generatePath();

	p->setPen(color);
	p->setBrush(color);

	// If the path has not already been then draw it
	if (!getDone())
	{
		if (tempPathCords.count()>1)
		{
			QPoint temp = tempPathCords.at(1);

			int count = 0;

			for( PointList::Iterator point = tempPathCords.begin(); point != tempPathCords.end(); ++point )
			{
				
				if (editBox > 0 && count==editBox + 1)
				{
					p->drawLine(temp + offset,editPos + offset);
				}
				else if (editBox > 0 && count==editBox + 2)
				{
					p->drawLine(editPos + offset,(*point) + offset);
				}
				else
				{
					if (editBox == 0 && m_dontPaintBend == 0)
					{
						p->drawLine(temp + offset,(*point) + offset);
					}
					else
					{
						if (count != m_dontPaintBend+1 && count != m_dontPaintBend + 2)
						{
							p->drawLine(temp + offset,(*point) + offset);
						}
					}
				}
				temp = *point;
				count++;
			}
		}

		if (getEditMode())
		{
			int count = 0;
			p->setPen(getManager()->getMapData()->editColor);
			p->setBrush(getManager()->getMapData()->editColor);
			for( PointList::Iterator point = tempPathCords.begin(); point != tempPathCords.end(); ++point )
			{
				if (count>1 && count < (int)tempPathCords.count()-2)
				{
					if (editBox > 0 && count==editBox + 2)
					{
						p->drawRect(editPos.x()-3,editPos.y()-3,6,6);
					}
					else
					{
						if (editBox == 0 && m_dontPaintBend == 0)
						{
							p->drawRect((*point).x()-3,(*point).y()-3,6,6);
						}
						else
						{
							if (count != m_dontPaintBend+1)
							{
								p->drawRect((*point).x()-3,(*point).y()-3,6,6);	
							}
						}
					}
				}
				count ++;
			}
		}

	}

	p->setPen(color);
	p->setBrush(color);

	// Draw the arrow head of the path
	drawArrow(tempDestDir,p,tempPathCords.last() + offset);

	if (getDestRoom()->getZone()!=zone)
	{
		if (getOpsitePath())
			drawArrow(getSrcDir(),p,tempPathCords.first() + offset);
	}

	// Mark this path and the opsite (two way path) as having been drawn
	setDone(true);

	if (getOpsitePath())
	{
		getOpsitePath()->setDone(true);
	}
}



/** Used to paint the element at a given location and size
  * @param p The painter to paint the element to
  * @param pos The position to paint the elmenet
  * @param size The size the element should be draw
  * @param zone The current zone being viewed */
void CMapPath::paintElementResize(QPainter *p,QPoint,QSize,CMapZone *zone)
{
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;

	// Get start and end points of the path
	drawPath(p,zone,QPoint(0,0),getManager()->getMapData()->defaultPathColor);

	if (getSelected())
	{
		p->setPen(getManager()->getMapData()->selectedColor);
		p->setBrush(getManager()->getMapData()->selectedColor);
		p->drawRect(tempPathCords.first().x()-3,tempPathCords.first().y()-3,6,6);
		p->drawRect(tempPathCords.last().x()-3,tempPathCords.last().y()-3,6,6);
	}
}

/** Used to find out if the mouse is in the element */
bool CMapPath::mouseInElement(QPoint mousePos)
{
	if (srcDir == UP || srcDir == DOWN || srcDir==SPECIAL) return false;

	//FIXME_jp: Handle zone paths that have been termintated
	generatePath();

	if (tempPathCords.count()>1)
	{
		QPoint temp = tempPathCords.at(1);

		for( PointList::Iterator point = tempPathCords.begin(); point != tempPathCords.end(); ++point )
		{		
			QPoint pos1 = temp;
			QPoint pos2 = *point;

			QRect rect = getBoundRectOfSeg(pos1,pos2);

			QRegion region(rect,QRegion::Rectangle);

			if (region.contains(mousePos))
			{
				int distance = getDistance(mousePos.x(),mousePos.y(),pos1.x(),pos2.x(),pos1.y(),pos2.y());

				return (distance <= 5);
			}

			temp = *point;
		}
	}
	return false;
}

QRect CMapPath::getBoundRectOfSeg(QPoint pos1,QPoint pos2)
{
	QRect rect;

	if (pos1.x() == pos2.x())
	{
		pos1.setX(pos1.x()-5);
		pos2.setX(pos2.x()+5);
	}

	if (pos1.y() == pos2.y()){

		pos1.setY(pos1.y()-5);
		pos2.setY(pos2.y()+5);
	}

    if (pos1.y()>pos2.y())
	{
		rect.setBottom(pos1.y());
		rect.setTop(pos2.y());
	}
	else
	{
		rect.setTop(pos1.y());
		rect.setBottom(pos2.y());
	}

	if (pos1.x()>pos2.x())
	{
		rect.setRight(pos1.x());
		rect.setLeft(pos2.x());
	}
	else
	{
		rect.setLeft(pos1.x());
		rect.setRight(pos2.x());
	}


	return rect;
}

void CMapPath::paint(QPainter *p,CMapZone *zone)
{
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;

	CMapElement::paint(p,zone);
}

void CMapPath::resizePaint(QPoint,QPainter *,CMapZone *,int)
{
}

void CMapPath::editPaint(QPoint pos,QPainter *p,CMapZone *zone,int editBox)
{	
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;	

	setDone(false);
	drawPath(p,zone,QPoint (0,0),getManager()->getMapData()->defaultPathColor,editBox,pos);
}


void CMapPath::dragPaint(QPoint offset,QPainter *p,CMapZone *zone)
{
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;

	if (getSrcRoom()->getSelected() && getDestRoom()->getSelected())
	{
		drawPath(p,zone,offset,getManager()->getMapData()->defaultPathColor);
	}
}

void CMapPath::lowerPaint(QPainter *p,CMapZone *zone)
{
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;

	// Get start and end points of the path
	drawPath(p,zone,QPoint(-5,-5),getManager()->getMapData()->lowerPathColor);
}

void CMapPath::higherPaint(QPainter *p,CMapZone *zone)
{
	if (srcDir == UP || srcDir == DOWN || destDir == UP || destDir == DOWN || srcDir==SPECIAL || destDir==SPECIAL) return;

	// Get start and end points of the path
	drawPath(p,zone,QPoint(5,5),getManager()->getMapData()->higherPathColor);
}

/** Used to create a deep copy of the path */
CMapElement *CMapPath::copy(void)
{
	CMapPath *path = new CMapPath(getManager(),getSrcRoom(),getSrcDir(),getDestRoom(),getDestDir(),getOpsitePath());

	path->setAfterCommand(getAfterCommand());
	path->setBeforeCommand(getBeforeCommand());
	path->setSpecialCmd(getSpecialCmd());
	path->setSpecialExit(getSpecialExit());

	for( PointList::Iterator bend = bendList.begin(); bend != bendList.end(); ++bend )
	{
		path->addBend(*bend);
	}

	return path;
}

/** Used to add a bend to the path */
int CMapPath::addBend(QPoint pos)
{
    if (bendList.contains(pos))
		return -1;

	if (bendList.count()==0)
	{
		bendList.append(pos);
		return bendList.count()-1;
	}
	else
	{
		int x1 = getX();
		int y1 = getY();
		int x2 = getHighX();
		int y2 = getHighY();

		QPoint pos1 = getIndent(getSrcDir(),QPoint (x1,y1));

		x1=pos1.x(),y1=pos1.y();
		int count = 0;

		for( PointList::Iterator point = bendList.begin(); point != bendList.end(); ++point )
		{
			x2 = (*point).x();
			y2 = (*point).y();
			if (x1 == x2)
			{
 				x1-=3;
				x2+=3;
			}

			if (y1 == y2)       	
			{
				y1-=3;
				y2+=3;
			}

			int x1_1 = x1 < x2 ? x1 : x2;
			int y1_1 = y1 < y2 ? y1 : y2;
			int x2_1 = x1 >= x2 ? x1 : x2;
			int y2_1 = y1 >= y2 ? y1 : y2;

			QRegion r(x1_1,y1_1,x2_1-x1_1,y2_1-y1_1);

			if (r.contains(pos))
			{
				bendList.insert(count, pos);
				return count;
			}
			x1 = (*point).x();
			y1 = (*point).y();
			count++;
		}
		bendList.append(pos);
		return bendList.count()-1;
	}
}

/** Used to add a bend to the path */
void CMapPath::addBendWithUndo(QPoint pos)
{
    if (bendList.contains(pos))
		return;

	CMapCmdElementProperties *cmdAddBend = new CMapCmdElementProperties(getManager(),i18n("Add Bend"),this);
	cmdAddBend->getNewProperties().writeEntry("AddBend",pos);
	cmdAddBend->getOrgProperties().writeEntry("DelBend",pos);
	getManager()->addCommand(cmdAddBend);
}

/** Used to move a bend to a new position */
void CMapPath::moveBendWithUndo(int bend, QPoint pos)
{
	if (bend>0 && bend-1 < (int)bendList.count())
	{
		QPoint oldPos = bendList.at(bend-1);
		CMapCmdElementProperties *cmdMoveBend = new CMapCmdElementProperties(getManager(),i18n("Move Bend"),this);
		cmdMoveBend->getNewProperties().writeEntry("MoveBendPos",pos);
		cmdMoveBend->getNewProperties().writeEntry("MoveBendBend",bend);
		cmdMoveBend->getOrgProperties().writeEntry("MoveBendPos",oldPos);	
		cmdMoveBend->getOrgProperties().writeEntry("MoveBendBend",bend);
		getManager()->addCommand(cmdMoveBend);
	}
}

/** This method is used to delete bends from a path
  * @param seg The path segment number to delete */
void CMapPath::deletePathSegWithUndo(int seg)
{
	QPoint bend = deletePathSeg(seg);

	CMapCmdElementProperties *cmdDeleteBend = new CMapCmdElementProperties(getManager(),i18n("Delete Path Segment"),this);
	cmdDeleteBend->getNewProperties().writeEntry("DelBend",bend);
	cmdDeleteBend->getOrgProperties().writeEntry("AddBend",bend);
	getManager()->addCommand(cmdDeleteBend);
}

void CMapPath::deleteBend(QPoint bend)
{
	bendList.removeAll(bend);
}

/** Used to load the properties of the element from a list of properties */
void CMapPath::loadProperties(KConfigGroup grp)
{
	CMapElement::loadProperties(grp);

	setAfterCommand(grp.readEntry("SrcAfterCommand",getAfterCommand()));
	setBeforeCommand(grp.readEntry("SrcBeforeCommand",getBeforeCommand()));
	setSpecialCmd(grp.readEntry("SpecialCmdSrc",getSpecialCmd()));
	setSpecialExit(grp.readEntry("SpecialExit",getSpecialExit()));
	setSrcDir((directionTyp)grp.readEntry("SrcDir",(int)getSrcDir()));
	setDestDir((directionTyp)grp.readEntry("DestDir",(int)getDestDir()));
	setCords();

        if (grp.readEntry("PathTwoWay", getOpsitePath() != nullptr))
          makeTwoWay();
        else
          makeOneWay();

        CMapPath *opposite = getOpsitePath();
	if (opposite)
	{
		opposite->setAfterCommand(grp.readEntry("DestAfterCommand",opposite->getAfterCommand()));
		opposite->setBeforeCommand(grp.readEntry("DestBeforeCommand",opposite->getBeforeCommand()));
		opposite->setSrcDir((directionTyp)grp.readEntry("DestDir",(int)opposite->getSrcDir()));
		opposite->setDestDir((directionTyp)grp.readEntry("SrcDir",(int)opposite->getDestDir()));
		opposite->setSpecialCmd(grp.readEntry("SpecialCmdDest",opposite->getSpecialCmd()));
		opposite->setSpecialExit(grp.readEntry("SpecialExit",opposite->getSpecialExit()));
		opposite->setCords();
	}

	if (grp.hasKey("AddBend"))
	{
		QPoint bend(0,0);
		bend = grp.readEntry("AddBend",bend);
		addBend(bend);
	}

	if (grp.hasKey("DelBend"))
	{
		QPoint bend(0,0);
		bend = grp.readEntry("DelBend",bend);
		deleteBend(bend);
	}

	if (grp.hasKey("MoveBendBend"))
	{
		QPoint pos(0,0);
		pos = grp.readEntry("MoveBendPos",pos);
		int bend = grp.readEntry("MoveBendBend",-1);
		moveBend(bend,pos);
	}

	if (grp.hasKey("DeletePathSeg"))
	{
		int seg = grp.readEntry("DeletePathSeg", 0);
		deletePathSeg(seg);
	}
}

/** Used to save the properties of the element to a list of properties */
void CMapPath::saveProperties(KConfigGroup grp)
{
	// FIXME_jp : Make sure twoway paths are handled correctly
	CMapElement::saveProperties(grp);

	grp.writeEntry("SrcAfterCommand",getAfterCommand());
	grp.writeEntry("SrcBeforeCommand",getBeforeCommand());
	grp.writeEntry("SpecialCmdSrc",getSpecialCmd());
	grp.writeEntry("SpecialExit",getSpecialExit());
	grp.writeEntry("SrcDir",(int)getSrcDir());
	grp.writeEntry("DestDir",(int)getDestDir());	
	grp.writeEntry("SrcRoom",getSrcRoom()->getRoomID());
	grp.writeEntry("SrcZone",getSrcRoom()->getZone()->getZoneID());
	grp.writeEntry("SrcLevel",getSrcRoom()->getLevel()->getLevelID());
	grp.writeEntry("DestRoom",getDestRoom()->getRoomID());
	grp.writeEntry("DestZone",getDestRoom()->getZone()->getZoneID());
	grp.writeEntry("DestLevel",getDestRoom()->getLevel()->getLevelID());

	if (getOpsitePath())
	{
		grp.writeEntry("PathTwoWay",true);
		grp.writeEntry("DestAfterCommand",getOpsitePath()->getAfterCommand());
		grp.writeEntry("DestBeforeCommand",getOpsitePath()->getBeforeCommand());
		grp.writeEntry("SpecialCmdDest",getOpsitePath()->getSpecialCmd());
	}
	else
	{
		grp.writeEntry("PathTwoWay",false);
	}
}

/** Used to save the element as an XML object
  * @param properties The XML object to save the properties too 
  * @param doc The XML Document */	
void CMapPath::saveQDomElement(QDomDocument *doc,QDomElement *properties)
{
	CMapElement::saveQDomElement(doc,properties);

    properties->setAttribute("AfterCommand",getAfterCommand());
	properties->setAttribute("BeforeCommand",getBeforeCommand());
	properties->setAttribute("SpecialCmd",getSpecialCmd());
	writeBool(properties,"SpecialExit",getSpecialExit());
	writeInt(doc,properties,"SrcDir",(int)getSrcDir());
	writeInt(doc,properties,"DestDir",(int)getDestDir());
	writeInt(doc,properties,"SrcRoom",getSrcRoom()->getRoomID());
	writeInt(doc,properties,"SrcZone",getSrcRoom()->getZone()->getZoneID());
	writeInt(doc,properties,"SrcLevel",getSrcRoom()->getLevel()->getLevelID());
	writeInt(doc,properties,"DestRoom",getDestRoom()->getRoomID());
	writeInt(doc,properties,"DestZone",getDestRoom()->getZone()->getZoneID());
	writeInt(doc,properties,"DestLevel",getDestRoom()->getLevel()->getLevelID());

    QDomElement bendsNode = doc->createElement ("bends");
    properties->appendChild(bendsNode);

	for( PointList::Iterator bend = bendList.begin(); bend != bendList.end(); ++bend )
	{
		QDomElement bendNode = doc->createElement ("bend");
       
		writeInt(doc,&bendNode,"X",(*bend).x());
		writeInt(doc,&bendNode,"Y",(*bend).y());        
		bendsNode.appendChild(bendNode);
	}
}

/** Used to load the properties from a XML object
  * @param properties The XML object to load the properties from */
void CMapPath::loadQDomElement(QDomElement *properties)
{
	CMapElement::loadQDomElement(properties);
	setAfterCommand(properties->attribute("AfterCommand",getAfterCommand()));
	setBeforeCommand(properties->attribute("BeforeCommand",getAfterCommand()));
    setSpecialCmd(properties->attribute("SpecialCmd",getSpecialCmd()));
	setSpecialExit(readBool(properties,"SpecialExit",getSpecialExit()));
	setCords();  // fix the coords

	QDomNode n = properties->namedItem("bends");
	if (!n.isNull())
	{
		QDomElement bendsNode = n.toElement();
		if (!bendsNode.isNull() )
		{
			QDomNode n1 = bendsNode.firstChild();
			while (!n1.isNull() )
			{
				QDomElement bendNode = n1.toElement();

				if (!bendNode.isNull() )
				{
					int x=readInt(&bendNode,"X",-1);
					int y=readInt(&bendNode,"X",-1);

					if (x!=-1 && y!=-1)
					{
						addBend(QPoint(x,y));
					}
				}

				n1 = n1.nextSibling();
			}
		}
	}

	//TODO_jp : write path bends

}

void CMapPath::moveBy(QPoint offset)
{
	if (getSrcRoom()->getSelected() && getDestRoom()->getSelected())
	{
		
		for( PointList::Iterator point = bendList.begin(); point != bendList.end(); ++point )
		{
			(*point)+=offset;
		}
	}
}

/** This method is used to calculate the positions of the resize handles */
void CMapPath::generateResizePositions(void)
{
	resizePos.clear();
	resizePos.append(QRect(tempPathCords.first().x()-3,tempPathCords.first().y()-3,7,7));
	resizePos.append(QRect(tempPathCords.last().x()-3,tempPathCords.last().y()-3,7,7));

}

/** Used to find out if the mouse is in the resize box
  * @param mousePos The position of the mouse pointer
  * @param currentZone A pointer to the current zone
  * @return the ID of the resize box, or 0 if not in any */
int CMapPath::mouseInResize(QPoint ,CMapZone *)
{
	return 0;
}

/** Used to find out if the mosuse is in the edit box of the path
  * @param mousePos The position of the mouse pointer
  * @param currentZone A pointer to the current zone
  * @return the ID of the edit box, or 0 if not in any */
int CMapPath::mouseInEditBox(QPoint mousePos ,CMapZone *)
{
	if (getEditMode())
	{
		int count = 1;
		for( PointList::Iterator point = bendList.begin(); point != bendList.end(); ++point )
		{
			QRegion r((*point).x()-3,(*point).y()-3,6,6);
			if (r.contains(mousePos))
			{
				return count;
			}
			count ++;
		}
	}

	return 0;
}

/** Used to move a bend to a new position */
void CMapPath::moveBend(int bend, QPoint pos)
{
	if (bend>0 && bend-1 < (int)bendList.count())
	{
		bendList.replace(bend-1, pos);
		getManager()->changedElement(this);
	}
}

/** This method is used to delete bends from a path
  * @param seg The path segment number to delete */
QPoint CMapPath::deletePathSeg(int seg)
{	
  QPoint deletedPos;

  for( PointList::Iterator point = bendList.begin(); point != bendList.end(); ++point )
  {
    qDebug() << "Bend : " << (*point).x() << "," << (*point).y();	
  }

  if (seg > (int) bendList.count()) seg = bendList.count();
  deletedPos = bendList.at(seg - 1);
  return deletedPos;
}

/**
 * This method is used to find out if the mouse pointer is in a path segment.
 * @param mousePos The position of the mouse
 * @param currentZone The current zone being viewed
 * @return 0 if not in any segament, otherwise return the number of the segment
 */
int CMapPath::mouseInPathSeg(QPoint mousePos,CMapZone *currentZone)
{
	// Is this a inter zone path
	if (getSrcRoom()->getZone()!=currentZone || getDestRoom()->getZone()!=currentZone)
		return -1;

	if (bendList.count()==0)
		return -1;

	int count = 0;
	int x1=tempPathCords.first().x(),y1=tempPathCords.first().y(),x2,y2;

	for( PointList::Iterator point = tempPathCords.begin(); point != tempPathCords.end(); ++point )
	{
		x2 = (*point).x();
		y2 = (*point).y();

		if (count > 1 && count < (int)tempPathCords.count()-1)
		{
			int startx = x1-5;
			int endx = x2+5;
			int starty = y1-5;
			int endy = y2+5;
	
			QRegion r(startx,starty,endx-startx,endy-starty);

			if (r.contains(mousePos))
			{

				int d = getDistance (mousePos.x(),mousePos.y(),startx,endx,starty,endy);

				if (d<=5)
				{
					return count -1;
				}
			}
		}

		count++;

		x1 = x2;
		y1 = y2;
	}

	return -1;
}
