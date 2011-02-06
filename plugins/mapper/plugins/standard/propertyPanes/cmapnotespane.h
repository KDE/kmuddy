/***************************************************************************
                               cmapnotespane.h
                             -------------------
    begin                : Wed Aug 8 2001
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

#ifndef CMAPNOTESPANE_H
#define CMAPNOTESPANE_H

#include <qstring.h>
#include <qicon.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3multilineedit.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <kvbox.h>

#include "../../../cmapelement.h"
#include "../../../cmappropertiespanebase.h"

class CMapPluginStandard;

/**This pane adds the ability to save notes for rooms and zones
  *@author Kmud Developer Team
  */

class CMapNotesPane : public CMapPropertiesPaneBase
{
public: 
	CMapNotesPane(CMapPluginStandard *plugin,QString title,QIcon *icon,elementTyp panelType,CMapElement *element,QWidget *parent, const char *name);
	~CMapNotesPane();

public slots:
	/** This is called when the ok button of the property dialog is pressed */
	void slotOk();
	/** This is called when the cancel button of the property dialog is pressed */
	void slotCancel();

private:
	CMapElement *m_element;
	/** The components of the pane */
	Q3VBoxLayout *notesLayout;	
	QLabel *lblNotes;
	Q3MultiLineEdit *txtNotes;
	CMapPluginStandard *m_plugin;

};

#endif
