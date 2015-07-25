/***************************************************************************
                               cmapclipboard.cpp
                             -------------------
    begin                : Wed Jul 3 2002
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

#include "cmapclipboard.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <klocale.h>

#include "cmapmanager.h"
#include "cmappath.h"
#include "cmapzone.h"
#include "cmaptext.h"
#include "cmaproom.h"
#include "cmaplevel.h"
#include "cmapviewbase.h"
#include "cmapcmdelementcreate.h"
#include "cmapcmdelementproperties.h"

#include "kmemconfig.h"

CMapClipboard::CMapClipboard(CMapManager *mapManager,QObject *parent) : QObject(parent)
{
	kDebug() << "CMapClipboard::CMapClipboard constructor begins";

	m_parent = parent;
	m_mapManager = mapManager;

	m_clipboard = NULL;
	initActions();
	kDebug() << "CMapClipboard::CMapClipboard constructor ends";
}

CMapClipboard::~CMapClipboard()
{
	if (m_clipboard)
		delete m_clipboard;
}

/** This is used to create the clipboard actions */
void CMapClipboard::initActions(void)
{
  // Edit menu
  m_editSelectAll = new KAction (m_parent);
  m_editSelectAll->setText (i18n ("Select All"));
  connect (m_editSelectAll, SIGNAL (triggered ()), this, SLOT (slotSelectAll ()));
  m_mapManager->actionCollection()->addAction ("editSelectAll", m_editSelectAll);
  m_editUnselectAll = new KAction (m_parent);
  m_editUnselectAll->setText (i18n ("Unselect All"));
  connect (m_editUnselectAll, SIGNAL (triggered ()), this, SLOT (slotUnselectAll ()));
  m_mapManager->actionCollection()->addAction ("editUnselectAll", m_editUnselectAll);
  m_editSelectInvert = new KAction (m_parent);
  m_editSelectInvert->setText (i18n ("Invert Selection"));
  connect (m_editSelectInvert, SIGNAL (triggered ()), this, SLOT (slotInvertSelection ()));
  m_mapManager->actionCollection()->addAction ("editSelectInvert", m_editSelectInvert);
  m_editDelete = new KAction (m_parent);
  m_editDelete->setText (i18n ("Delete"));
  connect (m_editDelete, SIGNAL (triggered ()), this, SLOT (slotDelete ()));
  m_mapManager->actionCollection()->addAction ("editDelete", m_editDelete);
  m_editCopy = new KAction (m_parent);
  m_editCopy->setText (i18n ("Copy"));
  connect (m_editCopy, SIGNAL (triggered ()), this, SLOT (slotCopy ()));
  m_mapManager->actionCollection()->addAction ("editCopy", m_editCopy);
  m_editCut = new KAction (m_parent);
  m_editCut->setText (i18n ("Cut"));
  connect (m_editCut, SIGNAL (triggered ()), this, SLOT (slotCut ()));
  m_mapManager->actionCollection()->addAction ("editCut", m_editCut);
  m_editPaste = new KAction (m_parent);
  m_editPaste->setText (i18n ("Paste"));
  connect (m_editPaste, SIGNAL (triggered ()), this, SLOT (slotPaste ()));
  m_mapManager->actionCollection()->addAction ("editPaste", m_editPaste);
}

/** This method is used to set the enabled start of the actions */
void CMapClipboard::enableActions(bool enabled)
{
	m_editSelectAll->setEnabled(enabled);
	m_editUnselectAll->setEnabled(enabled);
	m_editSelectInvert->setEnabled(enabled);
	m_editDelete->setEnabled(enabled);
	m_editCopy->setEnabled(enabled);
	m_editCut->setEnabled(enabled);
	m_editPaste->setEnabled(enabled);
}

/** This method is called to copy the selected elements into the clipboard */
void CMapClipboard::slotCopy()
{
  if (m_clipboard)
    delete m_clipboard;

  m_clipboard = new KMemConfig();

  if (m_mapManager->getActiveView())
  {
    int group = 0;
    // Copy all the elements except for paths that are selected
    CMapLevel *level = m_mapManager->getActiveView()->getCurrentlyViewedLevel();
    QList<CMapElement *> lst = level->getAllElements();
    foreach (CMapElement *element, lst)
    {
      if (!element->getSelected()) continue;
      group++;
      QString grp;
      grp.sprintf("%d",group);
      KConfigGroup clipGroup = m_clipboard->group(grp);

      switch (element->getElementType())
      {
        case ROOM : {
                      CMapRoom *room = (CMapRoom *)element;
                      room->saveProperties(clipGroup);
                      clipGroup.deleteEntry("RoomID");
                      clipGroup.writeEntry("LabelPos",(int)CMapRoom::HIDE);
                    }
                    break;
        case TEXT : if (!((CMapText *)element)->getLinkElement())
                    {
                      element->saveProperties(clipGroup);
                      clipGroup.deleteEntry("TextID");
                    }
                    break;
        default   : break;
      }
    }

    // Copy all the path elements into the clipboard
    int pathGroup = 0;

    for (unsigned int idx = 0; idx < m_mapManager->getZone()->levelCount(); ++idx)
    {
      CMapLevel *level = m_mapManager->getZone()->getLevel(idx);

      foreach (CMapRoom *room, *level->getRoomList())
      {
        foreach (CMapPath *path, *room->getPathList())
        {
          if ((path->getSrcRoom()->getSelected() || path->getSrcRoom()->getZone()->getSelected()) &&
              (path->getDestRoom()->getSelected() || path->getDestRoom()->getZone()->getSelected()))
          {	
            copyPath (&pathGroup,path);
          }
        }
      }
    }

    // Copy all the linked text elements
    int linkGroup =0;

    foreach (CMapElement *element, lst)
    {
      if (element->getSelected() && element->getElementType()==TEXT)
      {
        CMapText *text = (CMapText *)element;
        CMapElement *link = text->getLinkElement();
        if (link)
        {
          linkGroup++;
          QString grp;
          grp.sprintf("LINK%d",linkGroup);
          KConfigGroup clipGroup = m_clipboard->group(grp);

          text->saveProperties(clipGroup);

          clipGroup.writeEntry("LinkLevelNum",link->getLevel()->getNumber());

          clipGroup.writeEntry("LinkX",link->getX());
          clipGroup.writeEntry("LinkY",link->getY());
          clipGroup.writeEntry("LinkZone",link->getZone()->getZoneID());
        }
      }
    }

    // Copy the selected paths
    KConfigGroup header = m_clipboard->group("Header");
    header.writeEntry("Elements",group);
    header.writeEntry("Paths",pathGroup);
    header.writeEntry("Links",linkGroup);
  }
}

/** This method is used to copy a path into the clipboard */                     	
void CMapClipboard::copyPath(int *pathGroup,CMapPath *path)
{
	(*pathGroup)++;
	QString grp;
	grp.sprintf("PATH%d",*pathGroup);
        KConfigGroup pGroup = m_clipboard->group(grp);

	path->saveProperties(pGroup);
	pGroup.writeEntry("SrcLevelNum",path->getSrcRoom()->getLevel()->getNumber());
	pGroup.writeEntry("DestLevelNum",path->getDestRoom()->getLevel()->getNumber());
	pGroup.writeEntry("SrcX",path->getSrcRoom()->getX());
	pGroup.writeEntry("SrcY",path->getSrcRoom()->getY());
	pGroup.writeEntry("DestX",path->getDestRoom()->getX());
	pGroup.writeEntry("DestY",path->getDestRoom()->getY());
	pGroup.writeEntry("SrcZone",path->getSrcRoom()->getZone()->getZoneID());
	pGroup.writeEntry("DestZone",path->getDestRoom()->getZone()->getZoneID());
}

/** This method is called to copy the selected elements into the clipboard, then delete them */
void CMapClipboard::slotCut()
{
	slotCopy();
	slotDelete();
}

/** This method is called to paste the elements in the clipboard onto the current map */
void CMapClipboard::slotPaste()
{	
	m_mapManager->openCommandGroup("Paste");
	
	if (m_clipboard && m_mapManager->getActiveView())
	{
		pasteElements();
		pastePaths();
		pasteLinks();
	}


	m_mapManager->closeCommandGroup();
}

/** This method is used to paste elements that are not paths or linked text elements */
void CMapClipboard::pasteElements()
{

	// Paste the non path elements

	// Get the number of elements in the clipboard	
        KConfigGroup header = m_clipboard->group("Header");
	int elements = header.readEntry("Elements",0);
	if (elements>0)
	{
		// Interate through each element in the clipboard
		for (int i=1;i<=elements; i++)
		{
			// Change to the group for the current element
			QString grp;
			grp.sprintf("%d",i);
                        KConfigGroup group = m_clipboard->group(grp);
	
			// Get the level number in the zone that the element is to be insterted into
			int levelNum = group.readEntry("LevelNum",-5);

			// Get the zone that the element came from
			int orgZone = group.readEntry("Zone",-5);
			if (levelNum == -5 || orgZone == -5)
			{
				// The level num was not found so this is not a zone copy
			}
			else
			{
			    CMapZone *zone = m_mapManager->getZone();

				// Check to see if the level exists and if not create it
				CMapLevel *level = NULL;
				if (levelNum >= (int)zone->levelCount())
				{
					level = m_mapManager->createLevel(UP);
				}
				else
				{
					level = zone->getLevel(levelNum);
				}

				// Update the clipboard with the new level that the element is to be pasted into
				group.writeEntry("Level",level->getLevelID());
			}

			// Copy the properties to new properties the remove unwanted keys
			KMemConfig properties;
                        KConfigGroup props = properties.group("Properties");
			group.copyTo(&props);

			int x = props.readEntry("X",-5);
			int y = props.readEntry("Y",-5);

			props.writeEntry("X",x + m_mapManager->getMapData()->gridSize.width());
			props.writeEntry("Y",y + m_mapManager->getMapData()->gridSize.height());

			// Create the command used to add a new element
			CMapCmdElementCreate *command = new CMapCmdElementCreate(m_mapManager,i18n("Paste Element"));

			command->addElement(&properties,"Properties");
	
			// Execute the commmand
			m_mapManager->addCommand(command);

			// Check the created elements to see if a zone was created
			QList<CMapElement *> *elements = command->getElements();

			foreach (CMapElement *el, *elements)
			{
				// Update the elements properties
				CMapCmdElementProperties *cmd = new CMapCmdElementProperties(m_mapManager,i18n("Update Properties"),el);
				cmd->setNewProperties(props);
				m_mapManager->addCommand(cmd);
			}
		}
	}
}

/** This method is used to paste path elements */
void CMapClipboard::pastePaths()
{
	// Paste the path elements
	// Get the number of elements in the clipboard	
        KConfigGroup header = m_clipboard->group("Header");
	int paths = header.readEntry("Paths",0);

	if (paths>0)
	{
		// Interate through each element in the clipboard
		for (int i=1;i<=paths; i++)
		{
			// Change to the group for the current element
			QString grp;
			grp.sprintf("PATH%d",i);
                        KConfigGroup group = m_clipboard->group(grp);

			int srcLevelNum = group.readEntry("SrcLevelNum",-5);
			int destLevelNum = group.readEntry("DestLevelNum",-5);

			if (srcLevelNum==-5 || destLevelNum ==-5)
			{
				// Should never get here
			}
			else
			{
				// Lookup witch zone the element is to be pasted into
			    CMapZone *zone = m_mapManager->getZone();

				// Check to see if the level exists and if not create it
				CMapLevel *srcLevel = zone->getLevel(srcLevelNum);
				CMapLevel *destLevel = zone->getLevel(destLevelNum);

				KMemConfig properties;
                                KConfigGroup props = properties.group("Properties");
			        group.copyTo(&props);

				int srcX = props.readEntry("SrcX",-5);
				int srcY = props.readEntry("SrcY",-5);

				srcX+=m_mapManager->getMapData()->gridSize.width();
				srcY+=m_mapManager->getMapData()->gridSize.height();

				int destX = props.readEntry("DestX",-5);
				int destY = props.readEntry("DestY",-5);

				destX+=m_mapManager->getMapData()->gridSize.width();
				destY+=m_mapManager->getMapData()->gridSize.height();

				// Update the clipboard with the new level that the element is to be pasted into
				props.writeEntry("SrcRoom", srcLevel->findRoomAt(QPoint(srcX,srcY))->getRoomID());
				props.writeEntry("DestRoom", destLevel->findRoomAt(QPoint(destX,destY))->getRoomID());
				props.writeEntry("SrcLevel",srcLevel->getLevelID());
				props.writeEntry("DestLevel",destLevel->getLevelID());

				// Create the command used to add a new element and the execute it
				CMapCmdElementCreate *command = new CMapCmdElementCreate(m_mapManager,i18n("Paste Path"));
				command->addElement(&properties);
				m_mapManager->addCommand(command);
			}
		}
	}
}

/** This method is used to update linked text elements with the correct properties from the clibboard */
void CMapClipboard::pasteLinks()
{
	// Update link elements with the correct properties
        KConfigGroup header = m_clipboard->group("Header");
	int links = header.readEntry("Links",0);

	if (links>0)
	{
		// Interate through each element in the clipboard
		for (int i=1;i<=links; i++)
		{
			// Change to the group for the current element
			QString grp;
			grp.sprintf("LINK%d",i);
                        KConfigGroup group = m_clipboard->group(grp);

			int linkLevelNum = group.readEntry("LinkLevelNum",-5);

			if (linkLevelNum==-5)
			{
				// Should never get here
			}
			else
			{
				// Get the zone that the element came from
			    CMapZone *zone = m_mapManager->getZone();

				// Check to see if the level exists and if not create it
				CMapLevel *linkLevel = zone->getLevel(linkLevelNum);	

				// Copy link text element properties to new properties
				KMemConfig properties;
                                KConfigGroup props = properties.group("Properties");
			        group.copyTo(&props);

				// Get cordianets of text element and linked element, and adjust them for any offsets
				int x = props.readEntry("LinkX",-5);
				int y = props.readEntry("LinkY",-5);

				x+=m_mapManager->getMapData()->gridSize.width();
				y+=m_mapManager->getMapData()->gridSize.height();

				int textX = props.readEntry("X",-5);
				int textY = props.readEntry("Y",-5);
				props.writeEntry("X",textX + m_mapManager->getMapData()->gridSize.width());
				props.writeEntry("Y",textY + m_mapManager->getMapData()->gridSize.height());

                // Setup the properties so that the text element will be linked to the new element
				CMapElement *link = linkLevel->findElementAt(QPoint(x,y));
				if (link->getElementType()==ZONE)
				{
					props.writeEntry("LinkedID",((CMapZone *)link)->getZoneID());
				}
				else
				{
					props.writeEntry("LinkedID",((CMapRoom *)link)->getRoomID());
				}
				props.writeEntry("LinkedLevel",link->getLevel()->getLevelID());

				// Create the command used to add a new element
				CMapCmdElementCreate *command = new CMapCmdElementCreate(m_mapManager,i18n("Paste Element"));
				command->addElement(&properties,"Properties");
				m_mapManager->addCommand(command);

				// Check the created elements to see if a zone was created
				QList<CMapElement *> *elements=command->getElements();

				foreach (CMapElement *el, *elements)
				{
					// Update the elements properties
					CMapCmdElementProperties *cmd = new CMapCmdElementProperties(m_mapManager,i18n("Update Properties"),el);
					cmd->setNewProperties(props);
					m_mapManager->addCommand(cmd);
				}
			}
		}
	}
}


/** This slot is called to delete all the selected objects in the current view */
void CMapClipboard::slotDelete(void)
{
  m_mapManager->openCommandGroup(i18n("Delete Elements"));
  CMapViewBase *currentView = m_mapManager->getActiveView();
  if (currentView)
  {
    CMapLevel *level = currentView->getCurrentlyViewedLevel();

    if (level)
    {
      QList<CMapElement *> lst = level->getAllElements();
      foreach (CMapElement *el, lst) {
        CMapRoom *room = dynamic_cast<CMapRoom *>(el);
        if (room) {
          QList<CMapPath *> paths = *room->getPathList(); // create a copy
          foreach (CMapPath *path, paths)
          {
            if (path->getSelected())
              m_mapManager->deleteElement(path);
          }
          paths = *room->getConnectingPathList();
          foreach (CMapPath *path, paths)
          {
            if (path->getSelected())
              m_mapManager->deleteElement(path);
          }
        }

        if (el->getSelected())
          m_mapManager->deleteElement(room);
      }
    }
  }

  m_mapManager->closeCommandGroup();
}

/** This slot is called when the select all menu option is selected */
void CMapClipboard::slotSelectAll(void)
{
  if (m_mapManager->getActiveView())
  {
    CMapLevel *level = m_mapManager->getActiveView()->getCurrentlyViewedLevel();
    QList<CMapElement *> lst = level->getAllElements();
    foreach (CMapElement *element, lst)
      element->setSelected(true);

    m_mapManager->levelChanged(level);
  }
}

/** This slot is called when the unselect all menu option is selected */
void CMapClipboard::slotUnselectAll(void)
{
	if (m_mapManager->getActiveView())
	{
		m_mapManager->unselectElements(m_mapManager->getActiveView()->getCurrentlyViewedLevel());

		m_mapManager->levelChanged(m_mapManager->getActiveView()->getCurrentlyViewedLevel());
	}
}

/** This slot is called when the invert selection menu option is called */
void CMapClipboard::slotInvertSelection(void)
{
  if (!m_mapManager->getActiveView()) return;
  CMapLevel *level = m_mapManager->getActiveView()->getCurrentlyViewedLevel();

  QList<CMapElement *> lst = level->getAllElements();
  foreach (CMapElement *element, lst)
    element->setSelected(!element->getSelected());

  m_mapManager->levelChanged(m_mapManager->getActiveView()->getCurrentlyViewedLevel());
}

