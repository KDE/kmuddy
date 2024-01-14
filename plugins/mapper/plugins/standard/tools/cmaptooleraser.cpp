/***************************************************************************
                               cmaperasertool.cpp
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

#include "cmaptooleraser.h"

#include <KLocalizedString>
#include <kiconloader.h>

#include <QBitmap>

#include "../../../cmapmanager.h"
#include "../../../cmaplevel.h"
#include "../../../cmapelement.h"

static unsigned char delete_cb_bits[] = {       // delete cursor bitmap
   0x0f, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00,
   0x89, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00,
   0x01, 0x04, 0x00, 0x00, 0x03, 0x08, 0x00, 0x00, 0x05, 0x10, 0x00, 0x00,
   0x09, 0x20, 0x00, 0x00, 0x11, 0x40, 0x00, 0x00, 0x22, 0x60, 0x00, 0x00,
   0x44, 0x50, 0x00, 0x00, 0x88, 0x48, 0x00, 0x00, 0x10, 0x45, 0x00, 0x00,
   0x20, 0x22, 0x00, 0x00, 0x40, 0x12, 0x00, 0x00, 0x80, 0x0a, 0x00, 0x00,
   0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static unsigned char delete_cm_bits[] = {       // delete cursor bitmap mask
   0x0f, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00,
   0xf9, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xfe, 0x03, 0x00, 0x00,
   0xff, 0x07, 0x00, 0x00, 0xff, 0x0f, 0x00, 0x00, 0xff, 0x1f, 0x00, 0x00,
   0xff, 0x3f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00,
   0xfc, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00,
   0xe0, 0x3f, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00,
   0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

CMapToolEraser::CMapToolEraser(KActionCollection *actionCollection,CMapManager *manager)
	: CMapToolBase(actionCollection,
                   i18n("Eraser"),
                   BarIcon("kmud_eraser.png"),
                   manager,"toolsEraser",nullptr)
{

	QBitmap delete_cb = QBitmap::fromData (QSize(32, 32), delete_cb_bits);
	QBitmap delete_cm = QBitmap::fromData (QSize(32, 32), delete_cm_bits);

	deleteCursor = new QCursor( delete_cb, delete_cm, 1,1);
}

CMapToolEraser::~CMapToolEraser()
{
	delete deleteCursor;
}

/** Called when the tool recives a mouse release event */
void CMapToolEraser::mouseReleaseEvent(QPoint mousePos, QMouseEvent * /*e*/, CMapLevel *currentLevel)
{
  if (!currentLevel) return;
  CMapElement *element = currentLevel->findElementAt(mousePos);
  if (element)
    mapManager->deleteElement(element);
}

/** This function called when a tool is selected */
void CMapToolEraser::toolSelected(void)
{
  mapManager->setPropertiesAllViews(deleteCursor,false);
}

/** This function is called when a tool is unselected */
void CMapToolEraser::toolUnselected(void)
{
}

#include "moc_cmaptooleraser.cpp"
