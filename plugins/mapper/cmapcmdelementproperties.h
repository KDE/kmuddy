/***************************************************************************
                               cmapcmdelementproperties.h
                             -------------------
    begin                : Wed Feb 27 2002
    copyright            : (C) 2002 by Kmud Developer Team
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

#ifndef CMAPCMDELEMENTPROPERTIES_H
#define CMAPCMDELEMENTPROPERTIES_H


#include <k3command.h>

#include <qstringlist.h>
#include <qfont.h>
#include <qpoint.h>
#include <kvbox.h>

#include "cmapcommand.h"
#include "cmapelementutil.h"

#include "kmemconfig.h"
#include <kconfiggroup.h>

class CMapElement;
class CMapManager;
class CMapLevel;
class CMapRoom;
class CMapZone;
class CMapText;
class CMapPath;

class CMapCmdElementProperties : public CMapCommand
{
public:
	CMapCmdElementProperties(CMapManager *mapManager,QString name,CMapElement *element);
	~CMapCmdElementProperties();
	virtual void execute();
	virtual void unexecute();
	KConfigGroup getOrgProperties(void);
	KConfigGroup getNewProperties(void);

	/** Used to store check if the values are different and store then if they are */
	void compare(QString id,QString orgValue,QString dialogValue);
	void compare(QString id,bool orgValue,bool dialogValue);
	void compare(QString id,QColor orgValue,QColor dialogValue);
	void compare(QString id,int orgValue,int dialogValue);
	void compare(QString id,QStringList orgValue,QStringList dialogValue);
	void compare(QString id,QFont orgValue,QFont dialogValue);
	void compare(QString id,QSize orgValue,QSize dialogValue);

	void setNewProperties(KConfigGroup newProperties);
	void setOldProperties(KConfigGroup oldProperties);

private:
	CMapManager *m_manager;
	KMemConfig *m_properties;
};

#endif
