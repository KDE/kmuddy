/***************************************************************************
                          dlgmapinfo.h  -  description
                             -------------------
    begin                : Wed Mar 7 2001
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

#ifndef DLGMAPINFO_H
#define DLGMAPINFO_H

#include <qwidget.h>
#include "ui_dlgmapinfobase.h"

class CMapManager;

/**The map infomation dialog
  *@author KMud Development Team
  */

class DlgMapInfo : public QDialog, private Ui_DlgMapInfoBase
{
   Q_OBJECT
public: 
	DlgMapInfo(CMapManager *,QWidget *parent=0, const char *name=0);
	~DlgMapInfo();

	QString intToStr(int num);
};

#endif
