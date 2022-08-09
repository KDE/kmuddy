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
	~CMapToolSelect() override;

	/** Called when the tool recives a mouse press event */
	void mousePressEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;
	/** Called when the tool recives a mouse release event */
	void mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;
	/** Called when the tool recives a mouse move event */
	void mouseMoveEvent(QPoint mousePos, Qt::KeyboardModifiers, Qt::MouseButtons, CMapLevel *currentLevel) override;
	/** Called when the tool recives a mouse double click event */
	void mouseDoubleClickEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel) override;
	/** This is called when a key is pressed */
	void keyPressEvent(QKeyEvent *e) override;

	/** This function called when a tool is selected */
	void toolSelected(void) override;
	/** This function is called when a tool is unselected */
	void toolUnselected(void) override;

        void paint(QPainter *) override;

private slots:
	/** Called when a rectange drag opertion is performed */
	void slotStartDraging(void);

private:
	/** Used to move elements */
	void moveElement(QPoint offset, CMapLevel *currentLevel);
	/** Used to resize the selected elements */
	void resizeElements(QPoint mousePos,CMapLevel *currentLevel);
        QPoint alignToGrid(QPoint offset);

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
