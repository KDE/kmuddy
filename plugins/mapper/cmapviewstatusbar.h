/***************************************************************************
                               cmapmudviewstatusbar.h
                             -------------------
    begin                : Thu May 10 2001
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

#ifndef CMAPMUDVIEWSTATUSBAR_H
#define CMAPMUDVIEWSTATUSBAR_H

#include <q3frame.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3HBoxLayout>
#include <kvbox.h>

class QLabel;
class QPushButton;
class Q3HBoxLayout;

/**
  *@author Kmud Developer Team
  */

class CMapViewStatusbar : public Q3Frame
{
   Q_OBJECT
public: 
	CMapViewStatusbar(QWidget *parent=0, const char *name=0);
	~CMapViewStatusbar();

	void setLevel(int level);
	void setZone(QString zone);
	void setRoom(QString room);

	void addViewIndicator(QLabel *indicator);
	void addFollowButton(QPushButton *button);

private:
	Q3HBoxLayout *layout;

	QLabel *lblRoomLabel;
	QLabel *lblRoomStatus;
	QLabel *lblLevelLabel;
	QLabel *lblLevelStatus;
	QLabel *lblZoneLabel;
	QLabel *lblZoneStatus;
};

#endif
