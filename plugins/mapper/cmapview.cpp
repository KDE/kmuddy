/***************************************************************************
                               cmapview.cpp
                             -------------------
    begin                : Mon Mar 19 2001
    copyright            : (C) 2001 by Kmud Developer Team
                           (C) 2007 Tomas Mecir <kmuddy@kmuddy.net>
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

#include "cmapview.h"

#include <QAction>
#include <QPushButton>
#include <QScrollArea>
#include <QActionGroup>

#include "cmapmanager.h"
#include "cmapzone.h"
#include "cmapzonemanager.h"
#include "cmaplevel.h"
#include "cmappath.h"
#include "cmapview.h"
#include "cmapelement.h"
#include "cmapwidget.h"
#include "cmapviewstatusbar.h"
#include "cmaptoolbase.h"
#include "cmapclipboard.h"
#include "cmapcmdelementproperties.h"

#include <kselectaction.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kundostack.h>
#include <kinputdialog.h>

CMapView::CMapView(CMapManager *manager,QWidget *parent) : KXmlGuiWindow(parent)
{
  kDebug() << "CMapView::CMapView create view";

  setCaption (i18n ("Mapper"));
  setAttribute (Qt::WA_DeleteOnClose, false);  // do not delete on close

  mapManager = manager;
  currentLevel = 0;
  setFocusPolicy(Qt::StrongFocus);
  setWindowFlags (Qt::Widget);

  m_clipboard = new CMapClipboard(mapManager, this, actionCollection());
  initMenus();

  // set up the menus
  setHelpMenuEnabled (false);

  scroller = new QScrollArea(this);
  scroller->setWidgetResizable(true);
  scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setCentralWidget(scroller);

  mapWidget = new CMapWidget(this, mapManager, scroller);
  scroller->setWidget(mapWidget);
  mapWidget->show();

  statusbar = new CMapViewStatusbar(manager, this);
  setStatusBar(statusbar);

  cmdFollowMode = new QPushButton(i18n("Follow Moves"),statusbar);
  cmdFollowMode->setIcon(BarIcon("kmud_follow.png"));
  cmdFollowMode->setCheckable(true);
  cmdFollowMode->setFocusProxy(this);
  statusbar->addFollowButton(cmdFollowMode);

  cmdCreateMode = new QPushButton(i18n("Auto Create"),statusbar);
  cmdCreateMode->setIcon(BarIcon("kmud_create.png"));
  cmdCreateMode->setCheckable(true);
  cmdCreateMode->setFocusProxy(this);
  statusbar->addFollowButton(cmdCreateMode);
//  getMapData()->createModeActive = m_toolsCreate->isChecked();

  //FIXME_jp : get settings for status bar instead of defaults

  //FIXME_jp : Add proper resize instead of test size
  changed();
}

CMapView::~CMapView()
{
  kDebug() << "CMapView::~CMapView()";
}

void CMapView::initGUI()
{
  createGUI(KStandardDirs::locate("appdata", "kmuddymapperpart.rc"));
  mapWidget->initContexMenus();
  enableViewControls(false);
}

void CMapView::initMenus()
{
  kDebug() << "begin initMenus";
  kDebug() << "Main collection is "<<actionCollection();

  // Edit menu
  mapManager->getCommandHistory()->createUndoAction(actionCollection(), "editUndo");
  mapManager->getCommandHistory()->createRedoAction(actionCollection(), "editRedo");

  // Tools menu
  m_toolsGrid = new KToggleAction (this);
  m_toolsGrid->setText ( i18n("&Grid"));
  m_toolsGrid->setIcon (BarIcon("kmud_grid.png"));
  connect (m_toolsGrid, SIGNAL (triggered()), this, SLOT(slotToolsGrid()));
  actionCollection()->addAction ("toolsGrid", m_toolsGrid);
  m_toolsUpLevel = new QAction (this);
  m_toolsUpLevel->setText ( i18n("Display Upper Level"));
  m_toolsUpLevel->setIcon (BarIcon("arrow-up"));
  connect (m_toolsUpLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelUp()));
  actionCollection()->addAction ("toolsLevelUp", m_toolsUpLevel);
  m_toolsDownLevel = new QAction (this);
  m_toolsDownLevel->setText ( i18n("Display Lower Level"));
  m_toolsDownLevel->setIcon (BarIcon("arrow-down"));
  connect (m_toolsDownLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelDown()));
  actionCollection()->addAction ("toolsLevelDown", m_toolsDownLevel);
  m_toolsDeleteLevel = new QAction (this);
  m_toolsDeleteLevel->setText ( i18n("Delete Current Level"));
  m_toolsDeleteLevel->setIcon (BarIcon("edit-delete"));
  connect (m_toolsDeleteLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelDelete()));
  actionCollection()->addAction ("toolsLevelDelete", m_toolsDeleteLevel);
  m_toolsCreateZone = new QAction (this);
  m_toolsCreateZone->setText ( i18n("Create New Zone"));
  m_toolsCreateZone->setIcon (BarIcon("task-new"));
  connect (m_toolsCreateZone, SIGNAL (triggered()), this, SLOT(slotToolsZoneCreate()));
  actionCollection()->addAction ("toolsZoneCreate", m_toolsCreateZone);
  m_toolsDeleteZone = new QAction (this);
  m_toolsDeleteZone->setText ( i18n("Delete Current Zone"));
  m_toolsDeleteZone->setIcon (BarIcon("edit-delete"));
  connect (m_toolsDeleteZone, SIGNAL (triggered()), this, SLOT(slotToolsZoneDelete()));
  actionCollection()->addAction ("toolsZoneDelete", m_toolsDeleteZone);

  // View menu
  m_viewUpperLevel = new KToggleAction (this);
  m_viewUpperLevel->setText ( i18n("Map Upper Level"));
  connect  (m_viewUpperLevel, SIGNAL (triggered()), this, SLOT(slotViewUpperLevel()));
  actionCollection()->addAction ("viewUpperLevel", m_viewUpperLevel);
  m_viewLowerLevel = new KToggleAction (this);
  m_viewLowerLevel->setText ( i18n("Map Lower Level"));
  connect  (m_viewLowerLevel, SIGNAL (triggered()), this, SLOT(slotViewLowerLevel()));
  actionCollection()->addAction ("viewLowerLevel", m_viewLowerLevel);

  // Room Popup Actions
  QAction *action;
  action = new QAction (this);
  action->setText (i18n("Set &Current Position"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSetCurrentPos()));
  actionCollection()->addAction ("roomCurrentPos", action);
  action = new QAction (this);
  action->setText (i18n("Set Room to &Login Point"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSetLogin()));
  actionCollection()->addAction ("roomLoginPoint", action);
  action = new QAction (this);
  action->setText (i18n("&Speed walk to room"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSpeedwalkTo()));
  actionCollection()->addAction ("roomWalkTo", action);
  action = new QAction (this);
  action->setText (i18n("&Delete room"));
  action->setIcon (SmallIcon("edit-delete"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomDelete()));
  actionCollection()->addAction ("roomDelete", action);
  action = new QAction (this);
  action->setText (i18n("&Properties"));
  action->setIcon (SmallIcon("document-properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomProperties()));
  actionCollection()->addAction ("roomProperties", action);

  // Text Popup Actions
  action = new QAction (this);
  action->setText (i18n("&Delete Text"));
  action->setIcon (SmallIcon("edit-delete"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotTextDelete()));
  actionCollection()->addAction ("textDelete", action);
  action = new QAction (this);
  action->setText (i18n("&Properties"));
  action->setIcon (SmallIcon("document-properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotTextProperties()));
  actionCollection()->addAction ("textProperties", action);

  // Path Popup Actions
  action = new KToggleAction (this);
  action->setText (i18n("&One way"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathOneWay()));
  actionCollection()->addAction ("pathOneWay", action);
  action = new KToggleAction (this);
  action->setText (i18n("&Two way"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathTwoWay()));
  actionCollection()->addAction ("pathTwoWay", action);
  action = new QAction (this);
  action->setText (i18n("&Add Bend"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathAddBend()));
  actionCollection()->addAction ("pathAddBend", action);
  action = new QAction (this);
  action->setText (i18n("&Remove Segment"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathDelBend()));
  actionCollection()->addAction ("pathDelBend", action);
  action = new QAction (this);
  action->setText (i18n("&Edit Bends"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathEditBends()));
  actionCollection()->addAction ("pathEditBends", action);
  action = new QAction (this);
  action->setText (i18n("&Delete Path"));
  action->setIcon (SmallIcon("edit-delete"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathDelete()));
  actionCollection()->addAction ("pathDelete", action);
  action = new QAction (this);
  action->setText (i18n("&Properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathProperties()));
  actionCollection()->addAction ("pathPorperties", action);

  QStringList labelPos;
  labelPos.append(i18n("Hide"));
  labelPos.append(mapManager->directionToText(NORTH,""));
  labelPos.append(mapManager->directionToText(NORTHEAST,""));
  labelPos.append(mapManager->directionToText(EAST,""));
  labelPos.append(mapManager->directionToText(SOUTHEAST,""));
  labelPos.append(mapManager->directionToText(SOUTH,""));
  labelPos.append(mapManager->directionToText(SOUTHWEST,""));
  labelPos.append(mapManager->directionToText(WEST,""));
  labelPos.append(mapManager->directionToText(NORTHWEST,""));
  labelPos.append(i18n("Custom"));
  
  labelMenu = new KSelectAction (this);
  labelMenu->setText (i18n("&Label"));
  connect (labelMenu, SIGNAL (triggered()), this, SLOT(slotChangeLabelPos()));
  actionCollection()->addAction ("labelMenu", labelMenu);
  labelMenu->setItems(labelPos);

  // tool action group
  m_toolGroup = new QActionGroup (this);
}


void CMapView::readOptions()
{
  CMapData *data = mapManager->getMapData();
  m_toolsGrid->setChecked(data->gridVisable);
  m_viewLowerLevel->setChecked(data->showLowerLevel);
  m_viewUpperLevel->setChecked(data->showUpperLevel);
}

/** Used to get the currently viewed zone */
CMapZone *CMapView::getCurrentlyViewedZone()
{
  return currentLevel ? currentLevel->getZone() : 0;
}

/** Used to get the currently viewed level */
CMapLevel *CMapView::getCurrentlyViewedLevel()
{
  return currentLevel;
}

/** Used to set the current level. This is for internal use */
void CMapView::setLevel(CMapLevel *level)
{
  currentLevel = level;
}

void CMapView::playerPositionChanged(CMapRoom *room)
{
  if (!room) return;
  if (room->getLevel() != currentLevel)
    showPosition(room->getLevel(), false);
}

void CMapView::setSelectedElement(CMapElement *element)
{
  m_selectedElement = element;  
}

void CMapView::setSelectedPos(QPoint pos)
{
  m_selectedPos = pos;
}

CMapElement *CMapView::getSelectedElement()
{
  return m_selectedElement;
}


/** Used to let the map manager know if it should register the focus of this widget */
bool CMapView::acceptFocus()
{
  return true;
}

/** This method is called when an element is added */
void CMapView::addedElement(CMapElement *element)
{
  if (isElementVisible(element))
  {
    checkSize(element->getHighPos());
    mapWidget->update();
  }
}

/** This method is called when an element is deleted */
void CMapView::deletedElement(CMapLevel *deletedFromLevel)
{
  CMapLevel *upperLevel = getCurrentlyViewedLevel()->getNextLevel();
  CMapLevel *lowerLevel = getCurrentlyViewedLevel()->getPrevLevel();

  if (deletedFromLevel == getCurrentlyViewedLevel())
    mapWidget->update();

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
    if (deletedFromLevel == upperLevel)
      mapWidget->update();

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
    if (deletedFromLevel == lowerLevel)
      mapWidget->update();
}

/** This method is called when an element is changed */
void CMapView::changedElement(CMapElement *element)
{
  if (isElementVisible(element))
  {
    checkSize(element->getHighPos());
    mapWidget->update();
  }

  if (element == mapManager->getCurrentRoom())
    statusbar->setRoom(mapManager->getCurrentRoom()->getLabel());
}

/** This method is called when a map level is changed */
void CMapView::changedLevel(CMapLevel *level)
{
  if (!isLevelVisible(level)) return;
  changed();
}

/** Used to find out if a level is visible in the view */
bool CMapView::isLevelVisible(CMapLevel *level)
{
  CMapLevel *vlevel = getCurrentlyViewedLevel();
  if (!vlevel) return false;
  if (level == vlevel)
    return true;
  if (level == vlevel->getPrevLevel())
    return true;
  if (level == vlevel->getNextLevel())
    return true;

  return false;
}

/** Used to find out if a element is visiable in the view */
bool CMapView::isElementVisible(CMapElement *element)
{
  return isLevelVisible(element->getLevel());
}

/**
 * Used to enable/disable the view actions
 * @param If true then enable the actions otherwise disable the actions
 */
void CMapView::enableViewControls(bool enabled)
{
  if (!mapManager->getMapData()) return;  // if we don't have mapData, we're going down
  enableNonViewActions(enabled);
  m_clipboard->enableActions(enabled);
  m_toolsUpLevel->setEnabled(enabled);
  m_toolsDownLevel->setEnabled(enabled);
  m_toolsDeleteLevel->setEnabled(enabled);
  m_toolsCreateZone->setEnabled(enabled);
  m_toolsDeleteZone->setEnabled(enabled);
}

/**
 * This method is used to disable/enable mapper actions that are not done by enableViewControls()
 * @param If true then enable the actions otherwise disable the actions
 */
void CMapView::enableNonViewActions(bool enabled)
{
  m_toolsGrid->setEnabled(enabled);
}


void CMapView::requestPaint()
{
  mapWidget->update();
}

void CMapView::changed()
{
  maxSize = QSize(0,0);

  CMapLevel *level = getCurrentlyViewedLevel();
  if (!level) {
    mapWidget->update();
    return;
  }

  CMapLevel *upperLevel = level->getNextLevel();
  CMapLevel *lowerLevel = level->getPrevLevel();

  QPoint size(0,0);

  // Calc the size the widget should be
  QList<CMapElement *> lst = level->getAllElements();
  foreach (CMapElement *element, lst)
  {
    if (element->getHighX()>size.x()) size.setX(element->getHighX());
    if (element->getHighY()>size.y()) size.setY(element->getHighY());
  }

  if (upperLevel && mapManager->getMapData()->showUpperLevel)
  {
    lst = upperLevel->getAllElements();
    foreach (CMapElement *element, lst)
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  if (lowerLevel && mapManager->getMapData()->showLowerLevel)
  {
    lst = lowerLevel->getAllElements();
    foreach (CMapElement *element, lst)
    {
      if (element->getHighX()>size.x()) size.setX(element->getHighX());
      if (element->getHighY()>size.y()) size.setY(element->getHighY());
    }
  }

  checkSize(size);
  mapWidget->update();
}


/** Tell this map widget to display a different level. view wiil
  * center on the first room */
void CMapView::showPosition(CMapLevel *level,bool centerView)
{
  QPoint pos(0,0);
  if (!level->getRoomList()->isEmpty())
  {
    CMapRoom *room = level->getRoomList()->first();
    pos.setX(room->getX());
    pos.setY(room->getY());
  }
  showPosition(pos,level,centerView);
}

void CMapView::showPosition(CMapRoom *room,bool centerView)
{
  QPoint pos(0,0);  
  if (room)  
  {
    pos.setX(room->getX());
    pos.setY(room->getY());
    showPosition(pos,room->getLevel(),centerView);
  }  
}

/** Tell this map widget to display a different zone */
void CMapView::showPosition(QPoint pos,CMapLevel *level,bool centerView)
{
  if ((!centerView) && (getCurrentlyViewedLevel() == level)) return;

  if (!level) { changed(); return; }

  setLevel(level);

  changed();

  enableViewControls(true);
  // Center on the position
  if (centerView)
    scroller->ensureVisible(pos.x(),pos.y(), width()/2, height()/2);

  // Update the status bar
  statusbar->setZone(mapManager->getZone());
  statusbar->setLevel(level);
  CMapRoom *cur = mapManager->getCurrentRoom();
  statusbar->setRoom(cur ? cur->getLabel() : "");

  mapManager->activeViewChanged();
}

/** This is used ensure a location is visiable for views that scroll */
void CMapView::ensureVisible(QPoint pos)
{
  scroller->ensureVisible(pos.x(),pos.y(),10,10);
}

/** Used to calculate the correct size for the widget */
void CMapView::checkSize(QPoint pos)
{
  if (pos.x() > maxSize.width()) maxSize.setWidth(pos.x());
  if (pos.y() > maxSize.height()) maxSize.setHeight(pos.y());

  int view_x = width();
  int view_y = height();

  if (maxSize.width() > view_x)
    view_x = maxSize.width();

  if (maxSize.height() > view_y)
    view_y = maxSize.height();

  QSize grid = mapManager->getMapData()->gridSize;
  view_x += grid.width() * 3;
  view_y += grid.height() * 3;

  if (view_y != mapWidget->height() || view_x != mapWidget->width())
    mapWidget->setFixedSize(view_x, view_y);
}

/** Get the max x cord of all elements */
int CMapView::getMaxX(void)
{
        return maxSize.width();
}

/** Get the max y cord of all elements */
int CMapView::getMaxY(void)
{
        return maxSize.height();
}

void CMapView::setFollowMode(bool follow)
{
  cmdFollowMode->setChecked(follow);
}

bool CMapView::getFollowMode(void)
{
  return cmdFollowMode->isChecked();
}

void CMapView::setCreateMode(bool follow)
{
  cmdCreateMode->setChecked(follow);
}

bool CMapView::getCreateMode(void)
{
  return cmdCreateMode->isChecked();
}

// Tools slots

void CMapView::slotToolsGrid()
{
  mapManager->getMapData()->gridVisable = m_toolsGrid->isChecked();
  mapManager->redrawAllViews();
}

void CMapView::levelShift(bool up)
{
  CMapLevel *level = getCurrentlyViewedLevel();
  level = up ? level->getNextLevel() : level->getPrevLevel();
  if (level) {
    showPosition(level, false);
    return;
  }

  if (KMessageBox::warningYesNo (NULL, i18n("There is no level in that direction. Do you want to create a new one?"),i18n("KMuddy Mapper")) != KMessageBox::Yes) return;

  mapManager->createLevel(up ? UP : DOWN);
}

void CMapView::slotToolsLevelUp()
{
  levelShift(true);
}

void CMapView::slotToolsLevelDown()
{
  levelShift(false);
}

void CMapView::slotToolsLevelDelete()
{
  CMapLevel *level = getCurrentlyViewedLevel();
  if (!level) return;
  int count = mapManager->getZone()->levelCount();
  if (count <= 1) return;

  if (KMessageBox::warningYesNo (NULL,i18n("Are you sure that you want to delete the current level?"),i18n("KMuddy Mapper")) != KMessageBox::Yes) return;
  mapManager->deleteLevel(level);
}

void CMapView::slotToolsZoneCreate()
{
  bool ok;
  QString name = KInputDialog::getText(i18n("KMuddy Mapper"), i18n("Please enter the name of the new zone:"), QString(), &ok);
  if (!ok) return;
  if (!name.length()) return;
  mapManager->zoneManager()->createZone(name);
}

void CMapView::slotToolsZoneDelete()
{
  CMapZoneManager *zm = mapManager->zoneManager();
  if (KMessageBox::warningYesNo (NULL,i18n("Are you sure that you want to delete the current zone? This cannot be undone."),i18n("KMuddy Mapper")) != KMessageBox::Yes) return;
  zm->deleteZone(zm->activeZone());
}

void CMapView::slotViewUpperLevel()
{
  mapManager->getMapData()->showUpperLevel = m_viewUpperLevel->isChecked();
  mapManager->redrawAllViews();
}

void CMapView::slotViewLowerLevel()
{
  mapManager->getMapData()->showLowerLevel = m_viewLowerLevel->isChecked();
  mapManager->redrawAllViews();
}

/** Used to room under the point the current room */
void CMapView::slotRoomSetCurrentPos(void)
{
  mapManager->setCurrentRoom((CMapRoom *)m_selectedElement);
}

/** Used to room under the point the login room */
void CMapView::slotRoomSetLogin(void)
{
  mapManager->setLoginRoom((CMapRoom *)m_selectedElement);
}

/** Used to set speedwalk to the room under the pointer */
void CMapView::slotRoomSpeedwalkTo(void)
{
  mapManager->walkPlayerTo((CMapRoom *)m_selectedElement);
}

/** Used to delete the room under the pointer */
void CMapView::slotRoomDelete(void)
{
  mapManager->deleteElement(m_selectedElement);
}

/** Used to display the properties of the room under the pointer */
void CMapView::slotRoomProperties(void)
{
  mapManager->propertiesRoom((CMapRoom *)m_selectedElement);
}

/** Used to make the path under the pointer one way */
void CMapView::slotPathOneWay(void)
{
  mapManager->makePathOneWay((CMapPath *)m_selectedElement);
}

/** Used to make the path under the pointer two way */
void CMapView::slotPathTwoWay(void)
{
  mapManager->makePathTwoWay((CMapPath *)m_selectedElement);
}

/** Used to add a bend to the path under the pointer */
void CMapView::slotPathAddBend(void)
{
  kDebug() << "CMapView::CMapManager slotPathAddBend";
  mapManager->openCommandGroup(i18n("Add Bend"));
  CMapPath *path = (CMapPath *)m_selectedElement;

  path->addBendWithUndo(m_selectedPos);
  if (path->getOpsitePath())
  {
    path->getOpsitePath()->addBendWithUndo(m_selectedPos);
  }
  m_clipboard->slotUnselectAll();
  path->setEditMode(true);
  changedElement(path);

  mapManager->closeCommandGroup();
}

/** Used to delete the path segment under the pointer */
void CMapView::slotPathDelBend(void)
{
  mapManager->openCommandGroup(i18n("Delete Path Segment"));
  CMapPath *path = (CMapPath *)m_selectedElement;

  int seg = path->mouseInPathSeg(m_selectedPos, getCurrentlyViewedZone());

  path->deletePathSegWithUndo(seg);
  if (path->getOpsitePath())
  {
    int seg = path->getOpsitePath()->mouseInPathSeg(m_selectedPos, getCurrentlyViewedZone());
    path->getOpsitePath()->deletePathSegWithUndo(seg);
  }

  mapManager->changedElement(path);
  mapManager->closeCommandGroup();
}

/** Used to edit the bends of the path under the pointer */
void CMapView::slotPathEditBends(void)
{
  CMapPath *path = (CMapPath *)m_selectedElement;

  m_clipboard->slotUnselectAll();
  path->setEditMode(true);
  mapManager->changedElement(path);
}

/** Used to delete the path under the pointer */
void CMapView::slotPathDelete(void)
{
  mapManager->deleteElement(m_selectedElement);
}

/** Used to display the properties of the path under the pointer */
void CMapView::slotPathProperties(void)
{
  mapManager->propertiesPath((CMapPath *)m_selectedElement);
}

/** Used to delete the text element under the pointer */
void CMapView::slotTextDelete(void)
{
  mapManager->deleteElement(m_selectedElement);
}

/** Used to display the text properties of the text element under the pointer */
void CMapView::slotTextProperties(void)
{
  mapManager->propertiesText((CMapText *)m_selectedElement);
}

/** Used to change the position of room/zone labels */
void CMapView::slotChangeLabelPos()
{
  if (m_selectedElement->getElementType()==ROOM)
  {
    CMapRoom *room = (CMapRoom *)m_selectedElement;

    CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Change room label position"),room);

    command->getOrgProperties().writeEntry("LabelPos",(int)room->getLabelPosition());

    switch(labelMenu->currentItem())
    {
      case 0 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::HIDE); break;
      case 1 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::NORTH); break;
      case 2 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::NORTHEAST); break;
      case 3 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::EAST); break;
      case 4 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::SOUTHEAST); break;
      case 5 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::SOUTH); break;
      case 6 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::SOUTHWEST); break;
      case 7 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::WEST); break;
      case 8 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::NORTHWEST); break;
      case 9 : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::CUSTOM); break;
      default : command->getNewProperties().writeEntry("LabelPos",(int)CMapRoom::HIDE); break;
    }

    mapManager->addCommand(command);
  }

  if (m_selectedElement->getElementType()==ZONE)
  {
    CMapZone *zone = (CMapZone *)m_selectedElement;

    CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Change zone label position"),zone);

    command->getOrgProperties().writeEntry("LabelPos",(int)zone->getLabelPosition());

    switch(labelMenu->currentItem())
    {
      case 0 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::HIDE); break;
      case 1 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::NORTH); break;
      case 2 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::NORTHEAST); break;
      case 3 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::EAST); break;
      case 4 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::SOUTHEAST); break;
      case 5 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::SOUTH); break;
      case 6 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::SOUTHWEST); break;
      case 7 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::WEST); break;
      case 8 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::NORTHWEST); break;
      case 9 : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::CUSTOM); break;
      default : command->getNewProperties().writeEntry("LabelPos",(int)CMapZone::HIDE); break;
    }
    mapManager->addCommand(command);
  }
}


int CMapView::getWidth(void)
{
  if (mapWidget->width() > scroller->viewport()->width())
    return mapWidget->width();
  return scroller->viewport()->width();
}

int CMapView::getHeight(void)
{
  if (mapWidget->height() > scroller->viewport()->height())
    return mapWidget->height();
  return scroller->viewport()->height();
}

void CMapView::setCursor ( const QCursor & cursor)
{
  if (mapWidget)
    mapWidget->setCursor(cursor);
}

void CMapView::resizeEvent (QResizeEvent *)
{
  changed();
}

/** Used to set the view to active */
void CMapView::focusInEvent(QFocusEvent *)
{
}

void CMapView::closeEvent(QCloseEvent *)
{
}

void CMapView::slotWidgetBeingClosed()
{
}

void CMapView::slotDockWindowClose()
{
}

bool CMapView::queryClose()
{
  emit closed();
  return true;
}

