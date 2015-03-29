/***************************************************************************
                               cmapzone.h
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

#ifndef CMAPZONE_H
#define CMAPZONE_H

#include <qcolor.h>
#include <QList>

#include "cmapelement.h"

#include "kmemconfig.h"

class CMapLevel;
class CMapText;
class CMapManager;

/** The zone holds a list of levels in the zone. There's only one per map.
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapZone : public CMapElement
{
public:
	enum labelPosTyp { NORTH , SOUTH , WEST , EAST , NORTHWEST, NORTHEAST, SOUTHEAST , SOUTHWEST, HIDE, CUSTOM };	

public:
	CMapZone(CMapManager *manager);
	~CMapZone();

	elementTyp getElementType(void)                  { return ZONE ; }
        virtual void setLevel(CMapLevel *level);

	void setLabel(QString zoneLabel);
	QString getLabel(void)                           { return label; }
	void setDescription (QString str)                { description = str; }
	QString getDescription (void)                    { return description; }

	void setUseDefaultCol(bool b);
	bool getUseDefaultCol(void);
	void setUseDefaultBackground(bool b);
	bool getUseDefaultBackground(void);
	void setColor(QColor col);
	QColor getColor(void);
	void setBackgroundColor(QColor col);
	QColor getBackgroundColor(void);

	QList<CMapLevel *> *getLevels() { return &mapLevelList; }
        CMapLevel *firstLevel() const;

	//void paint(QPainter *p,CMapZone *zone);
	void dragPaint(QPoint offset,QPainter *p,CMapZone *zone);
	void lowerPaint(QPainter *p,CMapZone *zone);
	void higherPaint(QPainter *p,CMapZone *zone);

	/** This is used to get a unique ID for the zone */
	unsigned int getZoneID(void)                              { return m_ID; }

	CMapElement *copy(void);

	labelPosTyp getLabelPosition(void)          { return labelPosition; }
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
		
	void setZoneID(unsigned  int id);
	
protected:
	void geometryChanged(void);
	/** Used to paint the element at a given location and size
	  * @param p The painer to paint the element to
	  * @param pos The position to paint the elmenet
	  * @param size The size the element should be draw
	  * @param zone The current zone being viewed */
	virtual void paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *zone);

	unsigned int m_ID;

private:
	/** This is used to paint the sub boxes displayed when the element is painted */
	void paintSubBox(QPainter *p,int x,int y,int width,int height);

public:
	unsigned int m_room_id_count;
	unsigned int m_text_id_count;
	
private:      
	labelPosTyp labelPosition;

	bool useDefaultCol,useDefaultBackground;
	/** The zone name */
	QString label;
	QString description;

	QColor color;
	QColor backgroundCol;

	/** This is a list of all the maps for this mud */
	QList<CMapLevel *> mapLevelList;
};

#endif
