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

#include <QDebug>
#include <KLocalizedString>

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
	qDebug() << "CMapCmdMoveElements::addElement 1";
	elemProp properties;
	properties.pos = QPoint(element->getX(),element->getY());
	qDebug() << "CMapCmdMoveElements::addElement 1.0 : " << element->getX();
        CMapLevel *level = element->getLevel();
        if (!level) return;
	properties.level = level->getLevelID();

	qDebug() << "CMapCmdMoveElements::addElement 1.1";
	if (element->getElementType()==TEXT)
	{
		qDebug() << "CMapCmdMoveElements::addElement 1.2";
		CMapElement *lnkElement = ((CMapText *) element)->getLinkElement();
		if (lnkElement!=nullptr)
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
		qDebug() << "CMapCmdMoveElements::addElement 2";
		properties.labelPos = 0;
	}

	qDebug() << "CMapCmdMoveElements::addElement 3";	
	elements.push_back(properties);
	qDebug() << "CMapCmdMoveElements::addElement 4";
}

void CMapCmdMoveElements::redo()
{
  CMapLevel *level = nullptr;	
  for( elemProp &prop : elements )
  {
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

void CMapCmdMoveElements::undo()
{
  CMapLevel *level = nullptr;
  for( elemProp &prop : elements )
  {
    level = m_mapManager->findLevel(prop.level);
    CMapElement *elm = level->findElementAt(prop.pos + m_offset);
    if (elm)
      elm->moveBy(-m_offset);
  }

  if (level)
    m_mapManager->levelChanged(level);
}
