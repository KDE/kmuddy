/***************************************************************************
                          kcmapfile.h
                      -------------------
    description          :
    begin                : Mon Jan 1 2001
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


#ifndef KCMAPFILE_H
#define KCMAPFILE_H

#include <kconfig.h>
#include <kconfiggroup.h>

#include <qfile.h>

class CMapZone;
class CMapText;
class CMapRoom;
class CMapPath;

/**
  *@author Kevin Krammer and John-Paul Stanford
  */

class KCMapFile : public KConfig
{
public: 
	KCMapFile(QString filename,int gridWidth,int gridHeight);
	~KCMapFile();
	
	virtual void setRoomGroup(int rnum);
	virtual void setPathGroup(int pnum);
	virtual void setTextGroup(int tnum);
	virtual void setLevelGroup(int lnum);
	virtual void setZoneGroup(int znum);
	virtual void setSpeedwalkGroup(int snum);

	virtual void writeVersion(int major, int minor);
	virtual void readVersion(int &major, int &minor);
	virtual void writeCount(QString name, int number);
	virtual int readCount(QString name);

	void loadZone(CMapZone *zone);
	void loadRoom(CMapRoom *room);
	void loadText(CMapText *text);
	void loadPath(CMapPath *path);

	KConfigGroup currentGroup () { return m_group; };
protected:
 	KConfigGroup numberedGroup(QString groupPrefix, int number);

private:
	int m_gridWidth;
	int m_gridHeight;
        KConfigGroup m_group;
};

#endif
