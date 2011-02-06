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
#include <q3listbox.h>
#include <qinputdialog.h>

#include <klocale.h>
#include <kregexpeditorinterface.h>
#include <kparts/componentfactory.h>
#include <kvbox.h>

DlgMapMovement::DlgMapMovement(CMapManager *mapManager, QWidget *parent) : QWidget (parent)
{
  setupUi (this);
	m_mapManager = mapManager;

	m_chkEnableValidRoomChecking->setChecked(m_mapManager->getMapData()->validRoomCheck);

	slotValidCheckStateChanged(m_mapManager->getMapData()->validRoomCheck);

        QStringList::iterator it;
        for (it = mapManager->getMapData()->failedMoveMsg.begin();
             it != mapManager->getMapData()->failedMoveMsg.end(); ++it)
	{
		QString str = *it;
		if (!str.isEmpty())
		{
			new Q3ListBoxText(m_lstInvalidMoveStrs,str);
		}
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
		m_mapManager->getMapData()->failedMoveMsg.append(m_lstInvalidMoveStrs->text(i));
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
	
	QString text = "";
	bool ok = false;	

        QDialog *editorDialog = KServiceTypeTrader::createInstanceFromQuery<QDialog>("KRegExpEditor/KRegExpEditor");
	if ( editorDialog )
	{
		// kdeutils was installed, so the dialog was found fetch the editor interface
		KRegExpEditorInterface *editor = dynamic_cast<KRegExpEditorInterface *>(editorDialog);
		Q_ASSERT( editor ); // This should not fail!

		// now use the editor.
		editor->setRegExp("");

		// Finally exec the dialog
		if (editorDialog->exec())
		{
			ok = true;
			text = editor->regExp();
		}
	}
	else
	{
		text = QInputDialog::getText( i18n("KMuddy"), i18n("Enter invalid movement string as a regular expression"), QLineEdit::Normal,
	                                      QString(), &ok, this );
	}

    if ( ok && !text.isEmpty() )
    {
		new Q3ListBoxText(m_lstInvalidMoveStrs,text);
	}
}

void DlgMapMovement::slotEditClicked(void)
{
	int current =  m_lstInvalidMoveStrs->currentItem();
	if (current!=-1)
	{
		bool ok;
		QString text = "";

                QDialog *editorDialog = KServiceTypeTrader::createInstanceFromQuery<QDialog>("KRegExpEditor/KRegExpEditor");
		if ( editorDialog )
		{
			// kdeutils was installed, so the dialog was found fetch the editor interface
			KRegExpEditorInterface *editor = dynamic_cast<KRegExpEditorInterface *>(editorDialog);
			Q_ASSERT( editor ); // This should not fail!

			// now use the editor.
			editor->setRegExp("");

			// Finally exec the dialog
			if (editorDialog->exec())
			{
				ok = true;
				text = editor->regExp();
			}
		}
		else
		{

			text = QInputDialog::getText( i18n("Kmud"), i18n("Enter invalid movement string as a regular expression"), QLineEdit::Normal,
		                                     m_lstInvalidMoveStrs->text(current), &ok, this );
		}

		if ( ok && !text.isEmpty() )
		{
			m_lstInvalidMoveStrs->changeItem(text,current);
		}
	}
}

void DlgMapMovement::slotRemoveClicked(void)
{
	int current =  m_lstInvalidMoveStrs->currentItem();
	if (current!=-1)
	{
		m_lstInvalidMoveStrs->removeItem(current);
	}
}

