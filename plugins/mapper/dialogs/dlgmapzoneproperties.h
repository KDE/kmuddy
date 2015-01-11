/***************************************************************************
                               dlgmapzoneproperties.h
                             -------------------
    begin                : Mon May 7 2001
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

#ifndef DLGMAPZONEPROPERTIES_H
#define DLGMAPZONEPROPERTIES_H


#include <QDialog>

#include "../cmapzone.h"
#include "ui_dlgmapzonepropertiesbase.h"

class CMapManager;

/**
  *@author Kmud Developer Team
  */

class DlgMapZoneProperties : public QDialog, private Ui::DlgMapZonePropertiesBase
{
   Q_OBJECT
public: 
	DlgMapZoneProperties(CMapManager *manager,CMapZone *zoneElement,QWidget *parent=0, const char *name=0);
	~DlgMapZoneProperties();

private slots:
	void slotAccept();
	void slotUseDefaultColor(bool useDefaultColor);

    void slotE();
    void slotHide();
    void slotN();
    void slotNE();
    void slotNW();
    void slotS();
    void slotSE();
    void slotSW();
    void slotW();
    void slotCustom();

	void slotBlackgroundOptChange();

private:
	/** Used to get the path that a item responds to */
	void setLabelPos(CMapZone::labelPosTyp position);
	CMapZone::labelPosTyp getLabelPos(void);

private:
	CMapZone *zone;
	CMapManager *mapManager;
};

#endif
