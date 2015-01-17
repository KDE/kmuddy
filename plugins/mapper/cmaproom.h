/***************************************************************************
                               cmaproom.h
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

#ifndef CMAPROOM_H
#define CMAPROOM_H

#include <qcolor.h>
#include <qstringlist.h>
#include <QList>

#include "cmapelement.h"

class CMapPath;
class CMapText;

#include "kmemconfig.h"

/**The class used to store room data
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapRoom : public CMapElement
{
public:
	enum labelPosTyp { NORTH =0, SOUTH , WEST , EAST , NORTHWEST, NORTHEAST, SOUTHEAST , SOUTHWEST, HIDE, CUSTOM } ;

public:	
	CMapRoom(CMapManager *manager,QRect rect,CMapLevel *level);
	~CMapRoom();

	elementTyp getElementType(void)             { return ROOM ; }

        virtual void setLevel(CMapLevel *level);

	void setLabel(QString str);
	QString getLabel(void)                      { return label; }
	void setDescription (QString str)           { description = str; }
	QString getDescription (void)               { return description; }

	/** Used to add a exit from the room */
	void addPath(CMapPath *path);
	/** Used to find the path for the give direction. If a path can't be found then
	    null is returned                                                             */
	CMapPath *getPathDirection(directionTyp dir,QString specialCmd);
	/** Get a list of the paths from this room */
	QList<CMapPath *> *getPathList() { return &pathList; }
	/** Get a list of the paths connecting with this room */
	QList<CMapPath *> *getConnectingPathList() { return &connectingPaths; }
	/** Get a pointer to the contents list */
	QStringList *getContentsList(void)          { return &contentsList; }

	void setUseDefaultCol(bool b)               { useDefaultCol = b; }
	bool getUseDefaultCol(void)                 { return useDefaultCol; }
	void setColor(QColor col)                  { color = col; }
	QColor getColor(void)                      { return color; }

	void setCurrentRoom(bool currentRoom);
	bool getCurrentRoom(void)                   { return current; }
	void setLoginRoom(bool loginRoom);
	bool getLoginRoom(void)                     { return login; }

	void setCopiedRoom(CMapRoom *room)          { copiedRoom = room; }
	CMapRoom *getCopiedRoom(void)               { return copiedRoom; }

	void setMoveTime(int moveTime)              { time = moveTime; }
	int getMoveTime(void)                       { return time; }

	void paint(QPainter *p,CMapZone *);
	void dragPaint(QPoint offset,QPainter *p,CMapZone *);
	void lowerPaint(QPainter *p,CMapZone *);
	void higherPaint(QPainter *p,CMapZone *);

	CMapElement *copy(void);

	labelPosTyp getLabelPosition(void);
	void setLabelPosition(labelPosTyp pos);
	void setLabelPosition(labelPosTyp pos,CMapText *text);
	void textRemove(void);

	/** Used to load the properties of the element from a list of properties */
	virtual void loadProperties(KConfigGroup properties);
	/** Used to save the properties of the element to a list of properties */
	virtual void saveProperties(KConfigGroup properties);
    /** Used to save the element as an XML object 
      * @param properties The XML object to save the properties too
      * @param doc The XML Document */
	virtual void saveQDomElement(QDomDocument *doc,QDomElement *properties);
	/** Used to load the properties from a XML object 
	  * @param properties The XML object to load the properties too */
	virtual void loadQDomElement(QDomElement *properties);
	
	/** Used to move the element relative to it's current position */
	virtual void moveBy(QPoint offset);
	/** This is used to resize the element */
	void resize(QPoint offset,int resizeId);
	/** This is used to get a unique ID for the room */
	unsigned int getRoomID(void);
	/** This is used to set the ID of the room */
	void setRoomID(unsigned int id);
	/** Get the text element linked to this element */
	CMapText *getLinkedElement(void)                       { return textElement; }
	
protected:
	/** Used to paint the element at a given location and size
	  * @param p The painer to paint the element to
	  * @param pos The position to paint the elmenet
	  * @param size The size the element should be draw
	  * @param zone The current zone being viewed */
	void paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *zone);
	void geometryChanged(void);
	
	unsigned int m_ID;

private:
	int time;
	/** Used to store details of all the paths from this room */
	QList<CMapPath *> pathList;
	QList<CMapPath *> connectingPaths;

	QStringList contentsList;

	labelPosTyp labelPosition;

	bool current,login,useDefaultCol;

	QColor color;

	CMapText *textElement;

	QString label;
	QString description;

	CMapRoom *copiedRoom;
};

#endif
