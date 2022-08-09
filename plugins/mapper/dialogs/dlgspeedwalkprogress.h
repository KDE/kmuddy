/***************************************************************************
                               dlgspeedwalkprogress.h
                             -------------------
    begin                : Wed May 30 2001
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

#ifndef DLGSPEEDWALKPROGRESS_H
#define DLGSPEEDWALKPROGRESS_H

#include <QDialog>
#include "ui_dlgspeedwalkprogressbase.h"

/**
  *@author Kmud Developer Team
  */

class DlgSpeedwalkProgress : public QDialog, private Ui::DlgSpeedwalkProgressBase
{
   Q_OBJECT
public: 
	DlgSpeedwalkProgress(QWidget *parent=nullptr);
	~DlgSpeedwalkProgress() override;

	void setProgress(int progress);
	void setTotalSteps(int steps);

	int getTotalSteps(void);

protected slots:
	void slotAbort();

signals:
	void abortSpeedwalk();
};

#endif
