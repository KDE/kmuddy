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
//Added by qt3to4:
#include <Q3PtrList>

CMapClipboard::CMapClipboard(CMapManager *mapManager,QObject *parent, const char *name ) : QObject(parent,name)
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
		for (CMapElement *element = level->getFirstElement() ; element!=0; element=level->getNextElement())
		{
			if (element->getSelected())
			{
				group++;
				QString grp;
				grp.sprintf("%d",group);
                                KConfigGroup clipGroup = m_clipboard->group(grp);

				switch (element->getElementType())
				{
					case ZONE : {
					            	CMapZone *zone = (CMapZone *)element;
					            	copyZone(&group,zone,clipGroup);
					            	clipGroup.writeEntry("LabelPos",(int)CMapZone::HIDE);
					            }
					
					            break;
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
		}
		
		// Copy all the path elements into the clipboard
		int pathGroup = 0;

		for (CMapZone *zone = m_mapManager->getMapData()->getFirstZone(); zone!=0; zone=m_mapManager->getMapData()->getNextZone())
		{
			for (CMapLevel *level = zone->getLevels()->first(); level!=0; level=zone->getLevels()->next())
			{
				for (CMapRoom *room = level->getRoomList()->first(); room!=0; room=level->getRoomList()->next())
				{
					for (CMapPath *path = room->getPathList()->first(); path!=0; path = room->getPathList()->next())
					{
						if ((path->getSrcRoom()->getSelected() || path->getSrcRoom()->getZone()->getSelected()) &&
						    (path->getDestRoom()->getSelected() || path->getDestRoom()->getZone()->getSelected()))
						{	
							copyPath (&pathGroup,path);
						}
					}
				}
			}
		}

		// Copy all the linked text elements
		int linkGroup =0;

		for (CMapElement *element = level->getFirstElement() ; element!=0; element=level->getNextElement())
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

/** This method is used to copy a zone */
void CMapClipboard::copyZone(int *group,CMapZone *orgZone, KConfigGroup configGroup)
{
	// Save the zone
	orgZone->saveProperties(configGroup);

	// Copy the elements of the zone
	for (CMapLevel *level=orgZone->getLevels()->first(); level !=0;level = orgZone->getLevels()->next())
	{
		// Copy any zones in the zone
		for (CMapZone *zone=level->getZoneList()->first();zone !=0 ; zone = level->getZoneList()->next())
		{
			(*group)++;
			QString grp;
			grp.sprintf("%d",*group);
			copyZone(group,zone,m_clipboard->group(grp));
			m_clipboard->group(grp).writeEntry("LevelNum",zone->getLevel()->getNumber());
			m_clipboard->group(grp).writeEntry("LabelPos",(int)CMapZone::HIDE);
		}

		// Copy any rooms in the zone
		for (CMapRoom *room=level->getRoomList()->first();room !=0 ; room = level->getRoomList()->next())
		{
			(*group)++;
			QString grp;
			grp.sprintf("%d",*group);

			room->saveProperties(m_clipboard->group(grp));
			m_clipboard->group(grp).writeEntry("LevelNum",room->getLevel()->getNumber());
			m_clipboard->group(grp).deleteEntry("RoomID");
			m_clipboard->group(grp).writeEntry("LabelPos",(int)CMapRoom::HIDE);
		}

		// Copy any text elements in the zone
		for (CMapText *text=level->getTextList()->first();text !=0 ; text = level->getTextList()->next())
		{
			if (!text->getLinkElement())
			{
				(*group)++;
				QString grp;
				grp.sprintf("%d",*group);

				text->saveProperties(m_clipboard->group(grp));
				m_clipboard->group(grp).writeEntry("LevelNum",text->getLevel()->getNumber());
				m_clipboard->group(grp).deleteEntry("TextID");	
			}
		}
	}
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
	m_zoneListOrg.clear();
	m_zoneListNew.clear();

	unsigned int currentZoneID = m_mapManager->getActiveView()->getCurrentlyViewedZone()->getZoneID();

	m_mapManager->openCommandGroup("Paste");
	
	if (m_clipboard && m_mapManager->getActiveView())
	{
		pasteElements(currentZoneID);
		pastePaths(currentZoneID);
		pasteLinks(currentZoneID);
	}


	m_mapManager->closeCommandGroup();
}

/** This method is used to paste elements that are not paths or linked text elements */
void CMapClipboard::pasteElements(unsigned int currentZoneID)
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
    			// Lookup witch zone the element is to be pasted into
			    CMapZone *zone = NULL;
				int count= 0;
				for (IntList::Iterator it = m_zoneListOrg.begin(); it != m_zoneListOrg.end(); ++it )
				{
					if ((*it)==orgZone)
					{
						zone = m_mapManager->findZone(m_zoneListNew[count]);
						break;
					}
					count++;
				}

				// Check to see if the level exsits and if not create it
				CMapLevel *level = NULL;
				if (levelNum>=(int)zone->getLevels()->count())
				{
					level = m_mapManager->createLevel(UP,zone);
				}
				else
				{
					level = zone->getLevels()->at(levelNum);
				}

				// Update the clipboard with the new level that the element is to be pasted into
				group.writeEntry("Level",level->getLevelID());
			}

			// Copy the properties to new properties the remove unwanted keys
			KMemConfig properties;
                        KConfigGroup props = properties.group("Properties");
			group.copyTo(&props);

			props.deleteEntry("ZoneID");

			// if this is element is to be pated into the current zone then offset the elments
			if (((unsigned int)props.readEntry("Zone",-1))==currentZoneID)
			{
				int x = props.readEntry("X",-5);
				int y = props.readEntry("Y",-5);

				props.writeEntry("X",x + m_mapManager->getMapData()->gridSize.width());
				props.writeEntry("Y",y + m_mapManager->getMapData()->gridSize.height());
			}

			// Create the command used to add a new element
			CMapCmdElementCreate *command = new CMapCmdElementCreate(m_mapManager,i18n("Paste Element"));

			command->addElement(&properties,"Properties");
	
			// Execute the commmand
			m_mapManager->addCommand(command);

			// Check the created elements to see if a zone was created
			Q3PtrList<CMapElement> *elements=command->getElements();

			for (CMapElement *el = elements->first(); el!=0; el = elements->next())
			{
				// if a zone was created then save the origal id and the new id in the look table
				// so element that are to be pasted into the zone can find the new id of the zone
				if (el->getElementType()==ZONE)
				{
					int zoneID = group.readEntry("ZoneID",-5);

					m_zoneListOrg.append(zoneID);
					m_zoneListNew.append(((CMapZone *)el)->getZoneID());
				}

				// Update the elements properties
				CMapCmdElementProperties *cmd = new CMapCmdElementProperties(m_mapManager,i18n("Update Properties"),el);
				cmd->setNewProperties(props);
				m_mapManager->addCommand(cmd);
			}
		}
	}
}

/** This method is used to paste path elements */
void CMapClipboard::pastePaths(unsigned int currentZoneID)
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
				// Get the zone that the element came from
				int srcOrgZone = group.readEntry("SrcZone",-5);
				int destOrgZone = group.readEntry("DestZone",-5);

				// Lookup witch zone the element is to be pasted into
			    CMapZone *srcZone = NULL;
				CMapZone *destZone = NULL;

				int count= 0;
				for (IntList::Iterator it = m_zoneListOrg.begin(); it != m_zoneListOrg.end(); ++it )
				{
					if ((*it)==srcOrgZone)
					{
						srcZone = m_mapManager->findZone(m_zoneListNew[count]);
					}

					if ((*it)==destOrgZone)
					{
						destZone = m_mapManager->findZone(m_zoneListNew[count]);
					}

					if (srcZone && destZone)
					{
						break;
					}
					count++;
				}

				if (!srcZone)
				{
					srcZone = m_mapManager->getActiveView()->getCurrentlyViewedZone();
				}

				if (!destZone)
				{
					destZone = m_mapManager->getActiveView()->getCurrentlyViewedZone();
				}

				// Check to see if the level exsits and if not create it
				CMapLevel *srcLevel = srcZone->getLevels()->at(srcLevelNum);
				CMapLevel *destLevel = destZone->getLevels()->at(destLevelNum);

				KMemConfig properties;
                                KConfigGroup props = properties.group("Properties");
			        group.copyTo(&props);

				int srcX = props.readEntry("SrcX",-5);
				int srcY = props.readEntry("SrcY",-5);

				if (srcZone->getZoneID()==currentZoneID)
				{
					srcX+=m_mapManager->getMapData()->gridSize.width();
					srcY+=m_mapManager->getMapData()->gridSize.height();
				}

				int destX = props.readEntry("DestX",-5);
				int destY = props.readEntry("DestY",-5);

				if (destZone->getZoneID()==currentZoneID)
				{
					destX+=m_mapManager->getMapData()->gridSize.width();
					destY+=m_mapManager->getMapData()->gridSize.height();
				}

				// Update the clipboard with the new level that the element is to be pasted into
				props.writeEntry("SrcRoom",m_mapManager->findRoomAt(QPoint(srcX,srcY),srcLevel)->getRoomID());
				props.writeEntry("DestRoom",m_mapManager->findRoomAt(QPoint(destX,destY),destLevel)->getRoomID());
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
void CMapClipboard::pasteLinks(unsigned int currentZoneID)
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
				int linkOrgZone = group.readEntry("LinkZone",-5);

				// Lookup witch zone the element is to be pasted into
			    CMapZone *linkZone = NULL;

				int count= 0;
				for (IntList::Iterator it = m_zoneListOrg.begin(); it != m_zoneListOrg.end(); ++it )
				{
					if ((*it)==linkOrgZone)
					{
						linkZone = m_mapManager->findZone(m_zoneListNew[count]);
						break;
					}
					count++;
				}

				if (!linkZone)
				{
					linkZone = m_mapManager->getActiveView()->getCurrentlyViewedZone();
				}

				// Check to see if the level exsits and if not create it
				CMapLevel *linkLevel = linkZone->getLevels()->at(linkLevelNum);	

				// Copy link text element properties to new properties
				KMemConfig properties;
                                KConfigGroup props = properties.group("Properties");
			        group.copyTo(&props);

				// Get cordianets of text element and linked element, and adjust them for any offsets
				int x = props.readEntry("LinkX",-5);
				int y = props.readEntry("LinkY",-5);

				if (linkZone->getZoneID()==currentZoneID)
				{
					x+=m_mapManager->getMapData()->gridSize.width();
					y+=m_mapManager->getMapData()->gridSize.height();

					int textX = props.readEntry("X",-5);
					int textY = props.readEntry("Y",-5);
					props.writeEntry("X",textX + m_mapManager->getMapData()->gridSize.width());
					props.writeEntry("Y",textY + m_mapManager->getMapData()->gridSize.height());
				}

                // Setup the properties so that the text element will be linked to the new element
				CMapElement *link = m_mapManager->findElementAt(QPoint(x,y),linkLevel);
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
				Q3PtrList<CMapElement> *elements=command->getElements();

				for (CMapElement *el = elements->first(); el!=0; el = elements->next())
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
			for (int i = level->getRoomList()->count()-1 ; i>=0 ; i--)
			{
				CMapRoom *room = level->getRoomList()->at(i);

				for (int i2 = room->getPathList()->count() -1; i2>=0 ; i2--)
				{
					CMapPath *path = room->getPathList()->at(i2);

					if (path->getSelected())
					{
						m_mapManager->deleteElement(path);
					}
				}

				for (int i2 = room->getConnectingPathList()->count() -1; i2>=0 ; i2--)
				{
					CMapPath *path = room->getConnectingPathList()->at(i2);
					if (path->getSelected())
					{
						m_mapManager->deleteElement(path);
					}
				}

				if (room->getSelected())
				{
					level->getRoomList()->next();
					m_mapManager->deleteElement(room);
				}
				
			}

			for (int i = level->getTextList()->count()-1 ; i>=0 ; i --)
			{	
				CMapText *text = level->getTextList()->at(i);
				if (text->getSelected())
				{
					m_mapManager->deleteElement(text);
				}
			}

			for (int i = level->getZoneList()->count()-1 ; i>=0 ; i --)
			{	
				CMapZone *zone = level->getZoneList()->at(i);

				if (zone->getSelected())
				{
					m_mapManager->deleteElement(zone);
				}
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
		for (CMapElement *element = level->getFirstElement() ; element!=0; element=level->getNextElement())
		{
			element->setSelected(true);
		}	

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
	if (m_mapManager->getActiveView())
	{
		CMapLevel *level = m_mapManager->getActiveView()->getCurrentlyViewedLevel();
		for (CMapElement *element = level->getFirstElement() ; element!=0; element=level->getNextElement())
		{
			element->setSelected(!element->getSelected());
		}	

		m_mapManager->levelChanged(m_mapManager->getActiveView()->getCurrentlyViewedLevel());
	}
}

