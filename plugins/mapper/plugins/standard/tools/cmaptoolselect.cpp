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
#include "../../../cmappath.h"
#include "../cmapcmdmoveelements.h"

#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>

#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

CMapToolSelect::CMapToolSelect(KActionCollection *actionCollection,CMapManager *manager)
        : CMapToolBase(actionCollection,
                       i18n("Select"),
                       BarIcon("kmud_select.png"),
                       manager,"toolsSelect",0)
{
	kDebug() << "CMapToolSelect: constructor begins";

	bDragging = false;
	connect (&mouseDownTimer ,SIGNAL(timeout()),SLOT(slotStartDraging()));

	kDebug() << "CMapToolSelect: constructor ends";
}

CMapToolSelect::~CMapToolSelect()
{
}

void CMapToolSelect::paint(QPainter *p)
{
  if (!bDragging) return;

  int gridWidth = mapManager->getMapData()->gridSize.width();
  int gridHeight = mapManager->getMapData()->gridSize.height();

  CMapLevel *currentLevel = mapManager->getActiveView()->getCurrentlyViewedLevel();
  QList<CMapElement *> lst = currentLevel->getAllElements();
  if (moveDrag)
  {
    // This is a element move/drag operation
    p->setPen(Qt::black);

    QPoint offset;

    offset.setX((((int)(lastDrag.x() / gridWidth)) * gridWidth) - (((int)(mouseDrag.x()/gridWidth))*gridWidth));
    offset.setY((((int)(lastDrag.y() / gridHeight)) * gridHeight) - (((int)(mouseDrag.y()/gridHeight))*gridHeight));

    foreach (CMapElement *element, lst)
      if (element->getSelected())
        element->dragPaint(offset,p,currentLevel->getZone());
  }
  else if (resizeDrag > 0)
  {
    QPoint offset;

    offset.setX(((((int)(lastDrag.x() / gridWidth)) * gridWidth) - (((int)((mouseDrag.x()-6)/gridWidth))*gridWidth))-gridWidth);
    offset.setY(((((int)(lastDrag.y() / gridHeight)) * gridHeight) - (((int)((mouseDrag.y()-6)/gridHeight))*gridHeight))-gridHeight);			

    foreach (CMapElement *element, lst)
      if (element->getSelected())
        element->resizePaint(offset,p,currentLevel->getZone(),resizeDrag);
  }
  else if (m_editDrag > 0)
  {
    foreach (CMapElement *element, lst)
      if (element->getElementType()==PATH && element->getEditMode())
        ((CMapPath *)element)->editPaint(lastDrag,p,currentLevel->getZone(),m_editDrag);
  }
  else
  {
    // This is a element move/drag operation
    p->setPen(QColor(128, 128, 128, 64));
    p->setBrush(QColor(0, 128, 255, 32));
    p->drawRect(mouseDrag.x(),mouseDrag.y(),lastDrag.x()-mouseDrag.x(),lastDrag.y()-mouseDrag.y());
  }
}

/** Called when the tool recives a mouse press event */
void CMapToolSelect::mousePressEvent(QPoint mousePos, QMouseEvent *, CMapLevel *currentLevel)
{
  moveDrag = false;
  resizeDrag = 0;
  m_editDrag = 0;

  QList<CMapElement *> lst = currentLevel->getAllElements();

  // Check to see if the mouse was pressed in an element
  foreach (CMapElement *element, lst)
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
      if (element->mouseInElement(mousePos))
      {
        moveDrag = true;
      }
    }
  }

  if (resizeDrag > 0 || m_editDrag > 0)
  {
    // Repaint the map without the elements being resized
    foreach (CMapElement *element, lst)
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

    mapManager->redrawAllViews();
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

QPoint CMapToolSelect::alignToGrid(QPoint offset)
{
  int gridWidth = mapManager->getMapData()->gridSize.width();
  int gridHeight = mapManager->getMapData()->gridSize.height();

  offset.setX(offset.x() - offset.x() % gridWidth);
  offset.setY(offset.y() - offset.y() % gridHeight);

  return offset;
}

/** Called when the tool recives a mouse release event */
void CMapToolSelect::mouseReleaseEvent(QPoint mousePos, QMouseEvent *e, CMapLevel *currentLevel)
{
  QList<CMapElement *> lst = currentLevel->getAllElements();

  mouseDownTimer.stop();

  // Was a drag operation performed
  if (bDragging)
  {
    // Yes a drag is in action
    if (moveDrag)
    {
      kDebug() << "CMapToolSelect: move drag";
      // An element was draged to a new position so move it
      moveElement(alignToGrid(mousePos) - alignToGrid(mouseDrag), currentLevel);
    }
    else if (resizeDrag > 0)
    {
      // An element was resized so change it's size
      resizeElements(mousePos,currentLevel);
    }
    else if (m_editDrag > 0)
    {
      foreach (CMapElement *element, lst)
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
      if (!(QApplication::keyboardModifiers() & Qt::ControlModifier))
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
      foreach (CMapElement *element, lst)
      {
        if (element->elementIn(area,currentLevel->getZone()))
        {
          element->setSelected(true);
          found = true;
        }
      }
      if (found)
      {
        mapManager->levelChanged(currentLevel);
      }
    }

    bDragging = false;
    return;
  }

  // If we are shift-clicking, toggle whatever is clicked on. Otherwise unselect everything, then select what got clicked on.
  bool shift = e->modifiers() & Qt::ShiftModifier;

  if (!shift)
    mapManager->unselectElements(currentLevel);

  foreach (CMapElement *element, lst)
  {
    if (!element->mouseInElement(mousePos)) continue;
    bool sel = shift ? (!element->getSelected()) : true;
    element->setSelected(sel);
    break;
  }

  mapManager->getActiveView()->changed();
}

void CMapToolSelect::mouseDoubleClickEvent(QPoint mousePos, QMouseEvent *, CMapLevel *currentLevel)
{
  // If we double-click on something that is not a room, do nothing
  QList<CMapElement *> elements = currentLevel->elementsUnderMouse(mousePos);
  CMapRoom *room = 0;
  foreach (CMapElement *el, elements) {
    room = dynamic_cast<CMapRoom *>(el);
    if (room) break;
  }
  if (!room) return;

  // If we do not have a current room, set this room as one.
  if (!mapManager->getCurrentRoom()) {
    mapManager->setCurrentRoom(room);
    return;
  }

  // If we do, speedwalk from there to this room.
  mapManager->walkPlayerTo(room);
}


/** Called when the tool recives a mouse move event */
void CMapToolSelect::mouseMoveEvent(QPoint mousePos,Qt::ButtonState,CMapLevel *)
{
  // If a drag operation is not in progress then return
  if (!bDragging) return;

  // Make sure this is not the same position as the last one
  if (lastDrag!=mousePos)
  {
    lastDrag=mousePos;
    mapManager->getActiveView()->requestPaint();
  }
}

/** This is called when a key is pressed */
void CMapToolSelect::keyPressEvent(QKeyEvent *e)
{
  int x = 0, y = 0;
  if (e->key() == Qt::Key_Left) x = -1;
  else if (e->key() == Qt::Key_Right) x = 1;
  else if (e->key() == Qt::Key_Up) y = -1;
  else if (e->key() == Qt::Key_Down) y = 1;
  else return;

  int gridWidth = mapManager->getMapData()->gridSize.width();
  int gridHeight = mapManager->getMapData()->gridSize.height();

  QPoint offset(gridWidth * x, gridHeight * y);
  moveElement(offset, mapManager->getActiveView()->getCurrentlyViewedLevel());
}

/** This function called when a tool is selected */
void CMapToolSelect::toolSelected(void)
{
	CMapView *view = mapManager->getActiveView();
	if (view)
	{
		view->setMouseTracking(false);
		view->setCursor(Qt::ArrowCursor);
	}
}

/** This function is called when a tool is unselected */
void CMapToolSelect::toolUnselected(void)
{
	CMapView *view = mapManager->getActiveView();
	if (view)
		mapManager->unselectElements(view->getCurrentlyViewedLevel());
}

/** Used to move elements */
void CMapToolSelect::moveElement(QPoint offset, CMapLevel *currentLevel)
{
  moveDrag = false;

  offset = alignToGrid(offset);

  CMapCmdMoveElements *cmd = new CMapCmdMoveElements(mapManager, offset);

  bool found = false;
  QList<CMapElement *> lst = currentLevel->getAllElements();
  foreach (CMapElement *element, lst)
    if (element->getSelected()) {
      cmd->addElement(element);
      found = true;
    }

  if (found)
    mapManager->addCommand(cmd);
  else
    delete cmd;
}

/** Used to resize the selected elements */
void CMapToolSelect::resizeElements(QPoint mousePos,CMapLevel *currentLevel)
{
  int gridWidth = mapManager->getMapData()->gridSize.width();
  int gridHeight = mapManager->getMapData()->gridSize.height();

  QPoint offset;
  offset.setX(((((int)(mousePos.x() / gridWidth)) * gridWidth) - (((int)((mouseDrag.x()-6)/gridWidth))*gridWidth))-gridWidth);
  offset.setY(((((int)(mousePos.y() / gridHeight)) * gridHeight) - (((int)((mouseDrag.y()-6)/gridHeight))*gridHeight))-gridHeight);

  QList<CMapElement *> lst = currentLevel->getAllElements();
  foreach (CMapElement *element, lst)
    if (element->getSelected())
    {
      element->resize(offset,resizeDrag);
      element->setDoPaint(true);
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

  CMapView *view =  mapManager->getActiveView();    
  CMapLevel *currentLevel = view->getCurrentlyViewedLevel();

  if (!currentLevel) return;

  QList<CMapElement *> lst = currentLevel->elementsUnderMouse(mouseDrag);
  foreach (CMapElement *element, lst)
    element->setSelected(true);
}

