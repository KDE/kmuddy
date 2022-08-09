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

#include <QStatusBar>

class QPushButton;

class CMapZone;
class CMapLevel;
class CMapManager;

/**
  *@author Kmud Developer Team
  */

class CMapViewStatusbar : public QStatusBar
{
   Q_OBJECT
public: 
  CMapViewStatusbar(CMapManager *manager, QWidget *parent=nullptr);
  ~CMapViewStatusbar();

  void setLevel(CMapLevel *level);
  void setZone(CMapZone *zone);
  void setRoom(QString room);

  void addFollowButton(QPushButton *button);
private slots:
  void changeLevel(int index);
  void renameLevel(const QString &name);
  void changeZone(int index);
  void renameZone(const QString &name);
private:
  struct Private;
  Private *d;
};

#endif
