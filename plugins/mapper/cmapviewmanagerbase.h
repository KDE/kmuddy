/***************************************************************************
                               cmapviewmanagerbase.h
                             -------------------
    begin                : Tue Aug 7 2001
    copyright            : (C) 2001 by Kmud Developer Team
                         : (C) 2007 Tomas Mecir <kmuddy @kmuddy.com>
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

#ifndef CMAPVIEWMANAGERBASE_H
#define CMAPVIEWMANAGERBASE_H

#include <qobject.h>
#include <qstring.h>
#include <qicon.h>

#include <kaction.h>
#include <kactioncollection.h>

class CMapViewBase;
class CMapManager;

/**This is the base class of classes used to manager map views
  *@author Kmud Developer Team
  */

class CMapViewManagerBase : public QObject
{
   Q_OBJECT
public: 
	CMapViewManagerBase(CMapManager *manager,QString actionName,QString description, KActionCollection *actionCollection,QIcon icon,QObject *parent=0, const char *name=0);
	CMapViewManagerBase(CMapManager *manager,QString actionName,QString description, KActionCollection *actionCollection,QObject *parent=0, const char *name=0);
	~CMapViewManagerBase();
signals:
	/** This is emited when a new view is opened */
	void newViewOpened(CMapViewBase *view);

protected:
	/** Used to set the whats this text for the view */
	void setWhatsThis(QString text);	
	/** Used to set the toop tip of the view */
	void setToolTip(QString tip);
	/** This is used to show a view */
	void showView(CMapViewBase *view);

protected slots:
	/** This is called when the view is to be opended */
	virtual void slotRequestNewView()=0;

protected:
	CMapManager *mapManager;

private:
	/** The action name */
	QString actName;
	/** The action used to open the view */
	KAction *action;
	/** The view that is being managed */
	CMapViewBase *managedView;
};

#endif
