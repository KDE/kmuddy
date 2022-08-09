/***************************************************************************
                          dlgstatistics.h  -  Statistics dialog
                             -------------------
    begin                : Pi mar 14 2003
    copyright            : (C) 2003 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGSTATISTICS_H
#define DLGSTATISTICS_H

#include <QDockWidget>
#include "cactionbase.h"

class QTimer;
class QTreeWidget;

/**
Connection statistics dialog box.
  *@author Tomas Mecir
  */

class dlgStatistics : public QDockWidget, public cActionBase {
   Q_OBJECT
public: 
  dlgStatistics (QWidget *parent=nullptr);
  ~dlgStatistics () override;
  /** update information in the dialog */
public slots:
  void update ();
protected:
  void eventIntHandler (QString event, int, int, int) override;

  QTimer *updateTimer;

  QTreeWidget *view;
};

#endif
