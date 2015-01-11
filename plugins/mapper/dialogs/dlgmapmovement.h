/***************************************************************************
                               dlgmapmovement.h
                             -------------------
    begin                : Tue Mar 18 2003
    copyright            : (C) 2003 by Kmud Developer Team
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

#ifndef DLGMAPMOVEMENT_H
#define DLGMAPMOVEMENT_H

class CMapManager;

#include "ui_dlgmapmovementbase.h"
#include <QDialog>

/**
  *@author Kmud Developer Team
  */

class DlgMapMovement : public QDialog, public Ui::DlgMapMovementBase
{
	Q_OBJECT
public: 
	DlgMapMovement(CMapManager *mapManager, QWidget *parent);
	~DlgMapMovement();

	void slotOkPressed();

private slots:
	void slotValidCheckStateChanged(bool state);

	void slotAddClicked(void);
	void slotEditClicked(void);
	void slotRemoveClicked(void);
	
private:
	/** A pointer to the map manager */
	CMapManager *m_mapManager;
	
};

#endif
