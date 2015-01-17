/***************************************************************************
                               cmapcmdmoveelements.cpp
                             -------------------
    begin                : Mon Jun 17 2002
    copyright            : (C) 2002 by Kmud Developer Team
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

#include "cmapcmdmoveelements.h"

#include <kdebug.h>
#include <klocale.h>

#include "../../cmapmanager.h"
#include "../../cmapelement.h"
#include "../../cmaptext.h"
#include "../../cmaplevel.h"

CMapCmdMoveElements::CMapCmdMoveElements(CMapManager *mapManager,QPoint offset) : CMapCommand(i18n("Move Elements"))
{
	m_mapManager = mapManager;
	m_offset = offset;
}

CMapCmdMoveElements::~CMapCmdMoveElements()
{
}

void CMapCmdMoveElements::addElement(CMapElement *element)
{
	kDebug() << "CMapCmdMoveElements::addElement 1";
	struct elemProp properties;
	properties.pos = QPoint(element->getX(),element->getY());
	kDebug() << "CMapCmdMoveElements::addElement 1.0 : " << element->getX();
        CMapLevel *level = element->getLevel();
        if (!level) return;
	properties.level = level->getLevelID();

	kDebug() << "CMapCmdMoveElements::addElement 1.1";
	if (element->getElementType()==TEXT)
	{
		kDebug() << "CMapCmdMoveElements::addElement 1.2";
		CMapElement *lnkElement = ((CMapText *) element)->getLinkElement();
		if (lnkElement!=NULL)
		{
			if (lnkElement->getElementType()==ROOM)
			{
				properties.labelPos = (int)((CMapRoom *)lnkElement)->getLabelPosition();
			}

			if (lnkElement->getElementType()==ZONE)
			{
				properties.labelPos = (int)((CMapZone *)lnkElement)->getLabelPosition();
			}
		}
	}
	else
	{
		kDebug() << "CMapCmdMoveElements::addElement 2";
		properties.labelPos = 0;
	}

	kDebug() << "CMapCmdMoveElements::addElement 3";	
	elements.append(properties);
	kDebug() << "CMapCmdMoveElements::addElement 4";
}

void CMapCmdMoveElements::execute()
{
  CMapLevel *level = NULL;	
  for( PropList::Iterator it = elements.begin(); it != elements.end(); ++it )
  {
    struct elemProp prop = *it;

    level = m_mapManager->findLevel(prop.level);
    CMapElement *elm = level->findElementAt(prop.pos);
    if (!elm) continue;
    elm->moveBy(m_offset);

    if (elm->getElementType()==TEXT)
    {
      CMapText *text = (CMapText *)elm;
      if (text->getLinkElement())
      {
        if (!text->getLinkElement()->getSelected())
        {
          if (text->getLinkElement()->getElementType()==ROOM)
          {
            CMapRoom *room = (CMapRoom *)text->getLinkElement();
            if (room->getLabelPosition()!=CMapRoom::HIDE)
            {
              room->setLabelPosition(CMapRoom::CUSTOM);
            }	
          }
          if (text->getLinkElement()->getElementType()==ZONE)
          {
            CMapZone *zone = (CMapZone *)text->getLinkElement();
            if (zone->getLabelPosition()!=CMapZone::HIDE)
            {
              zone->setLabelPosition(CMapZone::CUSTOM);
            }
          }
        }
      }
    }
  }

  if (level)
    m_mapManager->levelChanged(level);
}

void CMapCmdMoveElements::unexecute()
{
  CMapLevel *level = NULL;
  for( PropList::Iterator it = elements.begin(); it != elements.end(); ++it )
  {
    struct elemProp prop = *it;

    level = m_mapManager->findLevel(prop.level);
    CMapElement *elm = level->findElementAt(prop.pos + m_offset);
    if (elm)
      elm->moveBy(-m_offset);
  }

  if (level)
    m_mapManager->levelChanged(level);
}
