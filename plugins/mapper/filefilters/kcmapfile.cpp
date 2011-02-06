/***************************************************************************
                          kcmapfile.cpp
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


#include "kcmapfile.h"

#include "../cmaproom.h"
#include "../cmapzone.h"
#include "../cmappath.h"
#include "../cmaptext.h"

KCMapFile::KCMapFile(QString filename,int gridWidth,int gridHeight):KConfig(filename)
{
  m_gridWidth = gridWidth;
  m_gridHeight = gridHeight;
}

KCMapFile::~KCMapFile()
{
}

void KCMapFile::setRoomGroup(int rnum)
{
  m_group = numberedGroup("ROOM-", rnum);
}

void KCMapFile::setPathGroup(int pnum)
{
  m_group = numberedGroup("PATH-", pnum);
}

void KCMapFile::setTextGroup(int tnum)
{
  m_group = numberedGroup("TEXT-", tnum);
}

void KCMapFile::setLevelGroup(int lnum)
{
  m_group = numberedGroup("LEVEL-", lnum);
}

void KCMapFile::setZoneGroup(int znum)
{
  m_group = numberedGroup("ZONE-", znum);
}

void KCMapFile::setSpeedwalkGroup(int snum)
{
  m_group = numberedGroup("SPEEDWALK-", snum);
}

KConfigGroup KCMapFile::numberedGroup(QString groupPrefix, int number)
{
  QString numString;
  numString.setNum(number);
  groupPrefix.append(numString);

  return group(groupPrefix);
}

void KCMapFile::writeVersion(int major, int minor)
{
  group("VERSION").writeEntry("major", major);
  group("VERSION").writeEntry("minor", minor);
}

void KCMapFile::readVersion(int &major, int &minor)
{
  major = group("VERSION").readEntry("major", 0);
  minor = group("VERSION").readEntry("minor", 0);
}

void KCMapFile::writeCount(QString name, int number)
{
  group("COUNTS").writeEntry(name, number);
}

int KCMapFile::readCount(QString name)
{
  return group("COUNTS").readEntry(name, 0);
}

void KCMapFile::loadZone(CMapZone *zone)
{
  zone->setLowPos(QPoint(m_group.readEntry("X", -1) * m_gridWidth,m_group.readEntry("Y", -1)* m_gridHeight));
  zone->setLabel(m_group.readEntry("name", ""));
}

void KCMapFile::loadRoom(CMapRoom *room)
{
  room->setUseDefaultCol(m_group.readEntry("use default color", true));
  room->setColor(m_group.readEntry("color"));
  room->setLowPos(QPoint(m_group.readEntry("X", -1) * m_gridWidth, m_group.readEntry("Y", -1)* m_gridHeight));
  room->setLabel(m_group.readEntry("label", ""));
  room->setDescription(m_group.readEntry("description", ""));
}

void KCMapFile::loadText(CMapText *text)
{
  text->setLowPos(QPoint(m_group.readEntry("X", -1) * m_gridWidth, m_group.readEntry("Y", -1)* m_gridHeight));

  text->setFont(m_group.readEntry("font"));
  text->setColor(m_group.readEntry("color", text->getColor()));

  text->setText(m_group.readEntry("text"));
}

void KCMapFile::loadPath(CMapPath *path)
{
  path->setSpecialExit(m_group.readEntry("special exit", false));
  path->setSpecialCmd(m_group.readEntry("special command", ""));

  // Load the path bends
  QString bendnum;
  QString bendpre("bend-");
  int num_bends = m_group.readEntry("bends", 0);

  for (int count = 0; count < num_bends; count++)
  {
    bendnum.setNum(count+1);
    path->addBendWithUndo(m_group.readEntry(bendpre+bendnum, QPoint()));
  }
}

