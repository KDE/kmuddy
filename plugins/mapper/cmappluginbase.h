/***************************************************************************
                               cmappluginbase.h
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

#ifndef CMAPPLUGINBASE_H
#define CMAPPLUGINBASE_H

#include <QList>
#include <kxmlguiclient.h>

#include "cmapelement.h"
#include "cmapview.h"

#include <kmuddy_export.h>

class CMapToolBase;
class CMapPropertiesPaneBase;
class CMapManager;
class CMapRoom;
class KmudSetupPane;
class KMemConfig;

/**This is the base class for mapper plugins
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapPluginBase : public QObject
{
   Q_OBJECT
public: 
	CMapPluginBase(QObject *parent=0);
	virtual ~CMapPluginBase();

	/** Used to get a list of the tools */
	virtual QList<CMapToolBase *> *getToolList(void);
	/** Used to get a list of the property pages for a map element */
	virtual QList<CMapPropertiesPaneBase *> createPropertyPanes(elementTyp type,CMapElement *element,QWidget *parent);
	virtual void createGlobalConfigPanes ();
	virtual void createProfileConfigPanes ();


	/** Used to tell the tool were to find the map manager */
	void setManager(CMapManager *manager);
	/** Used to get the map manager */
	CMapManager *getManager(void);

	/** This is called before a element menu is openend */
	virtual void beforeOpenElementMenu(CMapElement *element);
	/** This is called before a element is deleted */
	virtual void beforeElementDeleted(CMapElement *element);
	/** This method is called after undoing a delete action
	  * @param element The elemening being restored */
	virtual void afterElementUndeleted(CMapElement *element);
	/** This is called when a element is changed */
	virtual void elementChanged(CMapElement *element);
	virtual void saveConfigOptions(void);
	virtual void loadConfigOptions(void);
	virtual void mapErased(void);
	/** Request that a room be added to the speedwalk list
	 * @param room The room to be added */
	virtual void addSpeedwalkRoom(CMapRoom *) { };
	/** This is called when the character or mud profiles change */
	virtual void profileChanged(void) { };
	/** This method is called when profile data changes */
	virtual void profileDataChanged(const QString& , uint ) { }

	/**
	 * This is called when the map is about to be loaded from file
	 */
	virtual void loadAboutToStart() { }

 	/**
	 * This is called when the map is about to be saved to file
	 */
	virtual void saveAboutToStart(void) { }

	/**
	 * This is called when a new map is created
	 */
	virtual void newMapCreated(void) { }
	
	/** This method is used to get a list of new properties for a element
	  * It will usally be called when saving map data to file
	  * @param element The element being saved
	  * @param properties When method exits this should contain the new properties
	  */
	virtual void saveElementProperties(CMapElement *,KMemConfig *) { }
	/** This method is used to update an element with the properties load from a file
	  * It will usally be called when loading map data to file
	  * @param element The element being loaded
	  * @param properties The properties being loaded from the file
	  */
	virtual void loadElementProperties(CMapElement *,KMemConfig *) { }
	
	
protected:
	QList<CMapToolBase *>            toolList;

	CMapManager *mapManager;
};

#endif
