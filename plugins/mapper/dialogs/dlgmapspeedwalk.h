/***************************************************************************
                               dlgmapspeedwalk.h
                             -------------------
    begin                : Tue Jul 9 2002
    copyright            : (C) 2002 by Kmud Developer Team
                           (C) 2007 Tomas Mecir <kmuddy@kmuddy.com>
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

#ifndef DLGMAPSPEEDWALK_H
#define DLGMAPSPEEDWALK_H

class CMapManager;
#include <QWidget>
#include "ui_dlgmapspeedwalkbase.h"

/**
  *@author Kmud Developer Team
  */

class DlgMapSpeedwalk : public QWidget, public Ui::DlgMapSpeedwalkBase
{
   Q_OBJECT
public: 
	DlgMapSpeedwalk(CMapManager *mapManager, QWidget *parent);
	~DlgMapSpeedwalk();

	virtual void slotOkPressed();

private:
	/** A pointer to the map manager */
	CMapManager *m_mapManager;
};

#endif
