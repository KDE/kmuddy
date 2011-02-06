/***************************************************************************
                               cmaptoolbase.h
                             -------------------
    begin                : Tue May 1 2001
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

#ifndef CMAPTOOLBASE_H
#define CMAPTOOLBASE_H

#include <qobject.h>
#include <qstring.h>
#include <qicon.h>
#include <qevent.h>
//Added by qt3to4:
#include <QKeyEvent>

#include <kaction.h>
#include <ktoggleaction.h>

#include <kmuddy_export.h>

class CMapManager;
class CMapLevel;
class CMapViewBase;

class QActionGroup;
class KActionCollection;

/**
  *@author Kmud Developer Team
  */

class KMUDDY_EXPORT CMapToolBase : public QObject
{
        Q_OBJECT
public:
	/** The construtor for the tool */
	CMapToolBase(KActionCollection *actionCollection,QString description,QIcon icon,CMapManager *manager,QString actionName,QActionGroup *group, QObject *parent=0);
	virtual ~CMapToolBase();

	// Mouse events for the tool
	virtual void mousePressEvent(QPoint ,CMapLevel *)                       { }
	virtual void mouseReleaseEvent(QPoint ,CMapLevel *)                     { }
	virtual void mouseMoveEvent(QPoint ,Qt::ButtonState,CMapLevel *)        { }
	virtual void mouseLeaveEvent()                                           { }
	virtual void mouseEnterEvent()                                           { }
	virtual void keyPressEvent(QKeyEvent *)                                            { }
	virtual void keyReleaseEvent(QKeyEvent *)                                          { }

	/** This method is called when the active view changes */
	virtual void viewChangedEvent(CMapViewBase *view)=0;

	/** This function called when a tool is selected */
	virtual void toolSelected(void)=0;
	/** This function is called when a tool is unselected */
	virtual void toolUnselected(void)                                               { }
	/** Used to tell the tool were to find the map manager */
	void setManager(CMapManager *manager);
	/** Used to set the checked state of the tool */	
	void setChecked(bool checked);

	void plug (QWidget *w, int index=-1);

protected:
	/** Used to set the whats this text for the tool */
	void setWhatsThis(QString text);	
	/** Used to set the toop tip of the tool */
	void setToolTip(QString tip);

protected:
	/** A pointer to the map manager */
	CMapManager *mapManager;


	

signals:
	/** This is emited when the tool is selected */
	void actionSelected(CMapToolBase *tool);

private slots:
	/** This is called when the tool button is clicked */
	void slotActionSelected();

private:
	/** The action used by the tool */
	KToggleAction *action;

};

#endif
