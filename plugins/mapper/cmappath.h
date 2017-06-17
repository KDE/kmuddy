/***************************************************************************
                               cmappath.h
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

#ifndef CMAPPATH_H
#define CMAPPATH_H

#include <qrect.h>
#include <QList>

#include "cmapelement.h"

#include "kmemconfig.h"
#include <kconfiggroup.h>

class CMapRoom;

/**The class used to store path data
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapPath : public CMapElement
{
public:
	CMapPath(CMapManager *manager,CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir, bool twoWay);
	CMapPath(CMapManager *manager,CMapRoom *srcRoom,CMapRoom *destRoom);
	~CMapPath();

	elementTyp getElementType(void)              { return PATH ; }
	/** This method is used to set the cords of the path */
	void setCords(void);

	bool getDone(void)                           { return done; }
	void setDone(bool set)                       { done = set; }

	void paint(QPainter *p,CMapZone *zone);
	/** Used to paint the element while it is bined resized */	
	void resizePaint(QPoint offset,QPainter *p,CMapZone *currentZone,int resizeId);
	void dragPaint(QPoint offset,QPainter *p,CMapZone *zone);
	void lowerPaint(QPainter *p,CMapZone *zone);
	void higherPaint(QPainter *p,CMapZone *zone);
	void editPaint(QPoint pos,QPainter *p,CMapZone *,int editBox);

	/** Used to find out if the mouse is in the element */
	bool mouseInElement(QPoint mousePos);

	/** Used to create a deep copy of the path */
	CMapElement *copy(void);

	/** Get the directions of the start and finish directions of the path */
	directionTyp getSrcDir(void)                 { return srcDir; }
	directionTyp getDestDir(void)                { return destDir; }

	void setOpsitePath(CMapPath *path)           { opsitePath = path; }
	CMapPath *getOpsitePath(void)                { return opsitePath; }

	/** Get a pointer to the destination room */
	CMapRoom *getDestRoom(void)                  { return destRoom; }
	CMapRoom *getSrcRoom(void)                   { return srcRoom; }

	/** Set the source direction of the path */
	void setSrcDir(directionTyp SrcDir);
	/** Set the destination directions of the path */
	void setDestDir(directionTyp DestDir);

	/** Set a pointer to the destination room */
	void setDestRoom(CMapRoom *DestRoom);
	/** Set a pointer to the source room */
	void setSrcRoom(CMapRoom *SrcRoom);

        void makeOneWay();
        void makeTwoWay();

	/** Set the special exit command string */
	void setSpecialCmd(QString cmd)              { specialCmd = cmd; }
	/** Return the special exit command string */
	QString getSpecialCmd(void)                  { return specialCmd; }

	/** Tell the mapp if this path is a special exit */
	void setSpecialExit(bool special)            { bSpecialExit = special; }
	/** Return if this path is a special exit */
	bool getSpecialExit(void)                    { return bSpecialExit; }

	/** Get a list of bends in the path */
	int getBendCount(void)                       { return bendList.count(); }
	/** Used to set the before walking command */
	void setBeforeCommand(QString command)       { beforeCommand = command; }
	/** Used to get the before walking commnad */
	QString getBeforeCommand(void)               { return beforeCommand; }
	/** Used to set the after walking command */
	void setAfterCommand(QString command)        { afterCommand = command; }
	/** Used to get the before walking commnad */
	QString getAfterCommand(void)                { return afterCommand; }
	
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

	/** Used to move the element relative to it's current position */
	virtual void moveBy(QPoint offset);

	/** This is overriddent to stop a path be resized */
	virtual void resize(QPoint,int) { }
	/** Used to find out if the mouse is in the resize box
	  * @param mousePos The position of the mouse pointer
	  * @param currentZone A pointer to the current zone
	  * @return the ID of the resize box, or 0 if not in any */
	virtual int mouseInResize(QPoint mousePos,CMapZone *currentZone);
	/**
	 * This method is used to find out if the mouse pointer is in a path segment.
	 * @param mousePos The position of the mouse
	 * @param currentZone The current zone being viewed
	 * @return 0 if not in any segament, otherwise return the number of the segment
	 */
	int mouseInPathSeg(QPoint mousePos,CMapZone *currentZone);

	/** Used to find out if the mosuse is in the edit box of the path
	  * @param mousePos The position of the mouse pointer
	  * @param currentZone A pointer to the current zone
	  * @return the ID of the edit box, or 0 if not in any */
	int mouseInEditBox(QPoint mousePos ,CMapZone *);

	void setDontPaintBend(int bend);
	/** Used to add a bend to the path */
	void addBendWithUndo(QPoint pos);
	/** Used to move a bend to a new position */
	void moveBendWithUndo(int bend, QPoint pos);
	/** This method is used to delete bends from a path
	  * @param seg The path segment number to delete */
	void deletePathSegWithUndo(int seg);


protected:
	void deleteBend(QPoint bend);
	/** Used to add a bend to the path */
	int addBend(QPoint pos);
	/** Used to move a bend to a new position */
	void moveBend(int bend, QPoint pos);
	/** This method is used to delete bends from a path
	  * @param seg The path segment number to delete */
	QPoint deletePathSeg(int seg);

	/** This method is used to calculate the positions of the resize handles */
	void generateResizePositions(void);
	/** Used to paint the element at a given location and size
	  * @param p The painer to paint the element to
	  * @param pos The position to paint the elmenet
	  * @param size The size the element should be draw
	  * @param zone The current zone being viewed */
	virtual void paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *zone);

private:
	/** This method is used to calcualte the distance from a path segment */
	int getDistance (int x,int y,int x1,int x2,int y1,int y2);
	directionTyp generatePath();
	/** Used to draw an arrow head */
	void drawArrow(directionTyp dir,QPainter *p,QPoint end);
	QPoint getIndent(directionTyp dir,QPoint pos);
	/** Used to draw the path */
	void drawPath(QPainter *p,CMapZone *zone,QPoint offset,QColor color,int editBox=0,QPoint editPos=QPoint(0,0));

	QRect getBoundRectOfSeg(QPoint pos1,QPoint pos2);

private:
	typedef QList<QPoint> PointList;

	/** If this is greater than 0 then the bend will not be painted */
	int m_dontPaintBend;

	/** This is used to store a tempray list of path cords */
	PointList tempPathCords;

	bool done;
	/** The commands of the path */
	QString afterCommand,beforeCommand;
	/** A pointer to the path going in the opsite direction. Null if no path */
	CMapPath *opsitePath;
	/** These are used to store the directions the path enters and leaves a room */
	directionTyp srcDir,destDir;

	/** Used to store a pointer to the desination room */
	CMapRoom *destRoom,*srcRoom;

	/** Used to flag if this is a special exit */
	bool bSpecialExit;
	/** Used to store the special command string for a path */
	QString specialCmd;

	/** A list of bends in the path */
	PointList bendList;
};

#endif
