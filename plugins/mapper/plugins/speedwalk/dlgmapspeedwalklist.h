/***************************************************************************
                               dlgmapspeedwalklist.h
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

#ifndef DLGMAPSPEEDWALKLIST_H
#define DLGMAPSPEEDWALKLIST_H

#include <qwidget.h>
#include "dlgmapspeedwalklistbase.h"

class KAction;
class KActionMenu;

class CMapRoom;
class CMapManager;
class CMapZone;
class CMapLevel;
class CMapListViewItem;
class CMapPluginSpeedwalk;

/**This is the speewalk pane
  *@author Kmud Developer Team
  */

class DlgMapSpeedwalkList : public DlgMapSpeedwalkListBase
{
   Q_OBJECT

public: 
	DlgMapSpeedwalkList(CMapPluginSpeedwalk *plugin,QWidget *parent=0, const char *name=0);
	~DlgMapSpeedwalkList();

	/** Used to update the speedwalk list */
	void updateSpeedwalkList(CMapRoom *updatedRoom=0);


protected slots:
	void slotCatogryChanged(int);
	void slotDelete();
    void slotProperties();
    void slotWalk();
	void slotDoubleClicked(Q3ListViewItem *);
	void slotRightButtonClicked(Q3ListViewItem *,const QPoint &,int);

private slots:
	void slotRoomOpen();
	void slotRoomOpenNew();
	void slotRoomWalkTo();
	void slotRoomRemove();
	void slotRoomProperties();

	void slotLevelOpen();
	void slotLevelOpenNew();
	void slotLevelRemove();

	
	void slotZoneRemove();
	void slotZoneProperties();

private:
	void initContexMenus(void);
	signed int getLevelOfItem(CMapListViewItem *item);
	bool deleteZone(CMapListViewItem *zoneItem);
	bool deleteLevel(CMapListViewItem *levelItem);
	bool deleteRoom(CMapListViewItem *roomItem);
	QString getRoomName(CMapRoom *room);
	QString getZoneName(CMapZone *zone);
	QString getLevelName(CMapLevel *level);
	CMapListViewItem *findZone(CMapRoom *room);
	CMapListViewItem *findLevel(CMapRoom *room);

private:
	CMapPluginSpeedwalk *m_plugin;

	CMapListViewItem *m_selectedItem;
	CMapManager *m_manager;

	KActionMenu *m_room_menu;
	KAction *m_roomOpen;
	KAction *m_roomOpenNew;
	KAction *m_roomRemove;
	KAction *m_roomWalkTo;
	KAction *m_roomProperties;

	KActionMenu *m_level_menu;
	KAction *m_levelOpen;
	KAction *m_levelOpenNew;
	KAction *m_levelRemove;

	KActionMenu *m_zone_menu;
	KAction *m_zoneRemove;
	KAction *m_zoneProperties;
};

#endif
