/***************************************************************************
                               dlgmapmovement.cpp
                             -------------------
    begin                : Tue Mar 18 2003
    copyright            : (C) 2003 by Kmud Developer Team
                           (C) 2007 Tomas Mecir <kmuddy@kmuddy.com>
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

#include "dlgmapmovement.h"

#include "../cmapmanager.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QListWidget>
#include <qinputdialog.h>

#include <KLocalizedString>

DlgMapMovement::DlgMapMovement(CMapManager *mapManager, QWidget *parent) : QDialog(parent)
{
  setupUi (this);
  connect(this, SIGNAL(accepted()), this, SLOT(slotOkPressed()));

	m_mapManager = mapManager;

	m_chkEnableValidRoomChecking->setChecked(m_mapManager->getMapData()->validRoomCheck);

	slotValidCheckStateChanged(m_mapManager->getMapData()->validRoomCheck);

        QStringList::iterator it;
        for (it = mapManager->getMapData()->failedMoveMsg.begin();
             it != mapManager->getMapData()->failedMoveMsg.end(); ++it)
	{
		QString str = *it;
		if (!str.isEmpty())
                  m_lstInvalidMoveStrs->addItem(str);
	}

	connect(m_chkEnableValidRoomChecking,SIGNAL(toggled(bool)),this,SLOT(slotValidCheckStateChanged(bool)));
	connect(m_cmdAdd,SIGNAL(clicked()),this,SLOT(slotAddClicked()));
	connect(m_cmdEdit,SIGNAL(clicked()),this,SLOT(slotEditClicked()));
	connect(m_cmdRemove,SIGNAL(clicked()),this,SLOT(slotRemoveClicked()));
}

DlgMapMovement::~DlgMapMovement()
{
}

/** This slot is called when the OK button is pressed */
void DlgMapMovement::slotOkPressed()
{
	m_mapManager->getMapData()->validRoomCheck =  m_chkEnableValidRoomChecking->isChecked();

	m_mapManager->getMapData()->failedMoveMsg.clear();

	for (int i=0; i< m_lstInvalidMoveStrs->count();i++)
	{
		m_mapManager->getMapData()->failedMoveMsg.append(m_lstInvalidMoveStrs->item(i)->text());
	}
}

void DlgMapMovement::slotValidCheckStateChanged(bool state)
{
	m_cmdAdd->setEnabled(state);
	m_cmdEdit->setEnabled(state);
	m_cmdRemove->setEnabled(state);
	m_lstInvalidMoveStrs->setEnabled(state);
}

void DlgMapMovement::slotAddClicked(void)
{
  bool ok = false;	

  QString text = QInputDialog::getText(this, i18n("KMuddy"), i18n("Enter invalid movement string as a regular expression"), QLineEdit::Normal, QString(), &ok);
  if ( ok && !text.isEmpty() )
    m_lstInvalidMoveStrs->addItem(text);
}

void DlgMapMovement::slotEditClicked(void)
{
  int current =  m_lstInvalidMoveStrs->currentRow();
  if (current!=-1)
  {
    bool ok;
    QString text = QInputDialog::getText(this, i18n("KMuddy"), i18n("Enter invalid movement string as a regular expression"), QLineEdit::Normal, m_lstInvalidMoveStrs->item(current)->text(), &ok);

    if ( ok && !text.isEmpty() )
    {
      m_lstInvalidMoveStrs->item(current)->setText(text);
    }
  }
}

void DlgMapMovement::slotRemoveClicked(void)
{
  QListWidgetItem *item = m_lstInvalidMoveStrs->currentItem();
  if (item)
    m_lstInvalidMoveStrs->removeItemWidget(item);
}

#include "moc_dlgmapmovement.cpp"
