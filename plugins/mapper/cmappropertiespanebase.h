/***************************************************************************
                               cmappropertiespanebase.h
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

#ifndef CMAPPROPERTIESPANEBASE_H
#define CMAPPROPERTIESPANEBASE_H

#include <qwidget.h>
#include <qicon.h>
#include <kmuddy_export.h>

#include "cmapelement.h"

/**This is the base class for properties panes used by the mapper plugins
  *@author Kmud Developer Team
  */

class KMUDDY_EXPORT CMapPropertiesPaneBase : public QWidget
{
   Q_OBJECT
public: 
	CMapPropertiesPaneBase(QString title,QIcon *icon,elementTyp panelType,CMapElement *element,QWidget *parent=nullptr);
	~CMapPropertiesPaneBase() override             {}

	/** Set the panel title */
	void setTitle(QString title)          { paneName = title; }
	/** Get the panel title */
	QString getTitle(void)                { return paneName; }
	/** Set the panel icon */
	void setIcon(QIcon *icon)           { paneIcon = icon; }
	/** Get the panel icon */
	QIcon *getIcon(void)                { return paneIcon; }
	/** Set the panel type */
	void setPanelType(elementTyp type)    { panelType = type; }
	/** Get the panel type */
	elementTyp getPanelType(void)         { return panelType; }
	/** Set the element the properties are being changed for */
	void setElement(CMapElement *element) { elm = element; }

public slots:
	/** This is called when the ok button of the property dialog is pressed */
	virtual void slotOk()=0;
	/** This is called when the cancel button of the property dialog is pressed */
	virtual void slotCancel()=0;

private:
	/** Used to store the panel name */
	QString paneName;
	/** Used to store the panel icon */
	QIcon *paneIcon;
	/** Used to store the panel type */
	elementTyp panelType;
	/** The element being altered */
	CMapElement *elm;
};

#endif
