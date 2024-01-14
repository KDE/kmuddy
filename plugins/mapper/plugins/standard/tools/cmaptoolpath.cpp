/***************************************************************************
                               cmaptoolpath.cpp
                             -------------------
    begin                : Mon Aug 20 2001
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

#include "cmaptoolpath.h"

#include <KLocalizedString>
#include <kiconloader.h>

#include <QBitmap>

#include "../../../cmapmanager.h"
#include "../../../cmaproom.h"
#include "../../../cmaplevel.h"

static unsigned char path1_bits[] = {			// first path cursor bitmap
   0x10, 0x00, 0x30, 0x00, 0x70, 0x00, 0xf0, 0x00, 0xf0, 0x01, 0xf0, 0x03,
   0xf0, 0x07, 0xf0, 0x0f, 0xf0, 0x01, 0xf0, 0x03, 0xb0, 0x07, 0x12, 0x0f,
   0x03, 0x06, 0x02, 0x02, 0x02, 0x00, 0x07, 0x00};

static unsigned char path2_bits[] = {			// second path cursor bitmap
   0x10, 0x00, 0x30, 0x00, 0x70, 0x00, 0xf0, 0x00, 0xf0, 0x01, 0xf0, 0x03,
   0xf0, 0x07, 0xf0, 0x0f, 0xf0, 0x01, 0xf0, 0x03, 0xb0, 0x07, 0x12, 0x0f,
   0x05, 0x06, 0x04, 0x02, 0x02, 0x00, 0x07, 0x00};

CMapToolPath::CMapToolPath(KActionCollection *actionCollection,CMapManager *manager)
	: CMapToolBase(actionCollection,
                   i18n("Create Path"),
                   BarIcon(("kmud_path.png")),
                   manager,"toolsPath",nullptr)
{
		QBitmap pathStart_cb = QBitmap::fromData (QSize(16,16), path1_bits);
		pathStartCursor = new QCursor( pathStart_cb,pathStart_cb, 4,0);	

		QBitmap pathEnd_cb = QBitmap::fromData (QSize(16,16), path2_bits);
		pathEndCursor = new QCursor( pathEnd_cb, pathEnd_cb, 4,0);
}

CMapToolPath::~CMapToolPath()
{
	delete pathStartCursor;
	delete pathEndCursor;
}

/** Called when the tool recives a mouse release event */
void CMapToolPath::mouseReleaseEvent(QPoint mousePos, QMouseEvent * /*e*/, CMapLevel *currentLevel)
{
  if (!currentLevel) return;
  CMapRoom *destRoom = currentLevel->findRoomAt(mousePos);

  if (pathToolMode==1)
  {
    if ((destRoom && pathStartRoom) && (pathStartRoom!=destRoom))
      mapManager->createPath(pathStartRoom,destRoom);

    pathToolMode = 0;
    pathStartRoom = nullptr;
    currentCursor = pathStartCursor;
    mapManager->setPropertiesAllViews(currentCursor,false);
  }
  else
  {
    if (destRoom)
    {
      pathStartRoom = destRoom;
      pathToolMode = 1;
      currentCursor = pathEndCursor;
      mapManager->setPropertiesAllViews(currentCursor,false);
    }
  }
}

/** This function called when a tool is selected */
void CMapToolPath::toolSelected(void)
{
	currentCursor = pathStartCursor;

	mapManager->setPropertiesAllViews(currentCursor,false);

	pathStartRoom = nullptr;
	pathToolMode = 0;
}

