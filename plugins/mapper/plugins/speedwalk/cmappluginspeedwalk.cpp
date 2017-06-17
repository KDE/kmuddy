/***************************************************************************
                               cmappluginspeedwalk.cpp
                             -------------------
    begin                : Mon Aug 6 2001
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

#include "cmappluginspeedwalk.h"

#include <qicon.h>

#include <kstandarddirs.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>

#include "dlgmapspeedwalkcontainter.h"
#include "dlgmapspeedwalklist.h"
#include "cmapcmdspeedwalkadd.h"
#include "cmapcmdspeedwalkremove.h"

#include "../../cmapmanager.h"
#include "../../cmaproom.h"
#include "../../cmapview.h"

#include "cglobalsettings.h"

CMapPluginSpeedwalk::CMapPluginSpeedwalk(QObject *parent) : CMapPluginBase(parent)
{
  CMapView *view = dynamic_cast<CMapView *>(parent);
	m_showAction = new KAction (this);
        m_showAction->setText (i18n("Show Speedwalk List"));
        connect (m_showAction, SIGNAL (triggered()), this, SLOT(slotViewSpeedwalkList()));
        view->actionCollection()->addAction("viewSpeedwalkList", m_showAction);
	m_addAction = new KAction (this);
        m_addAction->setText (i18n("&Add to speed walk list"));
        connect (m_addAction, SIGNAL (triggered()), this, SLOT(slotRoomAddToSpeedwalk()));
        view->actionCollection()->addAction("roomAddToSpeedwalk", m_addAction);
	
	m_speedwalkList = NULL;

	m_showAction->setEnabled(true);
}

CMapPluginSpeedwalk::~CMapPluginSpeedwalk()
{
}

void CMapPluginSpeedwalk::saveConfigOptions(void)
{
  cGlobalSettings *gs = cGlobalSettings::self();
  gs->setInt ("mapper-speedwalk-catogrize", m_speedwalkCatogrize);
}

void CMapPluginSpeedwalk::loadConfigOptions(void)
{
  cGlobalSettings *gs = cGlobalSettings::self();
  m_speedwalkCatogrize = gs->getInt ("mapper-speedwalk-catogrize");
}

int CMapPluginSpeedwalk::getSpeedwalkCatogrize()
{
	return m_speedwalkCatogrize;
}

void CMapPluginSpeedwalk::setSpeedwalkCatogrize(int catogry)
{
	m_speedwalkCatogrize = catogry;
}

void CMapPluginSpeedwalk::showSpeedwalkList()
{
	if (!m_speedwalkList)
	{
		m_speedwalkList = new DlgMapSpeedwalkContainter(this);
		m_speedwalkList->show();
	}

}

void CMapPluginSpeedwalk::hideSpeedwalkList()
{
	if (m_speedwalkList)
	{
		delete m_speedwalkList;
		m_speedwalkList=NULL;
	}
}

/** Used to add a room to the speed walk list */
void CMapPluginSpeedwalk::addSpeedwalkRoom(CMapRoom *room)
{
	CMapCmdSpeedwalkAdd *cmd = new CMapCmdSpeedwalkAdd(this,room);
	mapManager->addCommand(cmd);

}

/** Used to delete a room from the speed walk list. */
void CMapPluginSpeedwalk::delSpeedwalkRoom(CMapRoom *room)
{
	CMapCmdSpeedwalkRemove *cmd = new CMapCmdSpeedwalkRemove(this);
	cmd->addRoom(room);
	mapManager->addCommand(cmd);
}

/** Used to add a room to the speed walk list */
void CMapPluginSpeedwalk::addSpeedwalkRoomNoCmd(CMapRoom *room,bool update)
{
	if (!m_speedwalkRoomList.contains(room))
	{
		m_speedwalkRoomList.append(room);
	}

	if (m_speedwalkList && update)
	{
		m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList(room);
	}
}

/** This is called when the character or mud profiles change */
void CMapPluginSpeedwalk::profileChanged(void)
{
	m_showAction->setEnabled(true);
}

/** This method is called to recreate the speedwalk list */
void CMapPluginSpeedwalk::updateSpeedwalkList()
{
	if (m_speedwalkList)
	{
		m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList();
	}
}

/** Used to delete a room from the speed walk list. */
void CMapPluginSpeedwalk::delSpeedwalkRoomNoCmd(CMapRoom *room,bool update)
{
	m_speedwalkRoomList.remove(room);

	if (m_speedwalkList && update)
		m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList();	
}

/** Used to add the room under the point to the speedwalk list */
void CMapPluginSpeedwalk::slotRoomAddToSpeedwalk()
{
  addSpeedwalkRoom((CMapRoom *)mapManager->getActiveView()->getSelectedElement());
}

void CMapPluginSpeedwalk::slotViewSpeedwalkList()
{
	showSpeedwalkList();
}

/** This is called before a element menu is openend */
void CMapPluginSpeedwalk::beforeOpenElementMenu(CMapElement *element)
{
	m_addAction->setEnabled(!m_speedwalkRoomList.contains((CMapRoom*)element));
}

/** This is called before a element is deleted */
void CMapPluginSpeedwalk::beforeElementDeleted(CMapElement *element)
{
	if (element->getElementType()==ROOM)
	{
		delSpeedwalkRoom((CMapRoom *)element);
	}
}

/** This is called when a element is changed */
void CMapPluginSpeedwalk::elementChanged(CMapElement *element)
{
	if (element->getElementType()==ZONE)
	{
		if (m_speedwalkList)
		{
			CMapZone *zone = (CMapZone *)element;
			foreach (CMapRoom *room, m_speedwalkRoomList)
			{
				if (room->getZone()==zone)
				{
					m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList();
					break;
				}
			}
		}
	}

	if (element->getElementType()==ROOM)
	{
		if (m_speedwalkList)
		{
			if (m_speedwalkRoomList.contains((CMapRoom *)element))
				m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList((CMapRoom *)element);
		}
	}
}


void CMapPluginSpeedwalk::mapErased(void)
{
	m_speedwalkRoomList.clear();

	if (m_speedwalkList)
	{
		m_speedwalkList->getSpeedwalkList()->updateSpeedwalkList();
	}
}

/** This method is used to get a list of new properties for a element
  * It will usally be called when saving map data to file
  * @param element The element being saved
  * @param properties When method exits this should contain the new properties
  */
void CMapPluginSpeedwalk::saveElementProperties(CMapElement *element,KMemConfig *properties)
{
	if (element->getElementType()==ROOM)
	{
		if (m_speedwalkRoomList.contains((CMapRoom *)element))
		{
			properties->group("Properties").writeEntry("SpeedwalkRoom","Yes");
		}
	}
}

/** This method is used to update an element with the properties load from a file
  * It will usally be called when loading map data to file
  * @param element The element being loaded
  * @param properties The properties being loaded from the file
  */
void CMapPluginSpeedwalk::loadElementProperties(CMapElement *element,KMemConfig *properties)
{
	if (element->getElementType()==ROOM && properties->group("Properties").hasKey("SpeedwalkRoom"))
	{
		addSpeedwalkRoomNoCmd((CMapRoom *)element);
	}
}

