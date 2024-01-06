/***************************************************************************
                               cmappluginstandard.cpp
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

#include "cmappluginstandard.h"

#include "tools/cmaptoolselect.h"
#include "tools/cmaptoolroom.h"
#include "tools/cmaptoolpath.h"
#include "tools/cmaptooltext.h"
#include "tools/cmaptooleraser.h"
#include "propertyPanes/cmapnotespane.h"

#include "../../cmapmanager.h"
#include "../../cmaplevel.h"
#include "../../cmapzone.h"
#include "../../cmaproom.h"
#include "../../cmaptext.h"
#include "../../cmappath.h"
#include "../../cmapview.h"

#include <QIcon>

#include <klocale.h>
#include <kactioncollection.h>

CMapPluginStandard::CMapPluginStandard(QObject *parent) : CMapPluginBase(parent)
{
  CMapView *view = dynamic_cast<CMapView *>(parent);
        CMapManager *manager = view->getManager();
        KActionCollection *acol = view->actionCollection();

	// Create and Add the tools to the tools list
	toolList.append(new CMapToolSelect(acol,manager));
	toolList.append(new CMapToolRoom(acol,manager));
	toolList.append(new CMapToolPath(acol,manager));
	toolList.append(new CMapToolText(acol,manager));
	toolList.append(new CMapToolEraser(acol,manager));

	acol->action("toolsEraser")->setEnabled(true);
	acol->action("toolsPath")->setEnabled(true);
	acol->action("toolsRoom")->setEnabled(true);
	acol->action("toolsSelect")->setEnabled(true);
	acol->action("toolsText")->setEnabled(true);
}

CMapPluginStandard::~CMapPluginStandard()
{
}

/** Used to get a list of the property pages for a map element */
QList<CMapPropertiesPaneBase *> CMapPluginStandard::createPropertyPanes(elementTyp type,CMapElement *element,QWidget *parent)
{
  QList<CMapPropertiesPaneBase *> list;

  if (type == ROOM || type == ZONE)
    list.append(new CMapNotesPane(this,i18n("Notes"),nullptr,type,element,parent));

  return list;
}

/** This is called when the character or mud profiles change */
void CMapPluginStandard::profileChanged(void)
{
  /*
	actionCollection()->action("toolsEraser")->setEnabled(true);
	actionCollection()->action("toolsPath")->setEnabled(true);
	actionCollection()->action("toolsRoom")->setEnabled(true);
	actionCollection()->action("toolsSelect")->setEnabled(true);
	actionCollection()->action("toolsText")->setEnabled(true);
        */
}

/**
 * This method is used to add a note or change a exsiting note
 * @param elemenet The element the note is for
 * @param note The next of the note
 */
void CMapPluginStandard::addNote(CMapElement *element,QString note)
{
  if (note.isEmpty()) removeNote(element);
  else m_noteList[element] = note;
}

/** This method is used to remove a note
  * @param element The note to remove
  */
void CMapPluginStandard::removeNote(CMapElement *element)
{
  m_noteList.remove(element);
}

/**
 * This method is used to get a note for the given element
 * @param element The element to get the note of
 * @return The note or empty string if there is no note
 */
QString CMapPluginStandard::getNote(CMapElement *element)
{
  return m_noteList.value(element);
}

/** This method is used to get a list of new properties for a element
  * It will usally be called when saving map data to file
  * @param element The element being saved
  * @param properties When method exits this should contain the new properties
  */
void CMapPluginStandard::saveElementProperties(CMapElement *element,KMemConfig *properties)
{
  QString note = getNote(element);
  if (note.isEmpty()) return;
  properties->group("Properties").writeEntry("Note",note);
}

/** This method is used to update an element with the properties load from a file
  * It will usally be called when loading map data to file
  * @param element The element being loaded
  * @param properties The properties being loaded from the file
  */
void CMapPluginStandard::loadElementProperties(CMapElement *element,KMemConfig *properties)
{
  if (properties->group("Properties").hasKey("Note"))
  {
    QString note = properties->group("Properties").readEntry("Note","");
    addNote(element,note);
  }
}

/** This is called before a element is deleted
  * @param element The element about to be deleted */
void CMapPluginStandard::beforeElementDeleted(CMapElement *element)
{
  QString note = getNote(element);
  if (note.isEmpty()) return;

  DeletedElement e;
  e.type = (int)element->getElementType();

  if (element->getElementType() == ROOM)
  {
    e.id = ((CMapRoom *)element)->getRoomID();
    e.level = element->getLevel()->getLevelID();
    e.note = note;
    m_deletedElements.append(e);
  }

  if (element->getElementType() == ZONE)
  {
    e.id = ((CMapZone *)element)->getZoneID();
    e.note = note;
    m_deletedElements.append(e);
  }

  removeNote(element);
}

/** This method is called after undoing a delete action
  * @param element The elemening being restored */
void CMapPluginStandard::afterElementUndeleted(CMapElement *element)
{
	DeletedElementList::iterator e;
    bool found = false;

	if (element->getElementType() == ROOM)
	{
		e = findRoom(element->getLevel()->getLevelID(),((CMapRoom*)element)->getRoomID(),&found);
	}

	if (element->getElementType()== ZONE)
	{
		e = findZone(((CMapZone*)element)->getZoneID(),&found);
	}
	
	if (found)
	{		
		addNote(element,(*e).note);
		m_deletedElements.erase(e);
	}
}

/**
 * This is called when the map is about to be loaded from file
 */
void CMapPluginStandard::loadAboutToStart()
{
  m_deletedElements.clear();
  m_noteList.clear();
}

/**
 * This is called when the map is about to be saved to file
 */
void CMapPluginStandard::saveAboutToStart(void)
{
  m_deletedElements.clear();
}

/**
 * This is called when a new map is created
 */
void CMapPluginStandard::newMapCreated(void)
{
  m_deletedElements.clear();
  m_noteList.clear();
}


CMapPluginStandard::DeletedElementList::iterator CMapPluginStandard::findRoom(int level,int id,bool *found)
{
	DeletedElementList::iterator it;
    for ( it = m_deletedElements.begin(); it != m_deletedElements.end(); ++it )
    {
		if ((*it).level == level && (*it).id == id)
		{
			*found = true;
			return it;
		}
    }
    
    return m_deletedElements.end();
}

CMapPluginStandard::DeletedElementList::iterator CMapPluginStandard::findZone(int id,bool *found)
{
	DeletedElementList::iterator it;
    for ( it = m_deletedElements.begin(); it != m_deletedElements.end(); ++it )
    {
		if ((*it).id == id)
		{
			*found = true;
			return it;
		}
    }
	
	return m_deletedElements.end();
}

#include "moc_cmappluginstandard.cpp"
