/***************************************************************************
                               dlgspeedwalkprogress.cpp
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

#include "dlgspeedwalkprogress.h"

#include <q3progressbar.h>
#include <qpushbutton.h>
#include <kvbox.h>

DlgSpeedwalkProgress::DlgSpeedwalkProgress(QWidget *parent, const char *name ) : QDialog(parent,name)
{
  setupUi (this);
}

DlgSpeedwalkProgress::~DlgSpeedwalkProgress()
{
}

void DlgSpeedwalkProgress::setProgress(int progress)
{
	ProgressBar->setProgress(progress);
}

int DlgSpeedwalkProgress::getTotalSteps(void)
{
	return ProgressBar->totalSteps();
}

void DlgSpeedwalkProgress::setTotalSteps(int steps)
{
	ProgressBar->setTotalSteps(steps);
}

void DlgSpeedwalkProgress::slotAbort()
{
	emit abortSpeedwalk();
}
