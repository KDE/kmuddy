/***************************************************************************
                               dlgmapcolor.h
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

#ifndef DLGMAPCOLOR_H
#define DLGMAPCOLOR_H

#include "ui_dlgmapcolorbase.h"

class CMapManager;

/**
  *@author Kmud Developer Team
  */

class DlgMapColor : public QWidget, public Ui_DlgMapColorBase
{
   Q_OBJECT
public: 
	DlgMapColor (CMapManager *mapManager, QWidget *parent);
	~DlgMapColor ();

	/** This slot is called when the OK button is pressed */
	void slotOkPressed();

private:
	/** This method will load the color values */
	void loadColors();

private:
	/** A pointer to the map manager */
	CMapManager *m_mapManager;
};

#endif
