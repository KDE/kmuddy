/********************************************f*******************************
                          mapmanager.cpp  -  description
                             -------------------
    begin                : Wed Mar 7 2001
    copyright            : (C) 2001 by KMud Development Team
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

#include "cmapmanager.h"

#include <kdebug.h>
#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kservicetypetrader.h>
#include <kmessagebox.h>
#include <kstandardaction.h>
#include <kfiledialog.h>
#include <kpagedialog.h>
#include <kcomponentdata.h>
#include <kxmlguifactory.h>

#include <q3ptrqueue.h>
#include <qtimer.h>
#include <q3valuelist.h>
#include <QActionGroup>
#include <Q3PtrList>

#include "cmapzone.h"
#include "cmappath.h"
#include "cmaptext.h"
#include "cmaproom.h"
#include "cmapview.h"
#include "cmaplevel.h"
#include "cmapviewbase.h"
#include "cmaptoolbase.h"
#include "cmappluginbase.h"
#include "cmapclipboard.h"

#include "cmapcmdelementcreate.h"
#include "cmapcmdelementdelete.h"
#include "cmapcmdelementproperties.h"
#include "cmapcmdgroup.h"
#include "cmapcmdmovemap.h"
#include "cmapcmdleveldelete.h"
#include "cmapcmdlevelcreate.h"
#include "cmapcmdtogglepathtwoway.h"
#include "cmapfilefilterbase.h"

#include "kmuddy_mapper.h"

#include "filefilters/cmapfilefilterxml.h"

#include "dialogs/dlgmaproomproperties.h"
#include "dialogs/dlgmappathproperties.h"
#include "dialogs/dlgmaptextproperties.h"
#include "dialogs/dlgmapzoneproperties.h"
#include "dialogs/dlgspeedwalkprogress.h"
#include "dialogs/dlgmapinfo.h"
#include "dialogs/dlgmapspeedwalk.h"
#include "dialogs/dlgmapcolor.h"
#include "dialogs/dlgmapdirections.h"
#include "dialogs/dlgmapmovement.h"

#include "cactionmanager.h"
#include "cdialoglist.h"
#include "cglobalsettings.h"
#include "cstatus.h"

CMapManager::CMapManager (KMuddyMapper *mapper) :
  KXmlGuiWindow (NULL),
  cActionBase ("map-manager", 0),
  mapperPlugin (mapper)
{
  kDebug() << "constructor begins";
  setCaption (i18n ("Mapper"));
  setAttribute (Qt::WA_DeleteOnClose, false);  // do not delete on close

  // register action handlers
  addEventHandler ("dialog-create", 50, PT_STRING);
  addEventHandler ("dialog-save", 50, PT_STRING);

  mapData = new CMapData();

  // Setup vars
  loginRoom = NULL;
  currentRoom = NULL;
  elementEdit = NULL;

  m_elementUtils = new CMapElementUtil(this);

  initMenus();
  initPlugins();
  initFileFilters();

  container = new KVBox (this);
  container->show ();
  setCentralWidget (container);

  m_clipboard = new CMapClipboard(this,actionCollection(),"mapClipboard");

  activeView = 0;

  setDefaultOptions();
  readOptions();

  enableViewControls(false);

  speedwalkActive = false;
  pathToWalk.setAutoDelete(true);

  speedwalkProgressDlg = new DlgSpeedwalkProgress();
  speedwalkProgressDlg->hide();
  connect(speedwalkProgressDlg,SIGNAL(abortSpeedwalk()),this,SLOT(slotAbortSpeedwalk()));

  // set up the menus
  createGUI (KStandardDirs::locate("appdata", "kmuddymapperpart.rc"));

  /** Create undo/redo history */
  commandHistory = new K3CommandHistory(actionCollection(),true);
  //FIXME_jp: Needs to be configurable
  commandHistory->setUndoLimit(30);
  commandHistory->setRedoLimit(30);
  commandHistory->clear();
  historyGroup = NULL;
  m_commandsActive = true;

  m_zoneCount = 0;
  m_levelCount = 0;

  setUndoActive (false);
  activeView = new CMapView(this,container,"mapView");
  createNewMap();
  openMapView ();
  setUndoActive (true);

  kDebug() << "constructor ends";
}

CMapManager::~CMapManager()
{
  kDebug() << "CMapManager::~CMapManager() start";
  removeEventHandler ("dialog-create");
  removeEventHandler ("dialog-save");

  if (mapData)
    delete mapData;
  mapData = 0;
  delete activeView;

  if (commandHistory)
    delete commandHistory;

  delete m_elementUtils;

  kDebug() << "CMapManager::~CMapManager() end";
}

KComponentData CMapManager::instance ()
{
  return mapperPlugin->componentData();
}

void CMapManager::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "dialog-create") {
    if (par1 == "profile-prefs")
      createProfileConfigPanes ();
    else if (par1 == "app-prefs")
      createGlobalConfigPanes ();
  }
  if (event == "dialog-save") {
    if (par1 == "profile-prefs") {
    } else if (par1 == "app-prefs") {
      // TODO: the first two should be profile prefs
      mapDirection->slotOkPressed();
      mapMovement->slotOkPressed();

      mapColor->slotOkPressed();
      mapSpeedwalk->slotOkPressed();
      saveGlobalConfig ();
    }
  }
}

void CMapManager::createProfileConfigPanes ()
{
  KPageDialog *dlg = (KPageDialog *) cDialogList::self()->getDialog ("profile-prefs");

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
    plugin->createProfileConfigPanes();
}

void CMapManager::createGlobalConfigPanes ()
{
  KPageDialog *dlg = (KPageDialog *) cDialogList::self()->getDialog ("app-prefs");

  KPageWidgetItem *item;
  QFrame *frmdir = new QFrame (this);
  item = dlg->addPage (frmdir, i18n ("Mapper: Directions"));
  item->setIcon (KIcon ("gear"));
  QFrame *frmmovement = new QFrame (this);
  item = dlg->addPage (frmmovement, i18n ("Mapper: Movement"));
  item->setIcon (KIcon ("run"));
  QFrame *frmcolor = new QFrame (this);
  item = dlg->addPage (frmcolor, i18n ("Mapper: Colors"));
  item->setIcon (KIcon ("colorize"));
  QFrame *frmspeedwalk = new QFrame (this);
  item = dlg->addPage (frmspeedwalk, i18n ("Mapper: Speedwalk"));
  item->setIcon (KIcon ("launch"));

  QVBoxLayout *dirlayout = new QVBoxLayout (frmdir);
  QVBoxLayout *movementlayout = new QVBoxLayout (frmmovement);
  QVBoxLayout *colorlayout = new QVBoxLayout (frmcolor);
  QVBoxLayout *speedwalklayout = new QVBoxLayout (frmspeedwalk);
  mapDirection = new DlgMapDirections (this, frmdir);
  mapMovement = new DlgMapMovement (this, frmmovement);
  mapColor = new DlgMapColor(this, frmcolor);
  mapSpeedwalk = new DlgMapSpeedwalk(this, frmspeedwalk);
  dirlayout->addWidget (mapDirection);
  movementlayout->addWidget (mapMovement);
  colorlayout->addWidget (mapColor);
  speedwalklayout->addWidget (mapSpeedwalk);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
    plugin->createGlobalConfigPanes();
}

/** This method tells the plugins that the mapper wants to add a room to the speedwalk list
  * @param room The room to add to the speedwalk list
  */
void CMapManager::addSpeedwalkRoom(CMapRoom *room)
{
  for (CMapPluginBase *plugin=getPluginList()->first();plugin!=0; plugin=getPluginList()->next())
  {
    plugin->addSpeedwalkRoom(room);
  }
}


/** This will setup the import/export file filters */
void CMapManager::initFileFilters()
{
  m_fileFilter.setAutoDelete(true);

  m_fileFilter.append(new CMapFileFilterXML(this));
}

void CMapManager::initMenus()
{
  kDebug() << "begin initMenus";

  // File menu

  m_fileNew = new KAction (this);
  m_fileNew->setText (i18n("New Map"));
  connect  (m_fileNew, SIGNAL (triggered()), this, SLOT (slotFileNew()));
  actionCollection()->addAction ("fileNew", m_fileNew);
  m_fileLoad = new KAction (this);
  m_fileLoad->setText (i18n("Load"));
  connect  (m_fileLoad, SIGNAL (triggered()), this, SLOT (slotFileLoad()));
  actionCollection()->addAction ("fileLoad", m_fileLoad);
  m_fileSave = new KAction (this);
  m_fileSave->setText (i18n("Save As"));
  connect  (m_fileSave, SIGNAL (triggered()), this, SLOT (slotFileSave()));
  actionCollection()->addAction ("fileSave", m_fileSave);
  m_fileInfo = new KAction (this);
  m_fileInfo->setText (i18n("Information"));
  connect  (m_fileInfo, SIGNAL (triggered()), this, SLOT (slotFileInfo()));
  actionCollection()->addAction ("fileInfo", m_fileInfo);

  // Tools menu
  m_toolsCreate = new KToggleAction (this);
  m_toolsCreate->setText ( i18n("&Create Mode"));
  m_toolsCreate->setIcon (BarIcon("kmud_create.png"));
  connect  (m_toolsCreate, SIGNAL (triggered()), this, SLOT(slotToolsCreateMode()));
  actionCollection()->addAction ("toolsCreate", m_toolsCreate);
  m_toolsGrid = new KToggleAction (this);
  m_toolsGrid->setText ( i18n("&Grid"));
  m_toolsGrid->setIcon (BarIcon("kmud_grid.png"));
  connect (m_toolsGrid, SIGNAL (triggered()), this, SLOT(slotToolsGrid()));
  actionCollection()->addAction ("toolsGrid", m_toolsGrid);
  m_toolsUpLevel = new KAction (this);
  m_toolsUpLevel->setText ( i18n("Display Upper Level"));
  m_toolsUpLevel->setIcon (BarIcon("kmud_lvlup.png"));
  connect (m_toolsUpLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelUp()));
  actionCollection()->addAction ("toolsLevelUp", m_toolsUpLevel);
  m_toolsDownLevel = new KAction (this);
  m_toolsDownLevel->setText ( i18n("Display Lower Level"));
  m_toolsDownLevel->setIcon (BarIcon("kmud_lvldown.png"));
  connect (m_toolsDownLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelDown()));
  actionCollection()->addAction ("toolsLevelDown", m_toolsDownLevel);
  m_toolsDeleteLevel = new KAction (this);
  m_toolsDeleteLevel->setText ( i18n("Delete Current Level"));
  connect  (m_toolsDeleteLevel, SIGNAL (triggered()), this, SLOT(slotToolsLevelDelete()));
  actionCollection()->addAction ("toolsLevelDelete", m_toolsDeleteLevel);
  m_toolsUpZone = new KAction (this);
  m_toolsUpZone->setText ( i18n("Display Parent Zone"));
  m_toolsUpZone->setIcon (BarIcon("kmud_zoneup.png"));
  connect (m_toolsUpZone, SIGNAL (triggered()), this, SLOT(slotToolsZoneUp()));
  actionCollection()->addAction ("toolsZoneUp", m_toolsUpZone);
  m_toolsZoneProp = new KAction (this);
  m_toolsZoneProp->setText ( i18n("Edit Zone Properties"));
  connect  (m_toolsZoneProp, SIGNAL (triggered()), this, SLOT(slotToolsZoneProperties()));
  actionCollection()->addAction ("toolsZoneProperties", m_toolsZoneProp);
  m_toolsDeleteZone = new KAction (this);
  m_toolsDeleteZone->setText ( i18n("Delete Current zone"));
  connect  (m_toolsDeleteZone, SIGNAL (triggered()), this, SLOT(slotToolsDeleteZone()));
  actionCollection()->addAction ("toolsDeleteZone", m_toolsDeleteZone);

  // View menu
  m_viewUpperLevel = new KToggleAction (this);
  m_viewUpperLevel->setText ( i18n("Map Upper Level"));
  connect  (m_viewUpperLevel, SIGNAL (triggered()), this, SLOT(slotViewUpperLevel()));
  actionCollection()->addAction ("viewUpperLevel", m_viewUpperLevel);
  m_viewLowerLevel = new KToggleAction (this);
  m_viewLowerLevel->setText ( i18n("Map Lower Level"));
  connect  (m_viewLowerLevel, SIGNAL (triggered()), this, SLOT(slotViewLowerLevel()));
  actionCollection()->addAction ("viewLowerLevel", m_viewLowerLevel);
  m_viewToolsToolbar = new KToggleAction (this);
  m_viewToolsToolbar->setText ( i18n("Tools Toolbar"));
  connect  (m_viewToolsToolbar, SIGNAL (triggered()), this, SLOT(slotViewToolsToolbar()));
  actionCollection()->addAction ("viewToolsToolbar", m_viewToolsToolbar);
  m_viewToolsToolbar->setChecked(true);
  m_viewNavToolbar = new KToggleAction (this);
  m_viewNavToolbar->setText ( i18n("Navigation Toolbar"));
  connect  (m_viewNavToolbar, SIGNAL (triggered()), this, SLOT(slotViewNavToolbar()));
  actionCollection()->addAction ("viewNavToolbar", m_viewNavToolbar);
  m_viewNavToolbar->setChecked(true);

  // Label Submenu
  zoneMenu = new KSelectAction (this);
  zoneMenu->setText (i18n("Select Zone"));
  connect (zoneMenu, SIGNAL (triggered()), this, SLOT(slotSelectZone()));
  actionCollection()->addAction ("toolsZoneList", zoneMenu);

  // Room Popup Actions
  KAction *action;
  action = new KAction (this);
  action->setText (i18n("Set &Current Position"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSetCurrentPos()));
  actionCollection()->addAction ("roomCurrentPos", action);
  action = new KAction (this);
  action->setText (i18n("Set Room to &Login Point"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSetLogin()));
  actionCollection()->addAction ("roomLoginPoint", action);
  action = new KAction (this);
  action->setText (i18n("&Speed walk to room"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomSpeedwalkTo()));
  actionCollection()->addAction ("roomWalkTo", action);
  action = new KAction (this);
  action->setText (i18n("&Delete room"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomDelete()));
  actionCollection()->addAction ("roomDelete", action);
  action = new KAction (this);
  action->setText (i18n("&Properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotRoomProperties()));
  actionCollection()->addAction ("roomProperties", action);

    // Text Popup Actions
  action = new KAction (this);
  action->setText (i18n("&Delete Text"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotTextDelete()));
  actionCollection()->addAction ("textDelete", action);
  action = new KAction (this);
  action->setText (i18n("&Properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotTextProperties()));
  actionCollection()->addAction ("textProperties", action);

  // Zone Popup Actions
  action = new KAction (this);
  action->setText (i18n("&Open Zone"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotZoneOpen()));
  actionCollection()->addAction ("zoneOpen", action);
  action = new KAction (this);
  action->setText (i18n("&Open Zone in New View"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotZoneOpenNewView()));
  actionCollection()->addAction ("zoneOpenNewView", action);
  action = new KAction (this);
  action->setText (i18n("&Delete Zone"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotZoneDelete()));
  actionCollection()->addAction ("zoneDelete", action);
  action = new KAction (this);
  action->setText (i18n("&Properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotZoneProperties()));
  actionCollection()->addAction ("zoneProperties", action);

  // Path Popup Actions
  action = new KToggleAction (this);
  action->setText (i18n("&One way"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathOneWay()));
  actionCollection()->addAction ("pathOneWay", action);
  action = new KToggleAction (this);
  action->setText (i18n("&Two way"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathTwoWay()));
  actionCollection()->addAction ("pathTwoWay", action);
  action = new KAction (this);
  action->setText (i18n("&Add Bend"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathAddBend()));
  actionCollection()->addAction ("pathAddBend", action);
  action = new KAction (this);
  action->setText (i18n("&Remove Segment"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathDelBend()));
  actionCollection()->addAction ("pathDelBend", action);
  action = new KAction (this);
  action->setText (i18n("&Edit Bends"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathEditBends()));
  actionCollection()->addAction ("pathEditBends", action);
  action = new KAction (this);
  action->setText (i18n("&Delete Path"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathDelete()));
  actionCollection()->addAction ("pathDelete", action);
  action = new KAction (this);
  action->setText (i18n("&Properties"));
  connect (action, SIGNAL (triggered()), this, SLOT(slotPathProperties()));
  actionCollection()->addAction ("pathPorperties", action);


  QStringList labelPos;
  labelPos.append(i18n("Hide"));
  labelPos.append(directionToText(NORTH,""));
  labelPos.append(directionToText(NORTHEAST,""));
  labelPos.append(directionToText(EAST,""));
  labelPos.append(directionToText(SOUTHEAST,""));
  labelPos.append(directionToText(SOUTH,""));
  labelPos.append(directionToText(SOUTHWEST,""));
  labelPos.append(directionToText(WEST,""));
  labelPos.append(directionToText(NORTHWEST,""));
  labelPos.append(i18n("Custom"));
  
  labelMenu = new KSelectAction (this);
  labelMenu->setText (i18n("&Label"));
  connect (labelMenu, SIGNAL (triggered()), this, SLOT(slotChangeLabelPos()));
  actionCollection()->addAction ("labelMenu", labelMenu);
  labelMenu->setItems(labelPos);

  // tool action group
  m_toolGroup = new QActionGroup (this);
}

/** Used to create the plugins */
void CMapManager::initPlugins()
{
  int pluginCount = 0;
  toolList.setAutoDelete(false);
  toolList.clear();
  pluginList.setAutoDelete(false);
  pluginList.clear();
  kDebug() << "Loading Plugins...\n";

  KService::List offers = KServiceTypeTrader::self()->query("KMuddy/Mapper/Plugin");
  kDebug() << "got " << offers.count() << " offers for plugins\n";

  CMapPluginBase* plugin;

  for (unsigned int i = 0; i < (unsigned int) offers.count(); ++i)
  {
    kDebug() << "Processing offer "
      << (i+1)
      << ": \"" << offers[i]->name()
      << "\" (" << offers[i]->library() << ")\n";

    KPluginFactory *factory = KPluginLoader (*offers[i]).factory();
    if (!factory) {
      kWarning() << "Obtaining factory failed!";
      continue;
    }
    plugin = factory->create<CMapPluginBase> (this);

    if (!plugin)
    {
      kWarning() << "Plugin creation failed!";
    }
    else
    {
      kDebug() << "Add Plugin : " << plugin->name();
      pluginList.append(plugin);
  
      kDebug() << "Tools in plugin : " << plugin->getToolList()->count();
      for (CMapToolBase *tool =plugin->getToolList()-> first();tool!=0; tool= plugin->getToolList()->next())
      {
        kDebug() << "Add Tool : " << tool->name();
        toolList.append(tool);
      }

      insertChildClient(plugin);
      
      pluginCount++;
    }
  }

  kDebug() << "Finished loading " << pluginCount << " plugins\n";
  kDebug() << "Finished loading " << toolList.count() << " tools\n";
  
  if (toolList.count() > 0)
  {

    currentTool = toolList.first();
    currentTool->setChecked(true);
  }
  else
  {
    kWarning() << "No tools loaded!\n";
    currentTool = NULL;
  }

    if (pluginCount==0)
    {
    kWarning() << "No plugins loaded!\n";
    }

     kDebug() << "XML File : " << xmlFile(); 
}

void CMapManager::updateZoneListCombo(void)
{
  QStringList lst;

  for (CMapZone *zone = mapData->getFirstZone(); zone!=0; zone = mapData->getNextZone())
  {
    lst.append(zone->getLabel());
  }

  zoneMenu->clear();
  zoneMenu->setItems(lst);

  if (activeView)
  {
    CMapZone *zone = activeView->getCurrentlyViewedZone();

    if (zone)
    {
      zoneMenu->setCurrentItem(lst.findIndex(zone->getLabel()));
    }
  }
}

/** Used to get a list of the plugins */
Q3PtrList<CMapPluginBase> *CMapManager::getPluginList()
{
  return &pluginList;
}

/** Used to get a pointer to the map data */
CMapData *CMapManager::getMapData() const
{
  return mapData;
}

/** Used to create a new view of the map */
void CMapManager::openMapView()
{
  if (loginRoom)
    activeView->showPosition(QPoint(loginRoom->getX(),loginRoom->getY()),loginRoom->getLevel());
  else
  {
    CMapRoom *firstRoom = findFirstRoom(NULL);
    if (firstRoom)
    {
      activeView->showPosition(firstRoom->getLevel());
    }
  }
  enableViewControls(true);
}

void CMapManager::openNewMapView(CMapLevel *level)
{
  CMapViewBase *mapView = getActiveView();
  mapView->showPosition(level);
  enableViewControls(true);
}

/** This method is used to covert cords so that they snap to the grid */
QPoint CMapManager::cordsSnapGrid(QPoint oldPos)
{
  QPoint newPos;

  int oldx = (int)(oldPos.x() / mapData->gridSize.width());
  int oldy =  (int)(oldPos.y() / mapData->gridSize.height());
  newPos.setX( oldx * mapData->gridSize.width());
  newPos.setY( oldy * mapData->gridSize.height());

  return newPos;
}

/**
 * Used to create a new view of the a given level and
 * center the view on the given position.
 */
void CMapManager::openNewMapView(QPoint pos,CMapLevel *level)
{
  CMapViewBase *mapView = getActiveView();
  mapView->showPosition(pos,level);
}

/** Used to set properties of the view widget */
void CMapManager::setPropertiesAllViews(QCursor *cursor,bool mouseTracking)
{  
  activeView->setCursor(*cursor);
  activeView->setMouseTracking(mouseTracking);
}

/**
 * Used to enable/disable the view actions
 * @param If true then enable the actions otherwise disable the actions
 */
void CMapManager::enableViewControls(bool enabled)
{
  if (!mapData) return;  // if we don't have mapData, we're going down
  enableNonViewActions(enabled);
  m_clipboard->enableActions(enabled);
  m_toolsUpLevel->setEnabled(enabled);
  m_toolsDownLevel->setEnabled(enabled);
  m_toolsDeleteLevel->setEnabled(enabled);
  m_toolsUpZone->setEnabled(enabled);
  m_toolsZoneProp->setEnabled(enabled);
  m_toolsDeleteZone->setEnabled(enabled);
  
  m_fileNew->setEnabled(enabled);
  m_fileSave->setEnabled(enabled);
  m_fileLoad->setEnabled(enabled);
}

/**
 * This method is used to disable/enable mapper actions that are not done by enableViewControls()
 * @param If true then enable the actions otherwise disable the actions
 */
void CMapManager::enableNonViewActions(bool enabled)
{
  m_fileInfo->setEnabled(enabled);

  m_toolsCreate->setEnabled(enabled);
  m_toolsGrid->setEnabled(enabled);
}

/** Used to unselect all the elements in a level */
void CMapManager::unselectElements(CMapLevel *level)
{
  for (CMapElement *element = level->getFirstElement() ; element!=0; element=level->getNextElement())
  {
    element->setSelected(false);
    element->setEditMode(false);
  }  
}

/** Used to convert a text direction to a direction type */
QString CMapManager::directionToText(directionTyp dir,QString specialCmd)
{
  if ((uint)dir < NUM_DIRECTIONS)
  {
    return mapData->directions[dir];
  }
  else
  {
    return specialCmd;
  }
}

/** Used to convert a direction type to a text direction */
directionTyp CMapManager::textToDirection(QString text)
{
  directionTyp dir = SPECIAL;  

  for (uint i = 0; i < NUM_DIRECTIONS; ++i)
  {
    if (text == mapData->directions[i])
    {
      if (i>9)
      {
        dir = (directionTyp)(i-10);
      }
      else
      {
        dir =  (directionTyp)i;
      }
      break;
    }
  }
  
  return dir;
}

/** Used to set the login room */
void CMapManager::setLoginRoom(CMapRoom *room)
{
  openCommandGroup(i18n("Change Login Room"));
  if (loginRoom)
  {
    CMapCmdElementProperties *cmdRemove = new CMapCmdElementProperties(this,i18n("Remove Login Room Status"),loginRoom);
    cmdRemove->getOrgProperties().writeEntry("Login",true);
    cmdRemove->getNewProperties().writeEntry("Login",false);

    addCommand(cmdRemove);
  }

  CMapCmdElementProperties *cmdSet = new CMapCmdElementProperties(this,i18n("Set Login Room Status"),room);

  cmdSet->getOrgProperties().writeEntry("Login",false);
  cmdSet->getNewProperties().writeEntry("Login",true);

  addCommand(cmdSet);

  closeCommandGroup();
}

/** Uesd to return the login room */
CMapRoom *CMapManager::getLoginRoom()
{
  return loginRoom;
}

/** Should only be called by CMapRoom.setCurrentRoom() */
void CMapManager::setCurrentRoomWithoutUndo(CMapRoom *room)
{
  currentRoom = room;
}

/** Should only be called by CMapRoom.setLoginRoom() */
void CMapManager::setLoginRoomWithoutUndo(CMapRoom *room)
{
  loginRoom = room;
}


/** Used to set the current room */
void CMapManager::setCurrentRoom(CMapRoom *room)
{
  openCommandGroup(i18n("Change Current Room"));

  CMapCmdElementProperties *cmdRemove = new CMapCmdElementProperties(this,i18n("Remove Current Room Status"),currentRoom);
  cmdRemove->getOrgProperties().writeEntry("Current",true);
  cmdRemove->getNewProperties().writeEntry("Current",false);
  addCommand(cmdRemove);

  CMapCmdElementProperties *cmdSet = new CMapCmdElementProperties(this,i18n("Set Current Room Status"),room);
  cmdSet->getOrgProperties().writeEntry("Current",false);
  cmdSet->getNewProperties().writeEntry("Current",true);
  addCommand(cmdSet);

  closeCommandGroup();
}

/** Uesd to return the current room */
CMapRoom *CMapManager::getCurrentRoom()
{
  return currentRoom;
}

/** Used to find any elements at the given location
  * @param pos The location to look for the element
  * @param level The level to look for the element on
  * @return NULL if no element was found otherwise a pointer to the element */
CMapElement *CMapManager::findElementAt(QPoint pos,CMapLevel *level)
{
  if (!level)
    return NULL;

  for ( CMapRoom *room=level->getRoomList()->first(); room != 0; room=level->getRoomList()->next() )
  {
    if (room->mouseInElement(pos,level->getZone()))
    {
      return (CMapElement *) room;
    }
  }

  for ( CMapZone *zone=level->getZoneList()->first(); zone != 0; zone=level->getZoneList()->next() )
  {
    if (zone->mouseInElement(pos,level->getZone()))
    {
      return (CMapElement *)zone;
    }
  }

  for ( CMapText *text=level->getTextList()->first(); text != 0; text = level->getTextList()->next())
  {
    if (text->mouseInElement(pos,level->getZone()))
    {
      return (CMapElement *)text;
    }
  }

  return NULL;
}

/** This method is used to find a element from a list of properties
  * @param properties The list of proerties
  * @return The element if it's found otherwise NULL */
CMapElement *CMapManager::findElement(KConfigGroup properties)
{
  CMapElement *result = NULL;

  elementTyp type = (elementTyp)properties.readEntry("Type",(uint)OTHER);

  if (type!=OTHER)
  {
    if (type==PATH)
    {
      CMapLevel *srcLevel = findLevel(properties.readEntry("SrcLevel",-1));
      CMapRoom *srcRoom = srcLevel->findRoom(properties.readEntry("SrcRoom",-1));
      directionTyp srcDir = (directionTyp)properties.readEntry("SrcDir",0);

      QString specialCommand = properties.readEntry("SpecialCmdSrc","");
      result = srcRoom->getPathDirection(srcDir,specialCommand);

    }
    else
    {
      CMapLevel *level = findLevel(properties.readEntry("Level",-5));
      if (level)
      {
        if (type==ROOM)
        {
          result = level->findRoom(properties.readEntry("RoomID",-5));
        }
        else if (type==ZONE)
        {
          result = findZone(properties.readEntry("ZoneID",-5));
        }
        else
        {
          int x = properties.readEntry("X",-5);
          int y = properties.readEntry("Y",-5);

          for ( CMapText *text=level->getTextList()->first(); text != 0; text = level->getTextList()->next())
          {
            if (text->getX()==x && text->getY()==y)
            {
              result = text;
              break;
            }
          }
        }
      }
    }
  }

  return result;
}

/** Used to find any rooms at the given location
  * @param pos The location to look for the room
  * @param level The level to look for the room on
  * @return NULL if no room was found otherwise a pointer to the room */
CMapRoom *CMapManager::findRoomAt(QPoint pos,CMapLevel *level)
{
  if (!level)
    return NULL;

  for ( CMapRoom *room=level->getRoomList()->first(); room != 0; room=level->getRoomList()->next() )
  {
    if (room->mouseInElement(pos,level->getZone()))
    {
      return room;
    }
  }  

  return NULL;
}

/** Used to erase the map. This will erase all elements and can't be undone */
void CMapManager::eraseMap(void)
{
  eraseZone(mapData->rootZone);
  delete mapData->rootZone;
  mapData->rootZone = NULL;

  m_zoneCount = 0;
  m_levelCount = 0;

  activeView->setLevel(NULL);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
  {
    plugin->mapErased();
  }

  loginRoom = NULL;
  currentRoom = NULL;
  elementEdit = NULL;
}

void CMapManager::eraseZone(CMapZone *zone)
{
  if (zone == 0)
    return;
  
  for ( CMapLevel *level=zone->getLevels()->first(); level != 0; level=zone->getLevels()->next())
  {
    for ( CMapRoom *room=level->getRoomList()->first(); room != 0; room=level->getRoomList()->next() )
    {        
      room->getPathList()->clear();
      room->getConnectingPathList()->clear();
    }
    level->getRoomList()->clear();
    level->getTextList()->clear();
    for ( CMapZone* subZone=level->getZoneList()->first(); subZone != 0; subZone=level->getZoneList()->next() )
    {
      eraseZone(subZone);
    }
    level->getZoneList()->clear();
  }
  zone->getLevels()->clear();
}

/** Create new bottom or top level depending on the given direction */
CMapLevel *CMapManager::createLevel(directionTyp dir,CMapZone *intoZone)
{
  if (intoZone == NULL)
  {
    kDebug() << "CMapManager::createLevel : Unable to create level, intoZone == NULL";    
    return NULL;  
  }

  CMapLevel *result = NULL;

  CMapCmdLevelCreate *cmd = NULL;

  if (getUndoActive())
  {
    if (dir==UP)
    {    
      cmd = new CMapCmdLevelCreate(this,i18n("Create Level"),intoZone,intoZone->getLevels()->count());
    }
    else
    {
      cmd = new CMapCmdLevelCreate(this,i18n("Create Level"),intoZone,0);
    }

    addCommand(cmd);
    
    result = cmd->getLevel();
  }
  else
  {
    if (dir==UP)
    {
      result = m_elementUtils->createLevel(intoZone->getLevels()->count(),intoZone);
    }
    else
    {
      result = m_elementUtils->createLevel(0,intoZone);
    }
  }
  
  return result;
}

/** This is used to find a level with a given id
  * @param id The id of the level to find
  * @return Null if no level is found otherwise a pointer to the level */
CMapLevel *CMapManager::findLevel(unsigned int id)
{
  for (CMapZone *zone = getMapData()->getFirstZone();zone!=0;zone = getMapData()->getNextZone())
  {
    for (CMapLevel *level = zone->getLevels()->first();level!=0;level = zone->getLevels()->next())
    {
      
      if (level->getLevelID()==id)
        return level;
    }
  }

  return NULL;
}



/** This is used to find a zone with a given id
  * @param id The id of the zone to find
  * @return Null if no zone is found otherwise a pointer to the zone */
CMapZone *CMapManager::findZone(unsigned int id)
{  
  for (CMapZone *zone = getMapData()->getFirstZone();zone!=0;zone = getMapData()->getNextZone())
  {    
    if (zone->getZoneID()==id)
      return zone;
  }
  
  return NULL;
}

void CMapManager::listZones(CMapZone *zone)
{  
  for (CMapLevel *level = zone->getLevels()->first(); level!=0; level = zone->getLevels()->next())
  {
    for (CMapZone *z = level->getZoneList()->first(); z !=0; z = level->getZoneList()->next())
    {
      listZones(z);
      
    }
  }
}


/** Create new map */
void CMapManager::createNewMap()
{
  // Create the root zone
  getMapData()->rootZone = NULL;  


  CMapZone *zone = createZone(QPoint(-1,-1),NULL);

  // Create a empty room in the first level of the new zone
  CMapRoom *room = createRoom(QPoint(2 * mapData->gridSize.width(),2 * mapData->gridSize.height()),zone->getLevels()->first());
  setCurrentRoomWithoutUndo(room);
  setLoginRoomWithoutUndo(room);

  activeView->showPosition(currentRoom->getLowPos(),zone->getLevels()->first());
  updateZoneListCombo();

  if (activeView->getCurrentlyViewedLevel()==NULL)  
    activeView->showPosition(loginRoom,true);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
  {
    plugin->newMapCreated();
  }

}

/** Used to create a new room that can be undone/redone
   * @param pos The position to create the room
   * @param level The level to create the room in
   * @return A pointer to the newly created room
   */
CMapRoom *CMapManager::createRoom(QPoint pos,CMapLevel *level)
{
  CMapRoom *result = NULL;

  if (getUndoActive())
  {
    KMemConfig properties;
    KConfigGroup props = properties.group("Properties");
    props.writeEntry("Type",(int)ROOM);
    props.writeEntry("X",pos.x());
    props.writeEntry("Y",pos.y());
    props.writeEntry("Level",level->getLevelID());
    CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Room"));
    command->addElement(&properties);
    addCommand(command);
    Q3PtrList<CMapElement> *elements=command->getElements();

  
    for (CMapElement *el = elements->first(); el!=0; el = elements->next())
    {
      if (el->getElementType()==ROOM)
      {
        result = (CMapRoom *)el;
      }
    }
  }
  else
  {
    result = m_elementUtils->createRoom(pos,level);
  }

  return result;
}

/** Used to create a new zone */
CMapZone *CMapManager::createZone(QPoint pos,CMapLevel *level,bool levelCreate)
{
  // FIXME_jp : Allow this to call lowlevel mapper methods when undo is not active
  CMapZone *result = NULL;

  openCommandGroup(i18n("Create Zone"));

  KMemConfig properties;
  KConfigGroup props = properties.group("Properties");
  props.writeEntry("Type",(int)ZONE);
  props.writeEntry("X",pos.x());
  props.writeEntry("Y",pos.y());

  if (level)
  {
    props.writeEntry("Level",level->getLevelID());
  }

  CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Zone"));
  command->addElement(&properties);

  addCommand(command);  

  Q3PtrList<CMapElement> *elements=command->getElements();
  
  for (CMapElement *el = elements->first(); el!=0; el = elements->next())
  {
    if (el->getElementType()==ZONE)
    {
      result = (CMapZone *)el;
    }
  }

  if (levelCreate)
  {
    createLevel(UP,result);
  }
  closeCommandGroup();

  return result;
}

/** Used to create a new text label */
CMapText *CMapManager::createText(QPoint pos,CMapLevel *level,QString str)
{
  CMapText *result = NULL;

  if (getUndoActive())
  {
    KMemConfig properties;
    KConfigGroup props = properties.group("Properties");
    props.writeEntry("Type",(int)TEXT);
    props.writeEntry("X",pos.x());
    props.writeEntry("Y",pos.y());

    if (level)
    {
      props.writeEntry("Level",level->getLevelID());
    }
    
    props.writeEntry("Text",str);

    CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Text"));
    command->addElement(&properties);
    addCommand(command);
    Q3PtrList<CMapElement> *elements=command->getElements();
  
    for (CMapElement *el = elements->first(); el!=0; el = elements->next())
    {
      if (el->getElementType()==TEXT)
      {
        result = (CMapText *)el;
      }
    }
  }
  else
  {
    result = m_elementUtils->createText(pos,level,str);
  }

  return result;
}

/** Used to create a new text label */
CMapText *CMapManager::createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col)
{
  CMapText *result = NULL;

  if (getUndoActive())
  {
    KMemConfig properties;
    KConfigGroup props = properties.group("Properties");
    props.writeEntry("Type",(int)TEXT);
    props.writeEntry("X",pos.x());
    props.writeEntry("Y",pos.y());
    if (level)
    {
      props.writeEntry("Level",level->getLevelID());
    }

    props.writeEntry("Text",str);
    props.writeEntry("Font",font);
    props.writeEntry("Color",col);

    CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Text"));
    command->addElement(&properties);
    addCommand(command);
    Q3PtrList<CMapElement> *elements=command->getElements();
  
    for (CMapElement *el = elements->first(); el!=0; el = elements->next())
    {
      if (el->getElementType()==TEXT)
      {
        result = (CMapText *)el;
      }
    }
  }
  else
  {
    result = m_elementUtils->createText(pos,level,str,font,col);
  }

  return result;
}


/** Used to create a new path*/
CMapPath *CMapManager::createPath(QPoint srcPos,CMapLevel *srcLevel,directionTyp srcDir,
                                  QPoint destPos,CMapLevel *destLevel,directionTyp destDir)
{
  CMapRoom *room=NULL;
  CMapRoom *srcRoom=NULL;
  CMapRoom *destRoom=NULL;

  if (!srcLevel || !destLevel)
    return NULL;

  for ( room=srcLevel->getRoomList()->first(); room != 0; room=srcLevel->getRoomList()->next() )
  {
    if (room->getLowPos() == srcPos)
    {
      srcRoom = room;
      break;
    }
  }

  for ( room=destLevel->getRoomList()->first(); room != 0; room=destLevel->getRoomList()->next() )
  {
    if (room->getLowPos()  == destPos)
    {
      destRoom = room;
       break;
    }
  }

  return createPath(srcRoom,srcDir,destRoom,destDir);
}

CMapPath *CMapManager::createPath(CMapRoom *srcRoom,CMapRoom *destRoom)
{
  openCommandGroup(i18n("Create Path"));
  
  CMapPath *result = NULL;

  KMemConfig properties;
  KConfigGroup props = properties.group("Properties");
  DlgMapPathProperties d(this,props,false);

  if (d.exec())
  {
    kDebug() << "createPath 1";
      
    directionTyp srcDir = (directionTyp)props.readEntry("SrcDir",0);
    directionTyp destDir = (directionTyp)props.readEntry("DestDir",0);
    QString specialCmdSrc = props.readEntry("SpecialCmdSrc");
    QString specialCmdDest = props.readEntry("SpecialCmdDest");

    if (srcRoom->getPathDirection(srcDir,specialCmdSrc)==NULL &&
      destRoom->getPathDirection(destDir,specialCmdDest)==NULL)
    {
      kDebug() << "createPath 2";
      // create
      props.writeEntry("Type",(int)PATH);
      props.writeEntry("SrcRoom",srcRoom->getRoomID());
      props.writeEntry("SrcDir",(int)srcDir);
      props.writeEntry("SrcLevel",srcRoom->getLevel()->getLevelID());
      props.writeEntry("DestRoom",destRoom->getRoomID());
      props.writeEntry("DestDir",(int)destDir);        
      props.writeEntry("DestLevel",destRoom->getLevel()->getLevelID());
      
      if (props.hasKey("PathTwoWay"))
      {
        props.writeEntry("MakePathTwoWay","");
      }

      CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Path"));
      command->addElement(&properties);

      addCommand(command);

      command->secondStage();


      Q3PtrList<CMapElement> *elements=command->getElements();

      for (CMapElement *el = elements->first(); el!=0; el = elements->next())
      {
        if (el->getElementType()==PATH)
        {
          result = (CMapPath *)el;
        }
      }

      if (result && d.getTwoWay())
      {
        makePathTwoWay(result);

        CMapCmdElementProperties *cmd = new CMapCmdElementProperties(this,i18n("Set Path properties"),result->getOpsitePath());
        cmd->getOrgProperties().writeEntry("SrcAfterCommand","");
        cmd->getOrgProperties().writeEntry("SrcBeforeCommand","");  
        cmd->getOrgProperties().writeEntry("SpecialCmdSrc","");
        cmd->getNewProperties().writeEntry("SrcAfterCommand",props.readEntry("DestAfterCommand",""));
        cmd->getNewProperties().writeEntry("SrcBeforeCommand",props.readEntry("DestBeforeCommand",""));
        cmd->getNewProperties().writeEntry("SpecialCmdSrc",specialCmdDest);
        addCommand(cmd);
      }
  
    }
    else
    {
      KMessageBox::information (NULL,i18n("A path already exists at this location"),i18n("Kmud Mapper"));
    }
    
  }

  closeCommandGroup();
  return result;
}


/** Used to create a new path*/
CMapPath *CMapManager::createPath (CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir,bool undoable)
{
  // FIXME_jp : Allow this to call lowlevel mapper methods when undo is not active
  //            but becarefull of second stage stuff

  CMapPath *result = NULL;
  
  KMemConfig properties;
  KConfigGroup props = properties.group("Properties");
  props.writeEntry("Type",(int)PATH);
  props.writeEntry("SrcRoom",srcRoom->getRoomID());
  props.writeEntry("SrcLevel",srcRoom->getLevel()->getLevelID());
  props.writeEntry("SrcDir",(int)srcDir);
  props.writeEntry("DestRoom",destRoom->getRoomID());
  props.writeEntry("DestLevel",destRoom->getLevel()->getLevelID());
  props.writeEntry("DestDir",(int)destDir);

  CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Path"));
  command->addElement(&properties);
  bool active = getUndoActive();
  if (!undoable)
  {
    setUndoActive(false);
  }
    
  addCommand(command);

  command->secondStage();

  if (!undoable)
  {
    setUndoActive(active);
  }

  Q3PtrList<CMapElement> *elements=command->getElements();

  
  for (CMapElement *el = elements->first(); el!=0; el = elements->next())
  {
    if (el->getElementType()==PATH)
    {
      result = (CMapPath *)el;
    }
  }

  return result;
}


/** Find the first room in the map, if one can't be found then create one */
CMapRoom *CMapManager::findFirstRoom(CMapRoom *exsitingRoom)
{
  // Find the first room in the map
  for (CMapZone *zone = getMapData()->getFirstZone();zone!=0;zone = getMapData()->getNextZone())
  {
    for (CMapLevel *level = zone->getLevels()->first();level!=0;level = zone->getLevels()->next())
    {
      for (CMapRoom *room =  level->getRoomList()->first(); room!=0; room = level->getRoomList()->next())
      {
        if (room!=exsitingRoom)
        {
          return room;
        }
      }
    }
  }

  // If we get to this point then no room was found so create one
  return  createRoom(QPoint(2 * mapData->gridSize.width(),2 * mapData->gridSize.height()),getMapData()->getFirstZone()->getLevels()->first());
}

void CMapManager::deleteLevel(CMapLevel *level)
{
  //FIXME_jp: Check to see what happens when all levels are deleted.
    //          It may be nessecary to create a new one.

  openCommandGroup(i18n("Delete Level"));

  for (CMapRoom *room = level->getRoomList()->last(); room!=0; room = level->getRoomList()->last())
  {
    deleteElement(room);
  }

  for (CMapText *text = level->getTextList()->last(); text!=0; text = level->getTextList()->last())
    deleteElement(text);

  for (CMapZone *zone = level->getZoneList()->last(); zone!=0; zone = level->getZoneList()->last())
    deleteElement(zone);

  // Delete the level
  CMapCmdLevelDelete *cmd = new CMapCmdLevelDelete(this,i18n("Delete Level Actual Command"),level);
  addCommand(cmd);

  closeCommandGroup();
}


/** Check to see if a string is a valid move command
  * @param dirCmd The command that was typed
  * @return True, if a valid command otherwise false */
bool CMapManager::validMoveCmd(QString dirCmd)
{
  bool result = false;

  if (dirCmd.isEmpty()) return false;

  // check for directions
  for (uint i = 0; i < NUM_DIRECTIONS; ++i)
  {
    if (mapData->directions[i] == dirCmd)
    {
      result = true;
      break;
    }
  }
  
  if (!result)
  {
    if (currentRoom)
    {
      for (CMapPath *path=currentRoom->getPathList()->first(); path!=0; path=currentRoom->getPathList()->next())
      {
        if (path->getSpecialExit())
          if (path->getSpecialCmd()==dirCmd)
          {
            result = true;
            break;
          }
      }
    }
  }

  return result;
}

/** Used to delete a element from the map, should not be used by deleteElement as
  * it does not use groups                                                */
// FIXME_jp : This method can be removed if the deleteElement method is recursive
void CMapManager::deleteElementWithoutGroup(CMapElement *element,bool delOpsite)
{
  KMemConfig properties;

  element->saveProperties(properties.group("Properties"));

  CMapCmdElementDelete *command = new CMapCmdElementDelete(this,i18n("Delete Element"),delOpsite);

  command->addElement(&properties);
  addCommand(command);
}

/** Used to delete a element from the map */
void CMapManager::deleteElement(CMapElement *element,bool delOpsite)
{
  openCommandGroup(i18n("Delete Element"));

  // If the element is a room, then we also need to delete all it's paths
  if (element->getElementType()==ROOM)
  {
    CMapRoom *room = (CMapRoom *)element;
    if (room->getLinkedElement())  
    {
      deleteElementWithoutGroup(room->getLinkedElement(),true);
    }

    // Delete the paths for the room
    for (CMapPath *path=room->getPathList()->last(); path!=0; path=room->getPathList()->last())
      deleteElementWithoutGroup(path,false);

    // Delete any paths connecting with this room
    for (CMapPath *path=room->getConnectingPathList()->last(); path!=0; path = room->getConnectingPathList()->last())
      deleteElementWithoutGroup(path,false);
  }

  if (element->getElementType()==ZONE)
  {
    // Delete the levels in the zone
    CMapZone *zone = (CMapZone *)element;
    if (zone->getLinkedElement())  
    {
      deleteElementWithoutGroup(zone->getLinkedElement(),true);
    }

    while (zone->getLevels()->first()!=0)
    {
      deleteLevel(zone->getLevels()->first());
    }
  }

  deleteElementWithoutGroup(element,delOpsite);

  closeCommandGroup();
}

/** Used to load a map */
void CMapManager::importMap(const KUrl& url,CMapFileFilterBase *filter)
{
  setUndoActive(false);
  commandHistory->clear();
  historyGroup = NULL;

    eraseMap();
  
  // Load the map using the correct filter
  filter->loadData(url);

  if (!getLoginRoom())
  {
    CMapRoom *firstRoom = findFirstRoom(NULL);
    setLoginRoom(firstRoom);
  }

  setCurrentRoomWithoutUndo(loginRoom);

  if (loginRoom)
  {
    if (activeView->getCurrentlyViewedLevel()==NULL)  
      activeView->showPosition(loginRoom,true);
    setCurrentRoom(loginRoom);
  }

  updateZoneListCombo();
  
  setUndoActive(true);
}

/** Used to save a map */
void CMapManager::exportMap(const KUrl& url,CMapFileFilterBase *filter)
{  
  filter->saveData(url);
  commandHistory->documentSaved ();
}

/** Used to inform to change the state of the navigation tools */
void CMapManager::activeViewChanged(void)
{
  if (activeView)
  {
    CMapZone *zone = activeView->getCurrentlyViewedZone();
    if (zone)
    {
      QStringList lst = zoneMenu->items();
      zoneMenu->setCurrentItem(lst.findIndex(zone->getLabel()));
      m_toolsUpZone->setEnabled(zone->getZone());
    }

    CMapLevel *level = activeView->getCurrentlyViewedLevel();
    if (level)
    {
      m_toolsUpLevel->setEnabled(level->getNextLevel());
      m_toolsDownLevel->setEnabled(level->getPrevLevel());
    }
  }
}

void CMapManager::levelChanged(CMapLevel *level)
{
  if (level==NULL)
    return;

  activeView->changedLevel(level);
}

/** Used to inform the various parts of the mapper that a element has changed */
void CMapManager::changedElement(CMapElement *element)
{
  if (element==NULL)
    return;

  for (CMapPluginBase *plugin = getPluginList()->first();plugin!=0; plugin= getPluginList()->next())
  {
    plugin->elementChanged(element);
  }

  if (element->getElementType()==ZONE)
  {
    updateZoneListCombo();
  }
  
  activeView->changedElement(element);
}

/** Used to inform the various parts of the mapper that a element has added */
void CMapManager::addedElement(CMapElement *element)
{
  if (element->getElementType()==ZONE)
    updateZoneListCombo();

  if (activeView->getCurrentlyViewedLevel())
    activeView->addedElement(element);
}


/** Used to alter the path properties */
bool CMapManager::propertiesPath(CMapPath *path)
{
  DlgMapPathProperties d(this,path);

  return d.exec();
}

/** Used to alter the room properties */
bool CMapManager::propertiesRoom(CMapRoom *room)
{
  openCommandGroup("Change room properties");
  DlgMapRoomProperties d(this,room);

  bool b = d.exec();
  
  closeCommandGroup();
  
  return b;
}

/** Used to alter the zone properties */
bool CMapManager::propertiesZone(CMapZone *zone)
{
  openCommandGroup("Change zone properties");
  DlgMapZoneProperties d(this,zone);

  bool b = d.exec();
  
  closeCommandGroup();
  
  return b;
}

/** Used to alter the text properties */
bool CMapManager::propertiesText(CMapText *text)
{
  DlgMapTextProperties d(this,text);

  if (d.exec())
  {
    text->updateLinkElements();
    return true;
  }

  return false;
}

void CMapManager::setDefaultOptions()
{
  cGlobalSettings *gs = cGlobalSettings::self();
  // directions
  gs->setDefaultString ("mapper-direction-north", "north");
  gs->setDefaultString ("mapper-direction-northeast", "northeast");
  gs->setDefaultString ("mapper-direction-east", "east");
  gs->setDefaultString ("mapper-direction-southeast", "southeast");
  gs->setDefaultString ("mapper-direction-south", "south");
  gs->setDefaultString ("mapper-direction-southeast", "southwest");
  gs->setDefaultString ("mapper-direction-west", "west");
  gs->setDefaultString ("mapper-direction-northwest", "northwest");
  gs->setDefaultString ("mapper-direction-up", "up");
  gs->setDefaultString ("mapper-direction-down", "down");
  gs->setDefaultString ("mapper-direction-n", "n");
  gs->setDefaultString ("mapper-direction-ne", "ne");
  gs->setDefaultString ("mapper-direction-e", "e");
  gs->setDefaultString ("mapper-direction-se", "se");
  gs->setDefaultString ("mapper-direction-s", "s");
  gs->setDefaultString ("mapper-direction-sw", "sw");
  gs->setDefaultString ("mapper-direction-w", "w");
  gs->setDefaultString ("mapper-direction-nw", "nw");
  gs->setDefaultString ("mapper-direction-u", "u");
  gs->setDefaultString ("mapper-direction-d", "d");

  // move check
  gs->setDefaultBool ("mapper-movement-validcheck", false);

  gs->setDefaultColor ("mapper-color-Background",QColor(201,167,55));
  gs->setDefaultColor ("mapper-color-Grid",QColor(213,178,88));
  gs->setDefaultColor ("mapper-color-LowerRoom", Qt::darkGray);
  gs->setDefaultColor ("mapper-color-LowerZone", Qt::darkGray);
  gs->setDefaultColor ("mapper-color-LowerText", Qt::darkGray);
  gs->setDefaultColor ("mapper-color-DefaultRoom", QColor(192,192,192));
  gs->setDefaultColor ("mapper-color-DefaultZone", QColor(192,192,192));
  gs->setDefaultColor ("mapper-color-HigherRoom", Qt::white);
  gs->setDefaultColor ("mapper-color-HigherZone", Qt::white);
  gs->setDefaultColor ("mapper-color-HigherText", Qt::white);
  gs->setDefaultColor ("mapper-color-LowerPath", Qt::darkGray);
  gs->setDefaultColor ("mapper-color-DefaultPath", Qt::black);
  gs->setDefaultColor ("mapper-color-HigherPath", Qt::white);
  gs->setDefaultColor ("mapper-color-DefaultText", Qt::black);
  gs->setDefaultColor ("mapper-color-Selected", Qt::blue);
  gs->setDefaultColor ("mapper-color-Special", Qt::yellow);
  gs->setDefaultColor ("mapper-color-Login", Qt::blue);
  gs->setDefaultColor ("mapper-color-Edit", Qt::red);
  gs->setDefaultColor ("mapper-color-Current", Qt::red);

  // read speedwalk options
  gs->setDefaultBool ("mapper-speedwalk-abort-active", true);
  gs->setDefaultInt ("mapper-speedwalk-abort-limit", 100);
  gs->setDefaultInt ("mapper-speedwalk-delay", 0);


}

/** Used to read the map options */
void CMapManager::readOptions()
{  
  cGlobalSettings *gs = cGlobalSettings::self();

  // Read directions
  getMapData()->directions[NORTH] = gs->getString ("mapper-direction-north");
  getMapData()->directions[NORTHEAST] = gs->getString ("mapper-direction-northeast");
  getMapData()->directions[EAST] = gs->getString ("mapper-direction-east");
  getMapData()->directions[SOUTHEAST] = gs->getString ("mapper-direction-southeast");
  getMapData()->directions[SOUTH] = gs->getString ("mapper-direction-south");
  getMapData()->directions[SOUTHWEST] = gs->getString ("mapper-direction-southeast");
  getMapData()->directions[WEST] = gs->getString ("mapper-direction-west");
  getMapData()->directions[NORTHWEST] = gs->getString ("mapper-direction-northwest");
  getMapData()->directions[UP] = gs->getString ("mapper-direction-up");
  getMapData()->directions[DOWN] = gs->getString ("mapper-direction-down");

  int half = NUM_DIRECTIONS/2;
  getMapData()->directions[NORTH+half] = gs->getString ("mapper-direction-n");
  getMapData()->directions[NORTHEAST+half] = gs->getString ("mapper-direction-ne");
  getMapData()->directions[EAST+half] = gs->getString ("mapper-direction-e");
  getMapData()->directions[SOUTHEAST+half] = gs->getString ("mapper-direction-se");
  getMapData()->directions[SOUTH+half] = gs->getString ("mapper-direction-s");
  getMapData()->directions[SOUTHWEST+half] = gs->getString ("mapper-direction-sw");
  getMapData()->directions[WEST+half] = gs->getString ("mapper-direction-w");
  getMapData()->directions[NORTHWEST+half] = gs->getString ("mapper-direction-nw");
  getMapData()->directions[UP+half] = gs->getString ("mapper-direction-u");
  getMapData()->directions[DOWN+half] = gs->getString ("mapper-direction-d");

  // Read movecheck config
  getMapData()->validRoomCheck = gs->getBool ("mapper-movement-validcheck");
  int count = gs->getInt ("mapper-movement-check-count");
  getMapData()->failedMoveMsg.clear();
  for (int i = 1; i <= count; ++i)
    getMapData()->failedMoveMsg << gs->getString ("mapper-movement-check-" + QString::number (i));

  // Read map colors
  mapData->backgroundColor=gs->getColor ("mapper-color-Background");
  mapData->gridColor=gs->getColor ("mapper-color-Grid");
  mapData->lowerRoomColor=gs->getColor ("mapper-color-LowerRoom");
  mapData->lowerZoneColor=gs->getColor ("mapper-color-LowerZone");
  mapData->lowerTextColor=gs->getColor ("mapper-color-LowerText");
  mapData->defaultRoomColor=gs->getColor ("mapper-color-DefaultRoom");
  mapData->defaultZoneColor=gs->getColor ("mapper-color-DefaultZone");
  mapData->higherRoomColor=gs->getColor ("mapper-color-HigherRoom");
  mapData->higherZoneColor=gs->getColor ("mapper-color-HigherZone");
  mapData->higherTextColor=gs->getColor ("mapper-color-HigherText");
  mapData->lowerPathColor=gs->getColor ("mapper-color-LowerPath");
  mapData->defaultPathColor=gs->getColor ("mapper-color-DefaultPath");
  mapData->higherPathColor=gs->getColor ("mapper-color-HigherPath");
  mapData->defaultTextColor=gs->getColor ("mapper-color-DefaultText");
  mapData->selectedColor=gs->getColor ("mapper-color-Selected");
  mapData->specialColor = gs->getColor ("mapper-color-Special");
  mapData->loginColor=gs->getColor ("mapper-color-Login");
  mapData->editColor=gs->getColor ("mapper-color-Edit");
  mapData->currentColor=gs->getColor ("mapper-color-Current");

  // read speedwalk options
  mapData->speedwalkAbortActive = gs->getBool ("mapper-speedwalk-abort-active");
  mapData->speedwalkAbortLimit = gs->getInt ("mapper-speedwalk-abort-limit");
  mapData->speedwalkDelay = gs->getInt ("mapper-speedwalk-delay");

  // Set menu options
  m_toolsGrid->setChecked(mapData->gridVisable);
  m_toolsCreate->setChecked(mapData->createModeActive);
  m_viewLowerLevel->setChecked(mapData->showLowerLevel);
  m_viewUpperLevel->setChecked(mapData->showUpperLevel);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
  {
    plugin->loadConfigOptions();
  }
}

/** Used to write the map options */
void CMapManager::saveGlobalConfig()
{
  cGlobalSettings *gs = cGlobalSettings::self();

  // directions
  gs->setString ("mapper-direction-north", getMapData()->directions[NORTH]);
  gs->setString ("mapper-direction-northeast", getMapData()->directions[NORTHEAST]);
  gs->setString ("mapper-direction-east", getMapData()->directions[EAST]);
  gs->setString ("mapper-direction-southeast", getMapData()->directions[SOUTHEAST]);
  gs->setString ("mapper-direction-south", getMapData()->directions[SOUTH]);
  gs->setString ("mapper-direction-southeast", getMapData()->directions[SOUTHWEST]);
  gs->setString ("mapper-direction-west", getMapData()->directions[WEST]);
  gs->setString ("mapper-direction-northwest", getMapData()->directions[NORTHWEST]);
  gs->setString ("mapper-direction-up", getMapData()->directions[UP]);
  gs->setString ("mapper-direction-down", getMapData()->directions[DOWN]);

  int half = NUM_DIRECTIONS/2;
  gs->setString ("mapper-direction-n", getMapData()->directions[NORTH+half]);
  gs->setString ("mapper-direction-ne", getMapData()->directions[NORTHEAST+half]);
  gs->setString ("mapper-direction-e", getMapData()->directions[EAST+half]);
  gs->setString ("mapper-direction-se", getMapData()->directions[SOUTHEAST+half]);
  gs->setString ("mapper-direction-s", getMapData()->directions[SOUTH+half]);
  gs->setString ("mapper-direction-sw", getMapData()->directions[SOUTHWEST+half]);
  gs->setString ("mapper-direction-w", getMapData()->directions[WEST+half]);
  gs->setString ("mapper-direction-nw", getMapData()->directions[NORTHWEST+half]);
  gs->setString ("mapper-direction-u", getMapData()->directions[UP+half]);
  gs->setString ("mapper-direction-d", getMapData()->directions[DOWN+half]);

  // movement
  gs->setBool ("mapper-movement-validcheck", getMapData()->validRoomCheck);
  gs->setInt ("mapper-movement-check-count", getMapData()->failedMoveMsg.size());
  int idx = 0;
  QStringList::iterator it;
  for (it = getMapData()->failedMoveMsg.begin(); it != getMapData()->failedMoveMsg.end(); ++it)
    gs->setString ("mapper-movement-check-" + QString::number (++idx), *it);

  gs->setColor ("mapper-color-Background", mapData->backgroundColor);
  gs->setColor ("mapper-color-Grid", mapData->gridColor);
  gs->setColor ("mapper-color-LowerRoom", mapData->lowerRoomColor);
  gs->setColor ("mapper-color-LowerZone", mapData->lowerZoneColor);
  gs->setColor ("mapper-color-LowerText", mapData->lowerTextColor);
  gs->setColor ("mapper-color-HigherZone", mapData->higherZoneColor);
  gs->setColor ("mapper-color-DefaultRoom", mapData->defaultRoomColor);
  gs->setColor ("mapper-color-DefaultZone", mapData->defaultZoneColor);
  gs->setColor ("mapper-color-DefaultText", mapData->defaultTextColor);
  gs->setColor ("mapper-color-HigherRoom", mapData->higherRoomColor);
  gs->setColor ("mapper-color-HigherText", mapData->higherTextColor);
  gs->setColor ("mapper-color-LowerPath", mapData->lowerPathColor);
  gs->setColor ("mapper-color-DefaultPath",mapData->defaultPathColor);
  gs->setColor ("mapper-color-HigherPath", mapData->higherPathColor);
  gs->setColor ("mapper-color-Selected", mapData->selectedColor);
  gs->setColor ("mapper-color-Special", mapData->specialColor);
  gs->setColor ("mapper-color-Login", mapData->loginColor);
  gs->setColor ("mapper-color-Edit", mapData->editColor);
  gs->setColor ("mapper-color-Current", mapData->currentColor);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
  {
    plugin->saveConfigOptions();
  }

  gs->setBool ("mapper-speedwalk-abort-active", mapData->speedwalkAbortActive);
  gs->setInt ("mapper-speedwalk-abort-limit", mapData->speedwalkAbortLimit);
  gs->setInt ("mapper-speedwalk-delay", mapData->speedwalkDelay);

  redrawAllViews();
}

void CMapManager::getCounts(int *levels,int *rooms,int *paths,int *labels,int *zones)
{
  *levels = 0;
  *rooms = 0;
  *labels = 0;
  *paths = 0;
  *zones = 0;

  for (CMapZone *zone = mapData->getFirstZone(); zone !=0 ; zone = mapData->getNextZone())
  {
    for (CMapLevel *level = zone->getLevels()->first(); level !=0; level = zone->getLevels()->next())
    {
      for (CMapRoom *room = level->getRoomList()->first(); room!=0; room = level->getRoomList()->next())
      {
        *paths += room->getPathList()->count();
      }

      *rooms += level->getRoomList()->count();
      *labels += level->getTextList()->count();
      *zones += level->getZoneList()->count();
    }

    *levels += zone->getLevels()->count();
  }
}

directionTyp CMapManager::getOpsiteDirection(directionTyp dir)
{
  directionTyp result = SOUTH;
  switch (dir)
  {
    case NORTH     : result = SOUTH; break;
    case SOUTH     : result = NORTH; break;
    case EAST      : result = WEST; break;
    case WEST      : result = EAST; break;
    case NORTHWEST : result = SOUTHEAST; break;
    case NORTHEAST : result = SOUTHWEST; break;
    case SOUTHWEST : result = NORTHEAST; break;
    case SOUTHEAST : result = NORTHWEST; break;
    case UP        : result = DOWN; break;
    case DOWN      : result = UP; break;
    case SPECIAL   : result = SPECIAL; break;
  }

  return result;
}

/** Used to set the current tool */
void CMapManager::setCurrentTool(CMapToolBase *tool)
{
  if (currentTool)
    currentTool->toolUnselected();

  currentTool=tool;

  if (currentTool)
    currentTool->toolSelected();
}

/** Usd to get the current tool */
CMapToolBase *CMapManager::getCurrentTool(void)
{
  return currentTool;
}

/** This method puts a element into edit state */
void CMapManager::setEditElement(CMapElement *element)
{
  if (elementEdit)
  {
    elementEdit->setEditMode(false);
    changedElement(elementEdit);
  }

  element->setEditMode(true);
  elementEdit = element;
  changedElement(elementEdit);
}

/** This is used to remove edit state from element being edited */
void CMapManager::unsetEditElement(void)
{
  if (elementEdit)
  {
    elementEdit->setEditMode(false);
    changedElement(elementEdit);
  }
}

/** This gets the element that is being edited */
CMapElement *CMapManager::getEditElement(void)
{
  return elementEdit;
}

/** This cancels any editing */
void CMapManager::stopEditing(void)
{
  if (elementEdit)
  {
    elementEdit->setEditMode(false);
  }

  elementEdit = NULL;
}

int CMapManager::getUndoActive(void)
{
  return m_commandsActive;
}

/** This is used to tell the mapper if commands should be added to the history list */
void CMapManager::setUndoActive(bool active)
{
  m_commandsActive = active;
}

/** Used to add a command to the command history */
void CMapManager::addCommand(K3Command *command,bool execute)
{
  if (m_commandsActive)
  {
    if (historyGroup)
    {
      historyGroup->addCommand(command,execute);
    }
    else
    {
      commandHistory->addCommand(command,execute);
    }
  }
  else
  {
    command->execute();
  }
}

void CMapManager::openCommandGroup(QString name)
{
  CMapCmdGroup *group = new CMapCmdGroup(this,name);
  group->setPreviousGroup(historyGroup);
  historyGroup = group;
}

void CMapManager::closeCommandGroup(void)
{
  CMapCmdGroup *currentGroup = historyGroup;
  CMapCmdGroup *oldGroup =historyGroup->getPreviousGroup();
  historyGroup = oldGroup;
  addCommand(currentGroup,false);
}

/** move the player relative to the current position of the player
  * @param cmd The move command used to move the player */
void CMapManager::movePlayerBy(QString cmd)
{
  QString specialCmd = "";
  directionTyp dir  = textToDirection(cmd);

  if (dir == SPECIAL)
    specialCmd = cmd;

  movePlayerBy(dir,mapData->createModeActive,specialCmd);
}

/** move the player relative to the current position of the player
  * This command has support for special paths, but can only move
  * anlong exsiting special paths. In other words, it is unable to
  * create special paths.
  */
void CMapManager::movePlayerBy(directionTyp dir,bool create,QString specialCmd)
{
  openCommandGroup(i18n("Move Player"));
  CMapLevel *destLevel;

  // Make sure that the room we are currently in is visible
  CMapRoom *tmpRoom = currentRoom;

  if (activeView->getCurrentlyViewedLevel() != currentRoom->getLevel() && activeView->getFollowMode())
  activeView->showPosition(currentRoom->getLowPos(),currentRoom->getLevel());

  currentRoom = tmpRoom;
  // Find the destination of the path that was traveled and if no path
  // exists for the given direction create the room and path if necessarry
  CMapPath *path = currentRoom->getPathDirection(dir,specialCmd);

    if (path)
  {
    setCurrentRoom(path->getDestRoom());
    activeView->showPosition(currentRoom->getLowPos(),currentRoom->getLevel());
  }
  else
  {
    CMapRoom *srcRoom = currentRoom;
    CMapPath *opsitePath = NULL;
    int x=0,y=0;
    bool bFound = false;

    destLevel = currentRoom->getLevel();

    // Check to see if there is a path in the opsite direction that we should
    // be using
    for (CMapPath *path2=srcRoom->getConnectingPathList()->first();path2!=0;path2=srcRoom->getConnectingPathList()->next())
    {
      if (path2->getDestDir() == dir)
      {
        x = path2->getSrcRoom()->getX();
        y = path2->getSrcRoom()->getY();
        opsitePath = path2;
        bFound = true;
        break;
      }
    }

    if (!opsitePath)
    {
      // No opsite path found so we need to create a path    
      QPoint inc;
      directionToCord(dir,QSize(getMapData()->gridSize.width()*2,getMapData()->gridSize.height()*2),&inc);
      x = currentRoom->getX()+inc.x();
      y = currentRoom->getY()+inc.y();

      destLevel = currentRoom->getLevel();

      if (dir == UP)
      {
        if (currentRoom->getLevel()->getNextLevel())
        {
          destLevel = currentRoom->getLevel()->getNextLevel();
        }
        else
        {
          destLevel = createLevel(UP,currentRoom->getZone());
        }

      }

      if (dir == DOWN)
      {
        if (currentRoom->getLevel()->getPrevLevel())
        {
          destLevel = currentRoom->getLevel()->getPrevLevel();
        }
        else
        {
          destLevel = createLevel(DOWN,currentRoom->getZone());
        }
      }

      // Check to see if the map needs to be moved
      // and calulate the offset to move if it needs moving.
      if (x<getMapData()->gridSize.width()*3 || y<getMapData()->gridSize.height()*2)
      {
        int movex,movey;

        if (x<getMapData()->gridSize.width()*3)
        {
          movex = getMapData()->gridSize.width()*3 - x;
          x+=movex;
        }
        else
          movex = 0;

        if (y<getMapData()->gridSize.height()*3)
        {
          movey = getMapData()->gridSize.height()*3- y;
          y+=movey;
        }
        else
          movey = 0;

        moveMap (QPoint(movex,movey),currentRoom->getZone());
      }
    }

    // Check to see if the room already exists
    CMapElement *elm = findElementAt(QPoint (x,y),destLevel);
    CMapRoom *newCurrentRoom = NULL;


    // FIXME_jp : Tidy this up so that the returns are not needed.
    if (!elm)
    {
      newCurrentRoom = NULL;
    }
    else if (elm->getElementType()==ZONE)
    {
      // Found a zone were we want to create a room so exit method
      closeCommandGroup();
      return;
    }
    else
    {
      newCurrentRoom = (CMapRoom *)elm;
    }

    // Create the room if it does not exsit
    if (!newCurrentRoom)
    {
      if (create)
      {
        newCurrentRoom=createRoom(QPoint (x,y),destLevel);
        directionTyp destDir = getOpsiteDirection(dir);

        // Create the new path to the room
        CMapPath *newPath = createPath(srcRoom,dir,newCurrentRoom,destDir);

        // Make the path two way if the default path type is two way
        if (getMapData()->defaultPathTwoWay && bFound == false)
        {
          makePathTwoWay(newPath);
        }
        setCurrentRoom(newCurrentRoom);
      }
      else
      {
        closeCommandGroup();
        return;
      }
    }
  }

  closeCommandGroup();
}

/** Used to walk the player in the mud to a given room */
void CMapManager::walkPlayerTo(CMapRoom *toRoom)
{
  Q3PtrQueue<CMapRoom> roomsToVisit;
  CMapRoom *destRoom;
  CMapRoom *srcRoom;
  CMapPath *path;
  CMapRoom *foundRoom;
  signed int time = 0;
  bool bFound = false;
  CMapPath *foundPath = NULL;

  int speedWalkAbortCount = 0;

  if (currentRoom == toRoom) return;

  if (speedwalkActive)
  {
    KMessageBox::information (NULL,i18n("Speedwalking is already in progress"),i18n("Kmud Mapper"));
    return;
  }

  speedwalkActive = true;

  pathToWalk.clear();

  // Reset the seach count for all the rooms
  for (CMapZone *zone = mapData->getFirstZone(); zone!=0; zone = mapData->getNextZone())
  {
    for (CMapLevel *level=zone->getLevels()->first(); level !=0;level = zone->getLevels()->next())
    {
      for (CMapRoom *room=level->getRoomList()->first();room !=0 ; room = level->getRoomList()->next())
      {
        room->setMoveTime(-1);
      }
    }
  }

  // Init things for the start room
  srcRoom = currentRoom;
  destRoom = (CMapRoom *)toRoom;
  srcRoom->setMoveTime(time++);

  // enqueue starting room
  roomsToVisit.enqueue(srcRoom);
  CMapRoom* room;

  // this is the Breadth First Search Algorithm
  while (!(roomsToVisit.isEmpty() || bFound))
  {
    foundRoom = roomsToVisit.dequeue();

    // for all neighbours of foundRoom
    for (path=foundRoom->getPathList()->first(); path != 0; path=foundRoom->getPathList()->next())
    {
      room = path->getDestRoom();

      if (room == destRoom)
      {
        bFound = true;
        break;
      }

      // neighbour has not been visited yet, enqueue it
      if (room->getMoveTime() == -1)
      {
        room->setMoveTime(time++);
        roomsToVisit.enqueue(room);
      }
    }
  }

  // Check to see if we were unable to find any paths
  if (!bFound)
  {
    roomsToVisit.clear();
    KMessageBox::information (NULL,i18n("The automapper was unable to find a path to requested room"),i18n("Kmud Mapper"));

    return;
  }

  speedWalkAbortCount=0;
  
  // Trace steps that need to be taken backwards from the dest to the src room
  while(destRoom != srcRoom)
  {
    time = destRoom->getConnectingPathList()->first()->getSrcRoom()->getMoveTime();
    foundRoom = destRoom->getConnectingPathList()->first()->getSrcRoom();

    // Find the room with the shortest time as this is the room we
    // should be moving to.
    for (path=destRoom->getConnectingPathList()->first();path!=0; path=destRoom->getConnectingPathList()->next())
    {
      if (time == -1 || (path->getSrcRoom()->getMoveTime()<=time && path->getSrcRoom()->getMoveTime()!=-1))
      {
        time = path->getSrcRoom()->getMoveTime();
        foundRoom = path->getSrcRoom();
        foundPath = path;
      }
    }

    pathToWalk.push(new QString(directionToText(foundPath->getSrcDir(),foundPath->getSpecialCmd())));

    destRoom=foundRoom;
    // Check to make sure that tings are not stuck in a loop and abort
    // if the move limit is reached

    speedWalkAbortCount++;
    if (mapData->speedwalkAbortActive && (speedWalkAbortCount == mapData->speedwalkAbortLimit))
    {
      KMessageBox::information (NULL,i18n("Speedwalk abort because move limit was reached"),i18n("Kmud Mapper"));

      return;
    }
  }

  speedwalkProgressDlg->setTotalSteps(pathToWalk.count());
  speedwalkProgressDlg->setProgress(0);

  cActionManager *am = cActionManager::self();
  cStatus *status = dynamic_cast<cStatus *>(am->object ("status", am->activeSession()));
  if (status)
    status->statusBar()->addWidget(speedwalkProgressDlg,0,true);
  speedwalkProgressDlg->show();
  speedwalkProgress = 0;

  // Start walking path
  slotWalkPlayerAlongPath();

  // Tidy up
  roomsToVisit.clear();
}

void CMapManager::slotAbortSpeedwalk(void)
{
  pathToWalk.clear();
  speedwalkActive = false;
  speedwalkProgressDlg->setProgress(speedwalkProgressDlg->getTotalSteps());
  speedwalkProgressDlg->hide();
  cActionManager *am = cActionManager::self();
  cStatus *status = dynamic_cast<cStatus *>(am->object ("status", am->activeSession()));
  if (status)
    status->statusBar()->removeWidget(speedwalkProgressDlg);
}

/** Used to recersivly move the play along a speedwalk path */
void CMapManager::slotWalkPlayerAlongPath(void)
{
  if (speedwalkActive)
  {
    QString *dir = pathToWalk.pop();

    // TODO: using active session isn't a very good idea; progress bar should be shown on the mapper window, not in KMuddy's status bar; furthermore, the mapper should distinguish sessions and switch maps when session changes or something - until all this gets done, we cannot implement this any better
    mapperPlugin->sendCommand (mapperPlugin->activeSession(), *dir);
    speedwalkProgressDlg->setProgress(++speedwalkProgress);

    // Walk the path
    if (!pathToWalk.isEmpty())
    {
      QTimer::singleShot( mapData->speedwalkDelay * 100, this, SLOT(slotWalkPlayerAlongPath()) );
    }
    else
    {
      slotAbortSpeedwalk();
    }
  }
}

/** This method is used to convert a direction into a offset */
void CMapManager::directionToCord(directionTyp dir, QSize distance,QPoint *pos)
{
  int x = pos->x();
  int y = pos->y();

  switch (dir)
  {
    case NORTH     : x = 0;
                     y = -distance.height();
                     break;
    case EAST      : x = distance.width();
                     y = 0;
                     break;
    case SOUTH     : x = 0;
                     y = distance.height();
                     break;
    case WEST      : x = -distance.width();
                     y = 0;
                         break;
    case NORTHEAST : x = distance.width();
                     y = -distance.height();
                     break;
    case NORTHWEST : x = -distance.width();
                     y = -distance.height();
                     break;
    case SOUTHEAST : x = distance.width();
                     y = distance.height();
                     break;
    case SOUTHWEST : x = -distance.width();
                     y = distance.height();
                     break;
    case UP        : break;
    case DOWN      : break;
    case SPECIAL   : break;
  }

  pos->setX(x);
  pos->setY(y);
}

/**
 * This method is used to set the element when that a popup menu is to be showed for
 */
void CMapManager::setSelectedElement(CMapElement *element)
{
  m_selectedElement = element;  
}

/**
 * This methid is used to set the position that a element was selected when the contex menu is shown
 */
void CMapManager::setSelectedPos(QPoint pos)
{
  m_selectedPos = pos;
}

/**
 * This method is used to get the element that a popup menu is being displayed for
 */
CMapElement *CMapManager::getSelectedElement(void)
{
  return m_selectedElement;
}

/** This method is used to move the elements in a zone by the given vector */
void CMapManager::moveMap(QPoint inc,CMapZone *zone)
{
  CMapCmdMoveMap *cmd = new CMapCmdMoveMap(this,inc,zone,i18n("Move Elements in map"));
  addCommand(cmd);
}

/** This method is used to make a path two way */
void CMapManager::makePathTwoWay(CMapPath *path)
{
  if (!path->getOpsitePath())
  {
    CMapCmdTogglePathTwoWay *cmd =  new CMapCmdTogglePathTwoWay(this,i18n("Make Path Two-Way"),path);
    addCommand(cmd);
  }
}

/** This method is used to make a path one way */
void CMapManager::makePathOneWay(CMapPath *path)
{
  if (path->getOpsitePath())
  {
    CMapCmdTogglePathTwoWay *cmd =  new CMapCmdTogglePathTwoWay(this,i18n("Make Path One-Way"),path);
    addCommand(cmd);
  }
}

/** Get the level that has focues at the moment */
CMapViewBase *CMapManager::getActiveView(void)
{  
  return activeView;
}                                                                    

/** Used to repaint all the views */
void CMapManager::redrawAllViews(void)
{
  activeView->redraw();
}

/** This method is called to create a new map, when the new map menu option is selected */
void CMapManager::slotFileNew()
{
  if (KMessageBox::warningYesNo (NULL,i18n("Are you sure you want to create a new map?\nThis action can not be undone"),i18n("Kmud Mapper"))== KMessageBox::Yes)
  {
    setUndoActive(false);
    commandHistory->clear();
    historyGroup = NULL;
    eraseMap();
    createNewMap();
    setUndoActive(true);
  }
}

/** This method is called to load a map from a file when the load menu option is selected */
void CMapManager::slotFileLoad()
{
  int found = 0;
  QString filterStr = "";

  for (CMapFileFilterBase *filter = m_fileFilter.first();filter!=0;filter=m_fileFilter.next())
  {
    if (filter->supportLoad())
    {
         filterStr = filterStr + filter->getPatternExtension() + "|"+filter->getName();
      filterStr += "\n";
      found++;
    }
  }

  if (found>0)
  {
    filterStr = filterStr.remove(filterStr.length()-1,1);

    KFileDialog dlg(KUrl(":"), filterStr, 0);
      dlg.setOperationMode( KFileDialog::Opening );

    dlg.setCaption(i18n("Load Map File"));
    dlg.setMode( KFile::File );
    if (dlg.exec())
    {

      for (CMapFileFilterBase *filter = m_fileFilter.first();filter!=0;filter=m_fileFilter.next())
      {
        if (filter->getPatternExtension()==dlg.currentFilter() && filter->supportLoad())
        {
          importMap(dlg.selectedUrl(),filter);
          break;
        }
      }
    }
  }
}

/** This method is called to save the map to a file when the "save as" menu option is selected */
void CMapManager::slotFileSave()
{
  int found = 0;
  QString filterStr = "";
  
  for (CMapFileFilterBase *filter = m_fileFilter.first();filter!=0;filter=m_fileFilter.next())
  {
    if (filter->supportSave())
    {
      filterStr = filterStr + filter->getPatternExtension() + "|"+filter->getName();
      filterStr += "\n";
      found++;
    }
  }

  if (found>0)
  {
    filterStr = filterStr.remove(filterStr.length()-1,1);    
    KFileDialog dlg(KUrl(":"), filterStr,0);
      dlg.setOperationMode( KFileDialog::Saving );

    dlg.setCaption(i18n("Save Map File"));
    dlg.setMode( KFile::File );
    if (dlg.exec())
    {

      for (CMapFileFilterBase *filter = m_fileFilter.first();filter!=0;filter=m_fileFilter.next())
      {  
        if (filter->getPatternExtension()==dlg.currentFilter())
        {
          KUrl url = dlg.selectedUrl();
          if (!url.path().endsWith(filter->getExtension()))
          {
            url.setPath(url.path()+filter->getExtension());
          }
          exportMap(url,filter);
        
          break;
        }
      }
    }
  }
}

QString CMapManager::defaultSavePath () const
{
  return KStandardDirs::locateLocal ("appdata", "maps/");
}

/** This method is used to save the map to the default location */
void CMapManager::loadMap(void)
{
  QString mapDir = defaultSavePath();

  CMapFileFilterBase *filter = m_fileFilter.first();

  QString filename = mapDir + "/" + "map" + filter->getExtension();
  QFile file(filename);

  if (file.exists())
  {
    kDebug() << "Loading map.....";
    KUrl url;
    url.setPath(filename);
    importMap(url,filter);
  }
  else
  {
    kDebug() << "Creating new map....";
    createNewMap();
  }
}

/** This method is used to load the map from the default location */
void CMapManager::saveMap(void)
{
  QString mapDir = defaultSavePath();
  CMapFileFilterBase *filter = m_fileFilter.first();

  KUrl url;
  url.setPath(mapDir + "/" + "map"+ filter->getExtension());
  exportMap(url,filter);
}

/** This methid is called to display map information when the information menu option is selected */
void CMapManager::slotFileInfo()
{
  DlgMapInfo d(this,NULL,"infoDialog");
  
  d.exec();
}

void CMapManager::slotToolsGrid()
{
  getMapData()->gridVisable = m_toolsGrid->isChecked();
  redrawAllViews();
}


void CMapManager::slotToolsLevelUp()
{
  CMapLevel *level = getActiveView()->getCurrentlyViewedLevel()->getNextLevel();
  if (level)
  {
    getActiveView()->showPosition(level,false);
    getActiveView()->redraw();
  }
}

void CMapManager::slotToolsLevelDown()
{
  CMapLevel *level = getActiveView()->getCurrentlyViewedLevel()->getPrevLevel();
  if (level)
  {
    getActiveView()->showPosition(level,false);
    getActiveView()->redraw();
  }
}

void CMapManager::slotToolsLevelDelete()
{
  CMapLevel *level = getActiveView()->getCurrentlyViewedLevel();

  if (level)
    deleteLevel(level);
}

void CMapManager::slotToolsZoneUp()
{
  CMapZone *zone = getActiveView()->getCurrentlyViewedZone();
  if (zone)
  {
    CMapLevel *level = zone->getLevel();
    if (level)
    {
      getActiveView()->showPosition(zone->getLowPos(),level);
      getActiveView()->redraw();
    }

  }
}

void CMapManager::slotToolsDeleteZone()
{
  CMapZone *zone = getActiveView()->getCurrentlyViewedZone();
  if (zone)
  {
    deleteElement(zone);
  }
}

void CMapManager::slotToolsCreateMode()
{
  getMapData()->createModeActive = m_toolsCreate->isChecked();
}

void CMapManager::slotToolsZoneProperties()
{  
  CMapZone *zone = getActiveView()->getCurrentlyViewedZone();
  if (zone)
  {
    propertiesZone(zone);
  }
}

void CMapManager::slotSelectZone()
{
  for (CMapZone *zone = mapData->getFirstZone();zone!=0; zone = mapData->getNextZone())
  {
    if (zone->getLabel()==zoneMenu->currentText())
    {
      getActiveView()->showPosition(zone->getLevels()->first());
      getActiveView()->redraw();
      break;
    }
  }  
}

void CMapManager::slotViewUpperLevel()
{
  mapData->showUpperLevel = m_viewUpperLevel->isChecked();
  redrawAllViews();
}

void CMapManager::slotViewLowerLevel()
{
  mapData->showLowerLevel = m_viewLowerLevel->isChecked();
  redrawAllViews();
}

void CMapManager::slotViewToolsToolbar()
{
  QWidget* toolBar = factory()->container("tools", this);
  if (toolBar == 0 || !toolBar->inherits("QToolBar"))
    return;

  if(!m_viewToolsToolbar->isChecked())
  {
    toolBar->hide();
  }
  else
  {
    toolBar->show();
  }
}

void CMapManager::slotViewNavToolbar()
{
  QWidget* toolBar = factory()->container("navigation", this);
  if (toolBar == 0 || !toolBar->inherits("QToolBar"))
    return;

  if(!m_viewNavToolbar->isChecked())
  {
    toolBar->hide();
  }
  else
  {
    toolBar->show();
  }
}

bool CMapManager::queryClose()
{
  // TODO: ask if really
  emit closed();
  return true;
}

/** Used to room under the point the current room */
void CMapManager::slotRoomSetCurrentPos(void)
{
  setCurrentRoom((CMapRoom *)m_selectedElement);
}

/** Used to room under the point the login room */
void CMapManager::slotRoomSetLogin(void)
{
  setLoginRoom((CMapRoom *)m_selectedElement);
}

/** Used to set speedwalk to the room under the pointer */
void CMapManager::slotRoomSpeedwalkTo(void)
{
  walkPlayerTo((CMapRoom *)m_selectedElement);
}

/** Used to delete the room under the pointer */
void CMapManager::slotRoomDelete(void)
{
  deleteElement(m_selectedElement);
}

/** Used to display the properties of the room under the pointer */
void CMapManager::slotRoomProperties(void)
{
  propertiesRoom((CMapRoom *)m_selectedElement);
}

/** Used to make the path under the pointer one way */
void CMapManager::slotPathOneWay(void)
{
  makePathOneWay((CMapPath *)m_selectedElement);
}

/** Used to make the the path under the pointer two way */
void CMapManager::slotPathTwoWay(void)
{
  makePathTwoWay((CMapPath *)m_selectedElement);
}

/** Used to add a bend to the path under the pointer */
void CMapManager::slotPathAddBend(void)
{
  kDebug() << "CMapManager::CMapManager slotPathAddBend";
  openCommandGroup(i18n("Add Bend"));
  CMapPath *path = (CMapPath *)m_selectedElement;

  path->addBendWithUndo(m_selectedPos);
  if (path->getOpsitePath())
  {
    path->getOpsitePath()->addBendWithUndo(m_selectedPos);
  }
  m_clipboard->slotUnselectAll();
  path->setEditMode(true);
  changedElement(path);

  closeCommandGroup();
}

/** Used to delete the path segment under the pointer */
void CMapManager::slotPathDelBend(void)
{
  openCommandGroup(i18n("Delete Path Segment"));
  CMapPath *path = (CMapPath *)m_selectedElement;

  int seg = path->mouseInPathSeg(m_selectedPos,getActiveView()->getCurrentlyViewedZone());

  path->deletePathSegWithUndo(seg);
  if (path->getOpsitePath())
  {
    int seg = path->getOpsitePath()->mouseInPathSeg(m_selectedPos,getActiveView()->getCurrentlyViewedZone());
    path->getOpsitePath()->deletePathSegWithUndo(seg);
  }

  changedElement(path);  
  closeCommandGroup();
}

/** Used to edit the bends of the path under the pointer */
void CMapManager::slotPathEditBends(void)
{
  CMapPath *path = (CMapPath *)m_selectedElement;

  m_clipboard->slotUnselectAll();
  path->setEditMode(true);
  changedElement(path);
}

/** Used to delete the path under the pointer */
void CMapManager::slotPathDelete(void)
{
  deleteElement(m_selectedElement);
}

/** Used to display the properties of the path under the pointer */
void CMapManager::slotPathProperties(void)
{
  propertiesPath((CMapPath *)m_selectedElement);
}

/** Used to delete the text element under the pointer */
void CMapManager::slotTextDelete(void)
{
  deleteElement(m_selectedElement);
}

/** Used to display the text properties of the text element under the pointer */
void CMapManager::slotTextProperties(void)
{
  propertiesText((CMapText *)m_selectedElement);
}

/** Used to open the zone under the mouse pointer */
void CMapManager::slotZoneOpen(void)
{
  CMapZone *zone = (CMapZone *)m_selectedElement;
  getActiveView()->showPosition(QPoint(1,1),zone->getLevels()->first(),true);  
}

/** Used to open the zone under the mouse pointer */
void CMapManager::slotZoneOpenNewView(void)
{
  CMapZone *zone = (CMapZone *)m_selectedElement;
  CMapLevel *level = zone->getLevels()->first();

  openNewMapView (level);
}


/** Used to delete the zone under the mouse pointer */
void CMapManager::slotZoneDelete(void)
{
  deleteElement(m_selectedElement);
}

/** Used to display the properties of the zone under the pointer */
void CMapManager::slotZoneProperties(void)
{
  propertiesZone((CMapZone *)m_selectedElement);
}

/** Used to change the position of room/zone labels */
void CMapManager::slotChangeLabelPos()
{
  if (m_selectedElement->getElementType()==ROOM)
  {
    CMapRoom *room = (CMapRoom *)m_selectedElement;

    CMapCmdElementProperties *command = new CMapCmdElementProperties(this,i18n("Change room label position"),room);

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

    addCommand(command);
  }

  if (m_selectedElement->getElementType()==ZONE)
  {
    CMapZone *zone = (CMapZone *)m_selectedElement;

    CMapCmdElementProperties *command = new CMapCmdElementProperties(this,i18n("Change zone label position"),zone);

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

    addCommand(command);
  }
}

/** This is a debug function and not for genreal use */
void CMapManager::changeProperties(CMapElement *element,QString key,QString oldData,QString newData)
{
  CMapCmdElementProperties *cmd = new CMapCmdElementProperties(this,i18n("Change Element Property"),element);
  cmd->getOrgProperties().writeEntry(key,oldData);
  cmd->getNewProperties().writeEntry(key,newData);
  addCommand(cmd);
}

/** This is a debug function and not for genreal use */
void CMapManager::changeProperties(CMapElement *element,QString key,int oldData,int newData)
{
  CMapCmdElementProperties *cmd = new CMapCmdElementProperties(this,i18n("Change Element Property"),element);
  cmd->getOrgProperties().writeEntry(key,oldData);
  cmd->getNewProperties().writeEntry(key,newData);
  addCommand(cmd);
}

/** This is a debug function and not for genreal use */
void CMapManager::generateTestMap()
{
  kDebug() << "creating test map";
  bool smallMap = false;

  /////////////////////////////////////////////////////////////////////
  // Create a new empty map
  setUndoActive(false);
  commandHistory->clear();
  historyGroup = NULL;
  eraseMap();
  createNewMap();
  setUndoActive(true);

  /////////////////////////////////////////////////////////////////////
  // Create a test map
  // Test rooms and paths

  openCommandGroup("Create Test Map");

  if (!smallMap)
  {
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(WEST,true,"");
    movePlayerBy(NORTH,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");

    movePlayerBy(UP,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(DOWN,true,"");

    movePlayerBy(DOWN,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(SOUTH,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(EAST,true,"");
    movePlayerBy(UP,true,"");

    movePlayerBy(SOUTH,true,"");
    movePlayerBy(WEST,true,"");

    // Test text
    changeProperties(getMapData()->rootZone,"Label","",i18n("Root Zone"));

  }

  CMapLevel *level = getMapData()->getFirstZone()->getLevels()->first();
  if (level->getNextLevel())
  {
    level = level->getNextLevel();
  }

  QFont font = QFont("times");
  font.setPointSize(25);
  createText(QPoint(160,20),level,"Test Map",font,Qt::black);


  if (!smallMap)
  {
    CMapRoom *test1=createRoom(QPoint(14*20,14*20),level);
    changeProperties(test1,"Label","",i18n("Test room 1"));
    changeProperties(test1,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::WEST);
    CMapRoom *test2=createRoom(QPoint(20*20,20*20),level);
    changeProperties(test2,"Label","",i18n("Test room 2"));
    changeProperties(test2,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::EAST);
    CMapRoom *test3=createRoom(QPoint(14*20,20*20),level);
    changeProperties(test3,"Label","",i18n("Test room 3"));
    changeProperties(test3,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::WEST);
    CMapRoom *test4=createRoom(QPoint(20*20,14*20),level);
    changeProperties(test4,"Label","",i18n("Test room 4"));
    changeProperties(test4,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::NORTH);

    createPath(QPoint(14*20,14*20),level,SOUTHEAST,
                QPoint(20*20,20*20),level,NORTHWEST);
    createPath(QPoint(20*20,20*20),level,NORTHWEST,
         QPoint(14*20,14*20),level,SOUTHEAST);
    createPath(QPoint(14*20,20*20),level,NORTHEAST,
                           QPoint(20*20,14*20),level,SOUTHWEST);

    createPath(QPoint(20*20,14*20),level,SOUTHWEST,
                           QPoint(14*20,20*20),level,NORTHEAST);

    CMapPath *specialPath = createPath(test1,SPECIAL,test4,SPECIAL);
    makePathTwoWay(specialPath);

    CMapCmdElementProperties *cmd = new CMapCmdElementProperties(this,i18n("Set Special Exit"),specialPath);
    cmd->getOrgProperties().writeEntry("SpecialCmdSrc","");
    cmd->getOrgProperties().writeEntry("SpecialCmdDest","");
    cmd->getOrgProperties().writeEntry("SpecialExit",false);
    cmd->getNewProperties().writeEntry("SpecialCmdSrc","enter");
    cmd->getNewProperties().writeEntry("SpecialCmdDest","out");
    cmd->getNewProperties().writeEntry("SpecialExit",true);
    addCommand(cmd);

    CMapZone *testZone = createZone(QPoint(24*20,14*20),level);
    changeProperties(testZone,"Label","",i18n("Test Zone 2"));
    changeProperties(testZone,"LabelPos",CMapRoom::HIDE,CMapRoom::EAST);

    CMapRoom *test5 = createRoom(QPoint(3*20,3*20),testZone->getLevels()->first());
    changeProperties(test5,"Label","",i18n("Test room 5"));

    CMapPath *interZonePath = createPath(test4,NORTH,test5,NORTH);
    makePathTwoWay(interZonePath);

    makePathTwoWay(createPath(test4,SOUTH,test5,SOUTH));
    makePathTwoWay(createPath(test4,EAST,test5,WEST));


    changedElement(testZone);

    createZone(QPoint(6*20,6*20),testZone->getLevels()->first());
    font.setPointSize(8);
    createText(QPoint(20,300),level,"Multi line text lable\nThis is the second line\nThis is the third line",font,Qt::black);

  }
  closeCommandGroup();
  kDebug() << "test map created";
}
