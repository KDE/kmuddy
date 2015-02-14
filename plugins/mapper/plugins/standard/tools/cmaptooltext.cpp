/***************************************************************************
                               cmaptexttool.cpp
                             -------------------
    begin                : Thu Aug 9 2001
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

#include "cmaptooltext.h"

#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

#include <qcursor.h>
//Added by qt3to4:
#include <QKeyEvent>

#include "../../../cmapmanager.h"
#include "../../../cmapviewbase.h"
#include "../../../cmaplevel.h"
#include "../../../cmapelement.h"
#include "../../../cmaptext.h"

#include <kdebug.h>


CMapToolText::CMapToolText(KActionCollection *actionCollection,CMapManager *manager,QObject *parent)
	: CMapToolBase(actionCollection,
                   i18n("Text"),
                   BarIcon("kmud_text.png"),
                   manager,"toolsText",0,parent)
{
	textCursor = new QCursor(Qt::IBeamCursor);
}

CMapToolText::~CMapToolText()
{
	delete textCursor;
}

/** Called when the tool recives a mouse release event */
void CMapToolText::mouseReleaseEvent(QPoint mousePos,CMapLevel *currentLevel)
{
  CMapText *text = 0;
  QList<CMapElement *> lst = currentLevel->elementsUnderMouse(mousePos);
  foreach (CMapElement *element, lst)
  {
    if (element->getElementType() != TEXT) continue;
    text = (CMapText *) element;
    text->setCursor(text->convertPosToCursor(mousePos));
    break;
  }

  if (!text) {
    text = mapManager->createText(mousePos,currentLevel);
    text->setCursor(QPoint(0,1));
  }
  mapManager->setEditElement(text);
}

/** This is called when a key is pressed */
void CMapToolText::keyPressEvent(QKeyEvent *e)
{
	CMapElement *element = mapManager->getEditElement();

	if (element->getElementType()==TEXT && element->getEditMode())
	{
		CMapText *text = (CMapText *)element;

		// Was the ESC key press if so the abort edit
		if (e->key() == Qt::Key_Escape)
		{
			text->restoreText();
			mapManager->unsetEditElement();
		}
		else if (e->key() == Qt::Key_Left)
		{
			text->cursorLeft();
		}
		else if (e->key() == Qt::Key_Right)
		{
			text->cursorRight();
		}
		else if (e->key() == Qt::Key_Up)
		{
			text->cursorUp();
		}
		else if (e->key() == Qt::Key_Down)
		{
			text->cursorDown();
		}
		else if (e->key() == Qt::Key_Home)
		{
			text->setCursor(QPoint(0,text->getCursor().y()));
		}
		else if (e->key() == Qt::Key_End)
		{
			text->cursorEnd();
		}
		else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
		{
			text->insertCR();
		}
		else if (e->key() == Qt::Key_Backspace)
		{
			text->backspace();
		}
		else if (e->key() == Qt::Key_Delete)
		{
			text->deleteChar();
		}
		else
		{
			text->insertString(QString(QChar(e->ascii())));
		}

		//FIXME_jp: Ensure that cursor is visiable
		mapManager->changedElement(text);

		mapManager->getActiveView()->ensureVisible(text->getCursorCords());
                e->accept();
	}
}

/** This function called when a tool is selected */
void CMapToolText::toolSelected(void)
{
	mapManager->setPropertiesAllViews(textCursor,false);
}

/** This function is called when a tool is unselected */
void CMapToolText::toolUnselected(void)
{
	mapManager->unsetEditElement();
}

