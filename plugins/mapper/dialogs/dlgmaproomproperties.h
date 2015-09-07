/***************************************************************************
                          dlgmaproomproperties.h  -  description
                             -------------------
    begin                : Thu Mar 8 2001
    copyright            : (C) 2001 by KMud Development Team
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

#ifndef DLGMAPROOMPROPERTIES_H
#define DLGMAPROOMPROPERTIES_H

#include <QDialog>
#include "ui_dlgmaproompropertiesbase.h"

#include "../cmaproom.h"

/**The map room properties dialog
  *@author KMud Development Team
  */

class DlgMapRoomProperties : public QDialog, private Ui::DlgMapRoomPropertiesBase  {
   Q_OBJECT
public: 
	DlgMapRoomProperties(CMapManager *manager,CMapRoom *roomElement,QWidget *parent=0);
	~DlgMapRoomProperties();

private slots:
	void slotAccept();
	void slotUseDefaultColor(bool useDefaultColor);

    void slotRemoveItem();
	void slotAddItem();
	void slotNewItemSelected();
	void slotEditItemName(const QString & name);

    void slotPathDelete();
	void slotPathProperties();

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

private:
	void setLabelPos(CMapRoom::labelPosTyp position);
	CMapRoom::labelPosTyp getLabelPos(void);
	void regenerateExits(void);

private:
	CMapRoom *room;
	CMapManager *mapManager;
};

#endif
