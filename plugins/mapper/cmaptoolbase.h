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

#include <kmuddy_export.h>

class CMapManager;
class CMapLevel;

class QActionGroup;
class KActionCollection;
class KToggleAction;

/**
  *@author Kmud Developer Team
  */

class KMUDDY_EXPORT CMapToolBase : public QObject
{
        Q_OBJECT
public:
	/** The construtor for the tool */
	CMapToolBase(KActionCollection *actionCollection,QString description,QIcon icon,CMapManager *manager,QString actionName,QActionGroup *group);
	~CMapToolBase() override;

	// Mouse events for the tool
	virtual void mousePressEvent(QPoint, QMouseEvent *, CMapLevel *)                       { }
	virtual void mouseReleaseEvent(QPoint, QMouseEvent *, CMapLevel *)                     { }
	virtual void mouseMoveEvent(QPoint, Qt::KeyboardModifiers, Qt::MouseButtons, CMapLevel *)        { }
	virtual void mouseDoubleClickEvent(QPoint, QMouseEvent *, CMapLevel *)                  { }
	virtual void mouseLeaveEvent()                                           { }
	virtual void mouseEnterEvent()                                           { }
	virtual void keyPressEvent(QKeyEvent *)                                            { }
	virtual void keyReleaseEvent(QKeyEvent *)                                          { }

        /** Called when painting the widget */
        virtual void paint(QPainter *)   { }

	/** This function called when a tool is selected */
	virtual void toolSelected(void)=0;
	/** This function is called when a tool is unselected */
	virtual void toolUnselected(void)                                               { }
	/** Used to tell the tool were to find the map manager */
	void setManager(CMapManager *manager);
	/** Used to set the checked state of the tool */	
	void setChecked(bool checked);

	void plug (QWidget *w);

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
