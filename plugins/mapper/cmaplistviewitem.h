/***************************************************************************
                               cmaplistviewitem.h
                             -------------------
    begin                : Mon May 14 2001
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

#ifndef CMAPLISTVIEWITEM_H
#define CMAPLISTVIEWITEM_H

#include <q3listview.h>

#include <kmuddy_export.h>

class CMapElement;
class CMapRoom;
class CMapLevel;

class QString;
/**
  *@author Kmud Developer Team
  */

class KMUDDY_EXPORT CMapListViewItem : public Q3ListViewItem  {
public:
	CMapListViewItem( Q3ListView * parent );
    CMapListViewItem( Q3ListViewItem * parent );
    CMapListViewItem( Q3ListView * parent, Q3ListViewItem * after );
    CMapListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after );

    CMapListViewItem( Q3ListView * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
    CMapListViewItem( Q3ListViewItem * parent,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );

    CMapListViewItem( Q3ListView * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
    CMapListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after,
		   QString,     QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null,
		   QString = QString::null, QString = QString::null );
	~CMapListViewItem();

	void setElement(CMapElement *mapElement);
	CMapElement *getElement(void);
	CMapLevel *getLevel(void);
	void setLevel(CMapLevel *mpaLevel);

private:
	CMapElement *element;
	CMapLevel *level;

};

#endif
