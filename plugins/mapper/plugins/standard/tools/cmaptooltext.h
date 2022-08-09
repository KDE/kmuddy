/***************************************************************************
                               cmaptexttool.h
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

#ifndef CMAPTOOLTEXT_H
#define CMAPTOOLTEXT_H

#include <qobject.h>
#include <qcursor.h>
#include <qevent.h>

#include "../../../cmaptoolbase.h"

/**This tool is used to delete map elements
  *@author Kmud Developer Team
  */

class CMapToolText : public CMapToolBase
{
   Q_OBJECT
public:
	CMapToolText(KActionCollection *actionCollection,CMapManager *manager);
	~CMapToolText() override;

	/** Called when the tool recives a mouse release event */
	void mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;
	/** This is called when a key is pressed */
	void keyPressEvent(QKeyEvent *e) override;

	/** This function called when a tool is selected */
	void toolSelected(void) override;
	/** This function is called when a tool is unselected */
	void toolUnselected(void) override;

private:
	/** The mouse cursor */
	QCursor *textCursor;

};

#endif
