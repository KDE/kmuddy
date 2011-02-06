/***************************************************************************
                               cmaptoolselect.cpp
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

#include "cmaptoolselect.h"

#include "../../../cmapmanager.h"
#include "../../../cmapelement.h"
#include "../../../cmapview.h"
#include "../../../cmaplevel.h"
#include "../../../cmapviewbase.h"
#include "../../../cmappath.h"
#include "../cmapcmdmoveelements.h"

#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>

#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

CMapToolSelect::CMapToolSelect(KActionCollection *actionCollection,CMapManager *manager,QObject *parent)
        : CMapToolBase(actionCollection,
                       i18n("Select"),
                       BarIcon("kmud_select.png"),
                       manager,"toolsSelect",0,parent)
{
	kDebug() << "CMapToolSelect: constructor begins";

	bDragging = false;
	connect (&mouseDownTimer ,SIGNAL(timeout()),SLOT(slotStartDraging()));

	kDebug() << "CMapToolSelect: constructor ends";
}

CMapToolSelect::~CMapToolSelect()
{
}

/** Called when the tool recives a mouse press event */
void CMapToolSelect::mousePressEvent(QPoint mousePos,CMapLevel *currentLevel)
{
	moveDrag = false;
	resizeDrag = 0;
	m_editDrag = 0;

	// Check to see if the mouse was pressed in an element
	for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
	{
		// Is a edit drag in operation
		if (element->getElementType()==PATH && element->getEditMode())
		{
			CMapPath *path = (CMapPath *)element;
			m_editDrag = path->mouseInEditBox(mousePos,currentLevel->getZone());

			if (m_editDrag > 0)
			{
				break;
			}
		}

		// Is a resize drag in operation
		resizeDrag = element->mouseInResize(mousePos,currentLevel->getZone());
		if (resizeDrag > 0 && element->getSelected())
		{
			break;
		}
		else
		{
			resizeDrag = 0;
			if (element->mouseInElement(mousePos,currentLevel->getZone()))
			{
				moveDrag = true;
			}
		}
	}

	if (resizeDrag > 0 || m_editDrag > 0)
	{
		// Repaint the map without the elements being resized
		for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
		{
			if (element->getSelected())
			{
				element->setDoPaint(false);
			}

			if (element->getEditMode() && element->getElementType()==PATH)
			{
				((CMapPath *)element)->setDontPaintBend(m_editDrag);
			}
		}

		mapManager->getActiveView()->redraw();
		//mapManager->getActiveView()->generateContents();

		// Now Draw the map elements that need resizeing
		QPixmap background(*mapManager->getActiveView()->getViewportBackground());
	}

	if (moveDrag || resizeDrag > 0 || m_editDrag > 0)
	{
		// The mouse was pressed in an element so this is a element move/drag operation
		mouseDrag =  mousePos;
		lastDrag = QPoint (-100,-100);
	}
	else
	{
		// The mouse was not pressed in a element so this is a rectangle drag selection operation
		mouseDrag = lastDrag = mousePos;
	}

	mapManager->levelChanged(currentLevel);

	kDebug() << "CMapToolSelect: timer start";
	mouseDownTimer.start(150,false);
}

/** Called when the tool recives a mouse release event */
void CMapToolSelect::mouseReleaseEvent(QPoint mousePos,CMapLevel *currentLevel)
{
	mouseDownTimer.stop();

	// Was a drag operation performed
	if (bDragging)
	{
		// Yes a drag is in action
		if (moveDrag)
		{
			kDebug() << "CMapToolSelect: move drag";
			// An element was draged to a new position so move it
			moveElement(mousePos,currentLevel);
		}
		else if (resizeDrag > 0)
		{
			// An element was resized so change it's size
			resizeElements(mousePos,currentLevel);
		}
		else if (m_editDrag > 0)
		{
			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->getElementType()==PATH && element->getEditMode())
				{
					mapManager->openCommandGroup(i18n("Move Bend"));
					CMapPath *path = (CMapPath *) element;

					path->setDontPaintBend(0);
					path->moveBendWithUndo(m_editDrag,mousePos);
					if (path->getOpsitePath())
					{
						path->getOpsitePath()->moveBendWithUndo(m_editDrag,mousePos);
					}
					mapManager->closeCommandGroup();
					break;
				}
			}
		}
		else
		{
			kDebug() << "CMapToolSelect: select drag";
			bool found = false;

			// A rectangle selection operation was performed
			if (!mapManager->getActiveView()->getCtrlPressed())
			{				
				//FIXME_jp: unselectElements should return a bool like in kmud1
				mapManager->unselectElements(currentLevel);
				found = true;
			}

			int x1 = mouseDrag.x() < lastDrag.x() ? mouseDrag.x() : lastDrag.x();
			int y1 = mouseDrag.y() < lastDrag.y() ? mouseDrag.y() : lastDrag.y();
			int x2 = mouseDrag.x() >= lastDrag.x() ? mouseDrag.x() : lastDrag.x();
			int y2 = mouseDrag.y() >= lastDrag.y() ? mouseDrag.y() : lastDrag.y();
			QRect area = QRect(QPoint(x1,y1),QPoint(x2,y2));

			// Select the elements in the rectangle              	
			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->elementIn(area,currentLevel->getZone()))
				{
					element->setSelected(true);
					found = true;
				}
			}
// TODO: this must be done using a repaint
#if 0
			// Erase the rectangle
			p->setPen(Qt::black);
			// p->setRasterOp(Qt::NotROP);  // TODO: what with this?
			p->drawRect(mouseDrag.x(),mouseDrag.y(),mousePos.x()-mouseDrag.x(),mousePos.y()-mouseDrag.y());
#endif

			if (found)
			{
				mapManager->levelChanged(currentLevel);
			}
		}

		bDragging = false;
	}
	else
	{
		bool found = false;
		
		for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
		{
			if (element->mouseInElement(mousePos,currentLevel->getZone()))
			{
				found = true;
				if (!mapManager->getActiveView()->getCtrlPressed() )
				{
					element->setSelected(!element->getSelected());
				}
				else
				{
					element->setSelected(true);
				}
			}

		}

		if (!found)
		{
			mapManager->unselectElements(currentLevel);

		}

		mapManager->getActiveView()->redraw();

	}
}

/** Called when the tool recives a mouse move event */
void CMapToolSelect::mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *currentLevel)
{
	// If a drag operation is not in progress then return
	if (!bDragging) return;

	// Make sure this is not the same position as the last one
	if (lastDrag!=mousePos)
	{
// TODO: all this must be done in a repaint event
#if 0
		if (moveDrag)
		{
			// This is a element move/drag operation
			//p->setRasterOp(NotROP);  // TODO: what with this?
			p->setPen(Qt::black);

			QPoint offset;
			int gridWidth = mapManager->getMapData()->gridSize.width();
			int gridHeight = mapManager->getMapData()->gridSize.height();

			offset.setX((((int)(lastDrag.x() / gridWidth)) * gridWidth) - (((int)(mouseDrag.x()/gridWidth))*gridWidth));
			offset.setY((((int)(lastDrag.y() / gridHeight)) * gridHeight) - (((int)(mouseDrag.y()/gridHeight))*gridHeight));

			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->getSelected())
				{
					element->dragPaint(offset,p,currentLevel->getZone());
				}
			}

			offset.setX((((int)(mousePos.x() / gridWidth)) * gridWidth) - (((int)(mouseDrag.x()/gridWidth))*gridWidth));
			offset.setY((((int)(mousePos.y() / gridHeight)) * gridHeight) - (((int)(mouseDrag.y()/gridHeight))*gridHeight));

			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->getSelected())
				{
					element->dragPaint(offset,p,currentLevel->getZone());
				}
			}
		}
		else if (resizeDrag > 0)
		{
			QPixmap background(*mapManager->getActiveView()->getViewportBackground());

			QPoint offset;
			int gridWidth = mapManager->getMapData()->gridSize.width();
			int gridHeight = mapManager->getMapData()->gridSize.height();

			QPainter p2;

			p2.begin(&background);


			QRect drawArea = mapManager->getActiveView()->getViewArea();
			p2.translate(-drawArea.x(), -drawArea.y());

			offset.setX(((((int)(mousePos.x() / gridWidth)) * gridWidth) - (((int)((mouseDrag.x()-6)/gridWidth))*gridWidth))-gridWidth);
			offset.setY(((((int)(mousePos.y() / gridHeight)) * gridHeight) - (((int)((mouseDrag.y()-6)/gridHeight))*gridHeight))-gridHeight);			

			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->getSelected())
				{
					element->resizePaint(offset,&p2,currentLevel->getZone(),resizeDrag);
				}
			}

			p2.end();

			bitBlt(p->device(), 0, 0, &background);
		}
		else if (m_editDrag > 0)
		{
			QPixmap background(*mapManager->getActiveView()->getViewportBackground());

			QPainter p2;

			p2.begin(&background);

			QRect drawArea = mapManager->getActiveView()->getViewArea();
			p2.translate(-drawArea.x(), -drawArea.y());

			for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
			{
				if (element->getElementType()==PATH && element->getEditMode())
				{
					((CMapPath *)element)->editPaint(mousePos,&p2,currentLevel->getZone(),m_editDrag);
				}
			}

			p2.end();

			bitBlt(p->device(), 0, 0, &background);
		}
		else
		{
			//p->setRasterOp(NotROP);  // TODO: what with this?
			// This is a element move/drag operation
			p->setPen(QPen(Qt::DotLine));

			// Erase Old Rectangle
			p->drawRect(mouseDrag.x(),mouseDrag.y(),lastDrag.x()-mouseDrag.x(),lastDrag.y()-mouseDrag.y());

			// Draw New Rectangle
			p->drawRect(mouseDrag.x(),mouseDrag.y(),mousePos.x()-mouseDrag.x(),mousePos.y()-mouseDrag.y());
		}
#endif
		lastDrag=mousePos;
	}
}

/** This function called when a tool is selected */
void CMapToolSelect::toolSelected(void)
{
	CMapViewBase *view = mapManager->getActiveView();
	if (view)
	{
		view->setMouseTracking(false);
		view->setCursor(Qt::ArrowCursor);
	}
}

/** This function is called when a tool is unselected */
void CMapToolSelect::toolUnselected(void)
{
	CMapViewBase *view = mapManager->getActiveView();
	if (view)
	{
		mapManager->unselectElements(view->getCurrentlyViewedLevel());
		view->redraw();
	}
}

/** This method is called when the active view changes */
void CMapToolSelect::viewChangedEvent(CMapViewBase *view)
{
	if (view)
	{
		view->setMouseTracking(false);
		view->setCursor(Qt::ArrowCursor);
	}
}

/** Used to move elements */
void CMapToolSelect::moveElement(QPoint mousePos,CMapLevel *currentLevel)
{
	moveDrag = false;

	int gridWidth = mapManager->getMapData()->gridSize.width();
	int gridHeight = mapManager->getMapData()->gridSize.height();

	QPoint offset;
	offset.setX((((int)(mousePos.x() / gridWidth)) * gridWidth) - (((int)((mouseDrag.x()-6)/gridWidth))*gridWidth));
	offset.setY((((int)(mousePos.y() / gridHeight)) * gridHeight) - (((int)((mouseDrag.y()-6)/gridHeight))*gridHeight));

	CMapCmdMoveElements *cmd = new CMapCmdMoveElements(mapManager,offset);

	for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
	{
		if (element->getSelected())
		{

			cmd->addElement(element);
		}
	}

	mapManager->addCommand(cmd);
}

/** Used to resize the selected elements */
void CMapToolSelect::resizeElements(QPoint mousePos,CMapLevel *currentLevel)
{
	int gridWidth = mapManager->getMapData()->gridSize.width();
	int gridHeight = mapManager->getMapData()->gridSize.height();

	QPoint offset;
	offset.setX(((((int)(mousePos.x() / gridWidth)) * gridWidth) - (((int)((mouseDrag.x()-6)/gridWidth))*gridWidth))-gridWidth);
	offset.setY(((((int)(mousePos.y() / gridHeight)) * gridHeight) - (((int)((mouseDrag.y()-6)/gridHeight))*gridHeight))-gridHeight);

	for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
	{                                                                                         			mapManager->getActiveView()->redraw();
		if (element->getSelected())
		{
			element->resize(offset,resizeDrag);
			element->setDoPaint(true);
		}
	}

	resizeDrag = 0;
	mapManager->levelChanged(currentLevel);
}

///////////////////////////////////// SLOTS /////////////////////////////////////////////////

/** Called when a rectange drag opertion is performed */
void CMapToolSelect::slotStartDraging(void)
{
	kDebug() << "CMapToolSelect: timer stop";
	bDragging = true;
	mouseDownTimer.stop();
	
    CMapViewBase *view =  mapManager->getActiveView();    
	CMapLevel *currentLevel = view->getCurrentlyViewedLevel();

	if (currentLevel)
	{
		for (CMapElement *element=currentLevel->getFirstElement(); element!=0; element=currentLevel->getNextElement())
		{
			if (element->mouseInElement(mouseDrag,currentLevel->getZone()))
			{
				element->setSelected(true);
			}
		}
	}
}
