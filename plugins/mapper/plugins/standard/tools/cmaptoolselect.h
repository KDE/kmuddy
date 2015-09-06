/***************************************************************************
                               cmaptoolselect.h
                             -------------------
    begin                : Tue May 1 2001
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

#ifndef CMAPTOOLSELECT_H
#define CMAPTOOLSELECT_H

#include <qpainter.h>
#include <qtimer.h>
#include <qpoint.h>

#include "../../../cmaptoolbase.h"

class CMapManager;
class CMapElement;
class CMapLevel;

/**
 * @author Kmud Developer Team
 */

class CMapToolSelect : public CMapToolBase
{
        Q_OBJECT
public:
	CMapToolSelect(KActionCollection *actionCollection,CMapManager *manager);
	virtual ~CMapToolSelect();

	/** Called when the tool recives a mouse press event */
	virtual void mousePressEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel);
	/** Called when the tool recives a mouse release event */
	virtual void mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel);
	/** Called when the tool recives a mouse move event */
	virtual void mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *currentLevel);
	/** This is called when a key is pressed */
	virtual void keyPressEvent(QKeyEvent *e);

	/** This function called when a tool is selected */
	virtual void toolSelected(void);
	/** This function is called when a tool is unselected */
	virtual void toolUnselected(void);

        virtual void paint(QPainter *);

private slots:
	/** Called when a rectange drag opertion is performed */
	void slotStartDraging(void);

private:
	/** Used to move elements */
	void moveElement(QPoint offset, CMapLevel *currentLevel);
	/** Used to resize the selected elements */
	void resizeElements(QPoint mousePos,CMapLevel *currentLevel);


private:
	/** Used to mesure how long a mouse putton is held down */
	QTimer mouseDownTimer;
	/** The element that is being edited */
	CMapElement *elementEditing;
	/** Is a element being resized */
	int resizeDrag;
	/** Is a element being edited */
	int m_editDrag;
	/** Is a element being moved by dragging */
	bool moveDrag;
	/** Used to tell if the mouse is being draged */
	bool bDragging;
	/** Uses to store the current pos for dragging positions */
	QPoint mouseDrag;
	/** Used to store the last mouse pos for dragging positions */
	QPoint lastDrag;

};

#endif
