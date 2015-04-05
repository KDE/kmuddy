/***************************************************************************
                               dlgmapspeedwalklist.cpp
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

#include "dlgmapspeedwalklist.h"

#include <qpushbutton.h>
#include <q3listview.h>
#include <qcombobox.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <kmenu.h>

#include "../../kmuddy_mapper.h"
#include "../../cmapmanager.h"
#include "../../cmaproom.h"
#include "../../cmapzone.h"
#include "../../cmaplevel.h"
#include "../../cmaplistviewitem.h"
#include "../../cmapviewbase.h"

#include "cmappluginspeedwalk.h"
#include "cmapcmdspeedwalkremove.h"

DlgMapSpeedwalkList::DlgMapSpeedwalkList(CMapPluginSpeedwalk *plugin,QWidget *parent) : QWidget(parent)
{
  setupUi(this);
	m_manager = plugin->getManager();
	m_plugin = plugin;

	initContexMenus();
	m_cmdDelete->setPixmap(UserIcon("kmud_delete.png"));
	m_cmdProperties->setPixmap(UserIcon("kmud_properties.png"));
	m_cmdSpeedwalk->setPixmap(UserIcon("kmud_follow.png"));

	m_cboCategorize->setCurrentItem(plugin->getSpeedwalkCatogrize());

	m_lstSpeedwalk->setColumnWidthMode(0,Q3ListView::Maximum);
	updateSpeedwalkList();

	m_selectedItem = NULL;
}


DlgMapSpeedwalkList::~DlgMapSpeedwalkList()
{
	
}

/** Used to create the element context menus */
void DlgMapSpeedwalkList::initContexMenus(void)
{
	// Setup the actions
	// Room Actions	
	m_roomOpen = new KAction (this);
	m_roomOpen->setText (i18n("&Show in active view"));
	connect  (m_roomOpen, SIGNAL (triggered()), this, SLOT(slotRoomOpen()));
	m_roomOpenNew = new KAction (this);
	m_roomOpenNew->setText (i18n("Show in &new view"));
	connect  (m_roomOpenNew, SIGNAL (triggered()), this, SLOT(slotRoomOpenNew()));
	m_roomWalkTo = new KAction (this);
	m_roomWalkTo->setText (i18n("&Walk to room"));
	connect  (m_roomWalkTo, SIGNAL (triggered()), this, SLOT(slotRoomWalkTo()));
	m_roomRemove = new KAction (this);
	m_roomRemove->setText (i18n("&Remove from list"));
	connect  (m_roomRemove, SIGNAL (triggered()), this, SLOT(slotRoomRemove()));
	m_roomProperties = new KAction (this);
	m_roomProperties->setText (i18n("&Properties"));
	connect  (m_roomProperties, SIGNAL (triggered()), this, SLOT(slotRoomProperties()));

	m_levelOpen = new KAction (this);
	m_levelOpen->setText (i18n("&Open in active view"));
	connect  (m_levelOpen, SIGNAL (triggered()), this, SLOT(slotLevelOpen()));
	m_levelOpenNew = new KAction (this);
	m_levelOpenNew->setText (i18n("Open in &new view"));
	connect  (m_levelOpenNew, SIGNAL (triggered()), this, SLOT(slotLevelOpenNew()));
	m_levelRemove = new KAction (this);
	m_levelRemove->setText (i18n("&Remove from list"));
	connect  (m_levelRemove, SIGNAL (triggered()), this, SLOT(slotLevelRemove()));

	// Zone Actions
	m_zoneRemove = new KAction (this);
	m_zoneRemove->setText (i18n("&Remove from list"));
	connect  (m_zoneRemove, SIGNAL (triggered()), this, SLOT(slotZoneRemove()));

	// Setup the menus
        QAction *sep1 = new QAction (this); sep1->setSeparator (true);
        QAction *sep2 = new QAction (this); sep2->setSeparator (true);
        QAction *sep3 = new QAction (this); sep3->setSeparator (true);
        QAction *sep4 = new QAction (this); sep4->setSeparator (true);
	// Room menu
	m_room_menu = new KActionMenu(KIcon("room_menu"),i18n("Room"),this);
	m_room_menu->addAction(m_roomOpen);
	m_room_menu->addAction(m_roomOpenNew);
	m_room_menu->addAction(sep1);
	m_room_menu->addAction(m_roomWalkTo);
	m_room_menu->addAction(sep2);
	m_room_menu->addAction(m_roomProperties);

	// Level menu
	m_level_menu = new KActionMenu(KIcon("level_menu"),i18n("&Level"),this);
	m_level_menu->addAction(m_levelOpen);
	m_level_menu->addAction(m_levelOpenNew);
	m_level_menu->addAction(sep3);
	m_level_menu->addAction(m_levelRemove);
	
	// Zone menu
	m_zone_menu = new KActionMenu(KIcon("zone_menu"),i18n("&Zone"),this);
	m_zone_menu->addAction(m_zoneRemove);
	m_zone_menu->addAction(sep4);
}

void DlgMapSpeedwalkList::updateSpeedwalkList(CMapRoom *updatedRoom)
{                                                                      	
	m_lstSpeedwalk->clear();	

	CMapListViewItem *foundItem = NULL;	
	int catogry = m_cboCategorize->currentItem();

	// Catogrized by room
	if (catogry == 0)
	{
		m_lstSpeedwalk->setRootIsDecorated (false);
		foreach (CMapRoom *room, *m_plugin->getSpeedwalkRoomList())
		{
			CMapListViewItem *newItem = new CMapListViewItem (m_lstSpeedwalk,getRoomName(room));
			newItem->setElement(room);			
			if (room == updatedRoom) foundItem = newItem;
		}
	}

	// Catogrized by room/zone
	if (catogry == 1)
	{
		m_lstSpeedwalk->setRootIsDecorated (true);
		foreach (CMapRoom *room, *m_plugin->getSpeedwalkRoomList())
		{
			CMapListViewItem *roomItem = new CMapListViewItem (findZone(room),getRoomName(room));
			roomItem->setElement(room);

			if (room == updatedRoom) foundItem = roomItem;
		}
	}

	// Catogrize by room/level/zone
	if (catogry == 2)
	{
		m_lstSpeedwalk->setRootIsDecorated (true);
		foreach (CMapRoom *room, *m_plugin->getSpeedwalkRoomList())
		{			
			CMapListViewItem *roomItem = new CMapListViewItem (findLevel(room),getRoomName(room));
			roomItem->setElement(room);

			if (room == updatedRoom) foundItem = roomItem;
		}
	}

	if (foundItem)
	{
		switch (catogry)
		{
			case 0 : m_lstSpeedwalk->setOpen(foundItem->parent(),true);
			         break;
			case 1 : m_lstSpeedwalk->setOpen(foundItem->parent()->parent(),true);
			         m_lstSpeedwalk->setOpen(foundItem->parent(),true);

			         break;
			case 2 : m_lstSpeedwalk->setOpen(foundItem->parent()->parent()->parent(),true);
			         m_lstSpeedwalk->setOpen(foundItem->parent()->parent(),true);
			         m_lstSpeedwalk->setOpen(foundItem->parent(),true);
			         break;
		}

		m_lstSpeedwalk->setOpen(foundItem,true);
		m_lstSpeedwalk->ensureItemVisible(foundItem);
		m_lstSpeedwalk->setCurrentItem(foundItem);
		m_lstSpeedwalk->setSelected(foundItem,true);
	}
}

/* Used to get the room name */
QString DlgMapSpeedwalkList::getRoomName(CMapRoom *room)
{
	QString label =room->getLabel().trimmed();
	if (label.isEmpty()) label = i18n("Unnamed room");
	
	return label;
}

/* Used to get the room name */
QString DlgMapSpeedwalkList::getZoneName(CMapZone *zone)
{
	QString label =zone->getLabel().trimmed();
	
	return label;
}

/* Used to get the room name */
QString DlgMapSpeedwalkList::getLevelName(CMapLevel *level)
{
	return i18n("Level %1").arg(level->getNumber());
}

CMapListViewItem *DlgMapSpeedwalkList::findZone(CMapRoom *room)
{
	CMapListViewItem *item = (CMapListViewItem *)m_lstSpeedwalk->firstChild();
	bool found = false;

	while (item)
	{		
		CMapElement *elm = item->getElement();
		if (elm->getElementType()==ZONE)		
		{
			CMapZone *tmpZone = (CMapZone*)elm;

			if (room->getZone()==tmpZone)
			{
				found = true;
				break;
			}
		}
		item = (CMapListViewItem *)item->nextSibling();
	}

	if (!found)
	{
		item = new CMapListViewItem(m_lstSpeedwalk,getZoneName(room->getZone()));
		item->setElement(room->getZone());
	}

	return item;
}

CMapListViewItem *DlgMapSpeedwalkList::findLevel(CMapRoom *room)
{
	CMapListViewItem *zoneItem = (CMapListViewItem *)m_lstSpeedwalk->firstChild();
	CMapListViewItem *levelItem = NULL;
	bool foundZone = false;
	bool foundLevel = false;
	QString level = getLevelName(room->getLevel());

	while (zoneItem)
	{
		CMapElement *elm = zoneItem->getElement();
		if (elm->getElementType()==ZONE)		
		{
			CMapZone *tmpZone = (CMapZone*)elm;

			if (room->getZone()==tmpZone)
			{
                levelItem = (CMapListViewItem *)zoneItem->firstChild();

				while(levelItem)
				{
					if (level == levelItem->text(0))
					{
						foundLevel = true;
						break;
					}
					levelItem = (CMapListViewItem *)levelItem->nextSibling();
				}
				
				foundZone = true;	
				break;
			}
		}
		zoneItem = (CMapListViewItem *)zoneItem->nextSibling();

	}

	if (!foundZone)
	{
		zoneItem = new CMapListViewItem(m_lstSpeedwalk,getZoneName(room->getZone()));
		zoneItem->setElement(room->getZone());
	}

	if (!foundLevel)
	{
		levelItem = new CMapListViewItem(zoneItem,getLevelName(room->getLevel()));
		levelItem->setLevel(room->getLevel());
	}

	return levelItem;
}

/** Used to find out which level a item is at */
signed int DlgMapSpeedwalkList::getLevelOfItem(CMapListViewItem *item)
{
	signed int count=-1;

	while (item!=NULL)
	{
		count++;
		item = (CMapListViewItem *)item->parent();
	}

	return count;
}

/** Delete all the elements based on the zone */
bool DlgMapSpeedwalkList::deleteZone(CMapListViewItem *zoneItem)
{
  bool found = false;
  CMapCmdSpeedwalkRemove *cmd = new CMapCmdSpeedwalkRemove(m_plugin);

  if (zoneItem->getElement()->getElementType()==ZONE)
  {
    CMapZone *zone = (CMapZone *)zoneItem->getElement();

    QList<CMapRoom *> *rooms = m_plugin->getSpeedwalkRoomList();
    QList<CMapRoom *>::iterator it;
    for (it = rooms->begin(); it != rooms->end(); ) {
      CMapRoom *room = *it;
      if (room->getZone()==zone)
      {
        cmd->addRoom(room);
        it = rooms->erase(it);
        found = true;
      }
      else
      {
        ++it;
      }
    }
  }

  if (found)
    m_plugin->getManager()->addCommand(cmd);
  else
    delete cmd;

  return found;
}

/** Delete all the elements based on the level and zone */
bool DlgMapSpeedwalkList::deleteLevel(CMapListViewItem *levelItem)
{
  CMapCmdSpeedwalkRemove *cmd = new CMapCmdSpeedwalkRemove(m_plugin);
  bool found = false;
  if (levelItem->getLevel())
  {
    QList<CMapRoom *> *rooms = m_plugin->getSpeedwalkRoomList();
    QList<CMapRoom *>::iterator it;
    for (it = rooms->begin(); it != rooms->end(); ) {
      CMapRoom *room = *it;
      if (room->getLevel()==levelItem->getLevel())
      {
        cmd->addRoom(room);
        it = rooms->erase(it);
        found = true;
      }
      else
        it++;
    }
  }

  if (found)
  {
    m_manager->addCommand(cmd);
  }
  else
  {
    delete cmd;
  }

  return found;
}

/** Used to delete a room from the speedwalk list */
bool DlgMapSpeedwalkList::deleteRoom(CMapListViewItem *roomItem)
{
	CMapCmdSpeedwalkRemove *cmd = new CMapCmdSpeedwalkRemove(m_plugin);
	bool found = false;

	if (roomItem->getElement()->getElementType()==ROOM)
	{
		CMapRoom *room = (CMapRoom *)roomItem->getElement();
		if (room)
		{
			found = true;
			cmd->addRoom(room);
			//m_plugin->getSpeedwalkRoomList()->remove(room);
		}
	}

	if (found)
	{
		m_manager->addCommand(cmd);
	}
	else
	{
		delete cmd;
	}

	return found;
}


void DlgMapSpeedwalkList::slotDelete()
{
	bool found = false;
	CMapListViewItem *currentItem = (CMapListViewItem *)m_lstSpeedwalk->currentItem();
	if (currentItem == 0) return;

	int currentLevel = getLevelOfItem(currentItem);

	switch (m_plugin->getSpeedwalkCatogrize())
	{
		case 0 : if (deleteRoom(currentItem)) found = true;
		         break;

		case 1 : if (currentLevel==0)
		         {
			        if (deleteZone(currentItem)) found = true;
		         }

		         if (currentLevel==1)
		         {
	         		if (deleteRoom(currentItem)) found = true;
		         }
		         break;

		case 2 : if (currentLevel==0)
				 {
			        if (deleteZone(currentItem)) found = true;
				 }

				 if (currentLevel==1)
				 {
				 	if (deleteLevel(currentItem)) found = true;
				 }

				 if (currentLevel==2)
		         {
	         		if (deleteRoom(currentItem)) found = true;
		         }
		         break;
	}

	if (found)
	{
		updateSpeedwalkList();	
	}
}

void DlgMapSpeedwalkList::slotProperties()
{
	CMapListViewItem *currentItem = (CMapListViewItem *)m_lstSpeedwalk->currentItem();	

	if (!currentItem)
		return;

	CMapElement *elm = currentItem->getElement();
	if (elm!=NULL)
	{
		if (elm->getElementType()==ROOM)
		{
			m_manager->propertiesRoom((CMapRoom *)elm);
			updateSpeedwalkList((CMapRoom *)elm);
		}
	}
}

void DlgMapSpeedwalkList::slotWalk()
{
	CMapListViewItem *currentItem = (CMapListViewItem *)m_lstSpeedwalk->currentItem();	

	slotDoubleClicked(currentItem);
}

void DlgMapSpeedwalkList::slotCatogryChanged(int)
{
	m_plugin->setSpeedwalkCatogrize( m_cboCategorize->currentItem());
	updateSpeedwalkList();
}

void DlgMapSpeedwalkList::slotDoubleClicked(Q3ListViewItem *item)
{
	if (item == 0)
		return;
		
    CMapListViewItem *i = (CMapListViewItem *)item;

	if (i->getElement()!=NULL)
	{
		if (i->getElement()->getElementType()==ROOM)
		{
			CMapRoom *room =(CMapRoom *)(i->getElement());
			m_manager->walkPlayerTo(room);
		}
	}
}

void DlgMapSpeedwalkList::slotRightButtonClicked(Q3ListViewItem *item,const QPoint &pos,int)
{
	if (item == 0)
		return;
		
	CMapListViewItem *i = (CMapListViewItem *)item;
	bool enabled = m_manager->getActiveView()!=NULL;
	m_selectedItem = i;

	// Is it a level
	if (i->getLevel()!=NULL)
	{
		m_levelOpen->setEnabled(enabled);
		m_level_menu->menu()->popup(pos);
	}
	else
	{
		if (i->getElement()!=NULL)
		{
			// Is it a zone
			if (i->getElement()->getElementType()==ZONE)
			{									
				m_zone_menu->menu()->popup(pos);
			}
	
			// Is it a room
			if (i->getElement()->getElementType()==ROOM)
			{	
				m_roomOpen->setEnabled(enabled);
				m_room_menu->menu()->popup(pos);
			}
        }
	}
}

void DlgMapSpeedwalkList::slotRoomOpen()
{
	if (m_selectedItem->getElement()!=NULL)
	{
		if (m_selectedItem->getElement()->getElementType()==ROOM)
		{
			CMapRoom *room =(CMapRoom *)(m_selectedItem->getElement());
			m_manager->getActiveView()->showPosition(room->getLowPos(),room->getLevel());
		}
	}
}

void DlgMapSpeedwalkList::slotRoomOpenNew()
{
	if (m_selectedItem->getElement()!=NULL)
	{
		if (m_selectedItem->getElement()->getElementType()==ROOM)
		{
			CMapRoom *room =(CMapRoom *)(m_selectedItem->getElement());
			m_manager->openNewMapView(room->getLowPos(),room->getLevel());
		}
	}
}

void DlgMapSpeedwalkList::slotRoomWalkTo()
{
	if (m_selectedItem->getElement()!=NULL)
	{
		if (m_selectedItem->getElement()->getElementType()==ROOM)
		{
			CMapRoom *room =(CMapRoom *)(m_selectedItem->getElement());
			m_manager->walkPlayerTo(room);
		}
	}
}

void DlgMapSpeedwalkList::slotRoomRemove()
{
	deleteRoom(m_selectedItem);
}

void DlgMapSpeedwalkList::slotRoomProperties()
{
	if (m_selectedItem->getElement()!=NULL)
	{
		if (m_selectedItem->getElement()->getElementType()==ROOM)
		{
			CMapRoom *room =(CMapRoom *)(m_selectedItem->getElement());
			m_manager->propertiesRoom(room);
			updateSpeedwalkList((CMapRoom *)room);
		}
	}
}

void DlgMapSpeedwalkList::slotLevelOpen()
{
	if (m_selectedItem->getLevel()!=NULL)
	{
		m_manager->getActiveView()->showPosition(m_selectedItem->getLevel());
	}
}

void DlgMapSpeedwalkList::slotLevelOpenNew()
{
	if (m_selectedItem->getLevel()!=NULL)
	{
		m_manager->openNewMapView(m_selectedItem->getLevel());
	}
}

void DlgMapSpeedwalkList::slotLevelRemove()
{
	if (m_selectedItem->getLevel()!=NULL)
	{
		deleteLevel(m_selectedItem);
	}
}

void DlgMapSpeedwalkList::slotZoneRemove()
{
	if (m_selectedItem->getElement()!=NULL)
	{		
		if (m_selectedItem->getElement()->getElementType()==ZONE)
		{
			deleteZone(m_selectedItem);
		}
	}
}

