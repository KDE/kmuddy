/***************************************************************************
                               dlgmapdirections.h
                             -------------------
    begin                : Tue Nov 19 2002
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

#ifndef DLGMAPDIRECTIONS_H
#define DLGMAPDIRECTIONS_H

class CMapManager;

#include "ui_dlgmapdirectionsbase.h"
#include <QDialog>

/**
  *@author Kmud Developer Team
  */

class DlgMapDirections : public QDialog, private Ui::DlgMapDirectionsBase
{
	Q_OBJECT
public:
	DlgMapDirections(const CMapManager *mapManager, QWidget *parent);
	~DlgMapDirections();

	virtual void slotOkPressed();

private:
	/** A pointer to the map manager */
        const CMapManager *m_mapManager;
};

#endif
