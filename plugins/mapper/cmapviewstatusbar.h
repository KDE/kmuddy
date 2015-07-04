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
#include <QLabel>

class QLabel;
class QPushButton;
class QHBoxLayout;

/**
  *@author Kmud Developer Team
  */

class CMapViewStatusbar : public QStatusBar
{
   Q_OBJECT
public: 
  CMapViewStatusbar(QWidget *parent=0);
  ~CMapViewStatusbar();

  void setLevel(int level);
  void setZone(QString zone);
  void setRoom(QString room);

  void addFollowButton(QPushButton *button);

private:
  struct Private;
  Private *d;
};

#endif
