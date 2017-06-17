/***************************************************************************
                               cmappluginspeedwalk.h
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

#ifndef CMAPPLUGINSPEEDWALK_H
#define CMAPPLUGINSPEEDWALK_H

#include "../../cmappluginbase.h"


#include <QList>

class CMapManager;
class CMapRoom;
class CMapElement;
class DlgMapSpeedwalkContainter;

/**This plugin provides the speedwalk list
  *@author Kmud Developer Team
  */

class CMapPluginSpeedwalk : public CMapPluginBase
{
   Q_OBJECT
public: 
	CMapPluginSpeedwalk(QObject *parent);
	~CMapPluginSpeedwalk();

	/** This is called before a element menu is openend */
	virtual void beforeOpenElementMenu(CMapElement *element);
	/** This is called before a element is deleted */
	virtual void beforeElementDeleted(CMapElement *element);
	/** This is called when a element is changed */
	virtual void elementChanged(CMapElement *element);
	/** Used to hide the speedwalk list */
	void hideSpeedwalkList();
	/** Used to display the speedwalk list
	  * actually returns DlgMapSpeedwalkList*, dynamic_cast if necessary
	  */
	void showSpeedwalkList();

	int getSpeedwalkCatogrize();

	void setSpeedwalkCatogrize(int catogry);

	void saveConfigOptions(void);
	void loadConfigOptions(void);

	virtual void mapErased(void);

	/** Used to add a room to the speed walk list */
	void addSpeedwalkRoom(CMapRoom *room);
	/** Used to delete a room from the speed walk list. */
	void delSpeedwalkRoom(CMapRoom *room);
	/** Used to add a room to the speed walk list */
	void addSpeedwalkRoomNoCmd(CMapRoom *room,bool update = true);
	/** Used to delete a room from the speed walk list. */
	void delSpeedwalkRoomNoCmd(CMapRoom *room,bool update = true);

	/** This method is called to recreate the speedwalk list */
	void updateSpeedwalkList();
	
	/** This is called when the character or mud profiles change */
	void profileChanged(void);

	/** This method is used to get a list of new properties for a element
	  * It will usally be called when saving map data to file
	  * @param element The element being saved
	  * @param properties When method exits this should contain the new properties
	  */
	void saveElementProperties(CMapElement *element,KMemConfig *properties);

	/** This method is used to update an element with the properties load from a file
	  * It will usally be called when loading map data to file
	  * @param element The element being loaded
	  * @param properties The properties being loaded from the file
	  */
	void loadElementProperties(CMapElement *element,KMemConfig *properties);

        QList<CMapRoom *> *getSpeedwalkRoomList() { return &m_speedwalkRoomList; }

private:
	/** Used to store a list of rooms in the speedwalk list */
	QList<CMapRoom *> m_speedwalkRoomList;
	/** Used to say what catogery is being used to display the speed walk list */
	int m_speedwalkCatogrize;
	/** The speedwalk list dialog */
	DlgMapSpeedwalkContainter *m_speedwalkList;
	/** This action is used to add a room to the speedwalk list */
	KAction *m_addAction;
	/** This action is used to show the speedwalk list */
	KAction *m_showAction;

private slots:
	void slotViewSpeedwalkList();
	void slotRoomAddToSpeedwalk();
};

#endif
