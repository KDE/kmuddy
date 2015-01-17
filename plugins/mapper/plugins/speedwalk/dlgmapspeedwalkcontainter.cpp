/***************************************************************************
                               dlgmapspeedwalkcontainter.cpp
                             -------------------
    begin                : Sun May 13 2001
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

#include "dlgmapspeedwalkcontainter.h"

#include <qlayout.h>
#include <qevent.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <Q3GridLayout>
#include <klocale.h>

#include "dlgmapspeedwalklist.h"
#include "cmappluginspeedwalk.h"
#include "../../cmapmanager.h"

DlgMapSpeedwalkContainter::DlgMapSpeedwalkContainter(CMapPluginSpeedwalk *plugin,QWidget *parent) : KDialog(parent)
{
  setCaption (i18n ("Speedwalk List"));

  m_plugin = plugin;

  Q3GridLayout *vbox = new Q3GridLayout(this,1,1);

  list = new DlgMapSpeedwalkList(plugin,this,"speedlist");

  vbox->addWidget(list,0,0);
}

DlgMapSpeedwalkContainter::~DlgMapSpeedwalkContainter()
{
}

void DlgMapSpeedwalkContainter::closeEvent(QCloseEvent *)
{
	m_plugin->hideSpeedwalkList();
}

void DlgMapSpeedwalkContainter::slotWidgetBeingClosed()
{
	m_plugin->hideSpeedwalkList();
}

void DlgMapSpeedwalkContainter::slotDockWindowClose()
{
	m_plugin->hideSpeedwalkList();
}

DlgMapSpeedwalkList *DlgMapSpeedwalkContainter::getSpeedwalkList(void)
{
	return list;
}
