/***************************************************************************
                               cmappluginstandard.h
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

#ifndef CMAPPLUGINSTANDARD_H
#define CMAPPLUGINSTANDARD_H

#include <QMap>
#include <list>

#include "../../cmappluginbase.h"

class CMapManager;

/**This plugin provides the standard mapper tools and views
  *@author Kmud Developer Team
  */

class CMapPluginStandard : public CMapPluginBase
{
   Q_OBJECT
public: 
	CMapPluginStandard(QObject *);
	~CMapPluginStandard() override;

        QString tagName() override { return QString("standard"); }

	QList<CMapPropertiesPaneBase *> createPropertyPanes(elementTyp type,CMapElement *element,QWidget *parent) override;

	void profileChanged(void) override;
	/** This is called before a element is deleted
	  * @param element The element about to be deleted */
	void beforeElementDeleted(CMapElement *element) override;
	/** This method is called after undoing a delete action
	  * @param element The elemening being restored */
	void afterElementUndeleted(CMapElement *element) override;
	/**
	 * This method is used to add a note or change a exsiting note
	 * @param elemenet The element the note is for
	 * @param note The next of the note
	 */
	void addNote(CMapElement *element,QString note);

	/** This method is used to remove a note
	  * @param element The note to remove
	  */
	void removeNote(CMapElement *element);
	
	/**
	 * This method is used to get a note for the given element
	 * @param element The element to get the note of
	 * @return The note or empty string if there is no note
	 */
	QString getNote(CMapElement *element);

	/** This method is used to get a list of new properties for a element
	  * It will usally be called when saving map data to file
	  * @param element The element being saved
	  * @param properties When method exits this should contain the new properties
	  */
	void saveElementProperties(CMapElement *element,KMemConfig *properties) override;

	/** This method is used to update an element with the properties load from a file
	  * It will usally be called when loading map data to file
	  * @param element The element being loaded
	  * @param properties The properties being loaded from the file
	  */
	void loadElementProperties(CMapElement *element,KMemConfig *properties) override;

	/**
	 * This is called when the map is about to be loaded from file
	 */
	void loadAboutToStart() override;

 	/**
	 * This is called when the map is about to be saved to file
	 */
	void saveAboutToStart(void) override;

	/**
	 * This is called when a new map is created
	 */
	void newMapCreated(void) override;


private:
	struct DeletedElement
	{
		int type;
		int id;
		int level;
		QString note;
	};

	typedef std::list<DeletedElement> DeletedElementList;

	DeletedElementList::iterator findRoom(int level,int id,bool *found);
	DeletedElementList::iterator findZone(int id,bool *found);	
	
private:
  QMap<CMapElement *, QString> m_noteList;
  DeletedElementList m_deletedElements;
};

#endif
