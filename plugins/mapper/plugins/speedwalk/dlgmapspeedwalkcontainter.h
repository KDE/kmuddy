/***************************************************************************
                               dlgmapspeedwalkcontainter.h
                             -------------------
    begin                : Sun May 13 2001
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

#ifndef DLGMAPSPEEDWALKCONTAINTER_H
#define DLGMAPSPEEDWALKCONTAINTER_H

#include <kdialog.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <kvbox.h>

class CMapPluginSpeedwalk;
class DlgMapSpeedwalkList;

/**
  *@author Kmud Developer Team
  */

class DlgMapSpeedwalkContainter : public KDialog
{
   Q_OBJECT
public: 
	DlgMapSpeedwalkContainter(CMapPluginSpeedwalk *plugin,QWidget *parent=0);
	~DlgMapSpeedwalkContainter();
	
	DlgMapSpeedwalkList *getSpeedwalkList(void);

protected:
	void closeEvent(QCloseEvent *e);

protected slots:
	void slotDockWindowClose();
	void slotWidgetBeingClosed();

private:
	DlgMapSpeedwalkList *list;
	CMapPluginSpeedwalk *m_plugin;
	
};

#endif
