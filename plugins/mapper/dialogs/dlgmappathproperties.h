/***************************************************************************
                          dlgmappathproperties.h  -  description
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

#ifndef DLGMAPPATHPROPERTIES_H
#define DLGMAPPATHPROPERTIES_H

#include <qwidget.h>

#include "dlgmappathpropertiesbase.h"

#include "../cmapelement.h"
#include <kconfiggroup.h>

class CMapPath;
class CMapManager;

/**The map path properties dialog
  *@author KMud Development Team
  */

class DlgMapPathProperties : public DlgMapPathPropertiesBase
{
   Q_OBJECT
public: 
	DlgMapPathProperties(CMapManager *manager,KConfigGroup pathProperties,bool undoable=true,QWidget *parent=0, const char *name=0);
	DlgMapPathProperties(CMapManager *manager,CMapPath *pathElement,bool undoable=true,QWidget *parent=0, const char *name=0);

	bool getTwoWay(void);
	QString getSpecialCmdDest(void);
	QString getDestAfterCmd(void);
	QString getDestBeforeCmd(void);


	~DlgMapPathProperties();

private slots:
	void slotAccept();
	void slotDirectionChange();
	void slotExitTypeChange();

	void slotDestDown();
	void slotDestE();
	void slotDestN();
	void slotDestNE();
	void slotDestNW();
	void slotDestS();
	void slotDestSE();
	void slotDestSW();
	void slotDestUp();
	void slotDestW();

	void slotSrcDown();
	void slotSrcE();
	void slotSrcN();
	void slotSrcNE();
	void slotSrcNW();
	void slotSrcS();
	void slotSrcSE();
	void slotSrcSW();
	void slotSrcUp();
	void slotSrcW();

private:
	void propertiesAccept(QString cmdName);
	void pathAccept(QString cmdName);
	void setSrcDirection(directionTyp direction);
	void setDestDirection(directionTyp direction);
	directionTyp getSrcDirection(void);
	directionTyp getDestDirection(void);

private:
	bool useProperties;
	KConfigGroup properties;
	bool pathUndoable;
	CMapPath *path;
	CMapManager *mapManager;
};

#endif
