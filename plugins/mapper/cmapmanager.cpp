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
#include <kstandarddirs.h>
#include <klocale.h>
#include <kservicetypetrader.h>
#include <kmessagebox.h>
#include <kstandardaction.h>
#include <kfiledialog.h>
#include <kpagedialog.h>
#include <kcomponentdata.h>
#include <kundostack.h>

#include <QQueue>
#include <qtimer.h>
#include <q3valuelist.h>
#include <Q3PtrList>

#include "cmapzone.h"
#include "cmappath.h"
#include "cmaptext.h"
#include "cmaproom.h"
#include "cmapview.h"
#include "cmaplevel.h"
#include "cmapview.h"
#include "cmaptoolbase.h"
#include "cmappluginbase.h"
#include "cmapzonemanager.h"

#include "cmapcmdelementcreate.h"
#include "cmapcmdelementdelete.h"
#include "cmapcmdelementproperties.h"
#include "cmapcmdgroup.h"
#include "cmapcmdmovemap.h"
#include "cmapcmdmoveplayer.h"
#include "cmapcmdleveldelete.h"
#include "cmapcmdlevelcreate.h"
#include "cmapcmdtogglepathtwoway.h"
#include "cmapfilefilterbase.h"

#include "kmuddy_mapper.h"

#include "filefilters/cmapfilefilterxml.h"

#include "dialogs/dlgmaproomproperties.h"
#include "dialogs/dlgmappathproperties.h"
#include "dialogs/dlgmaptextproperties.h"
#include "dialogs/dlgspeedwalkprogress.h"
#include "dialogs/dlgmapspeedwalk.h"
#include "dialogs/dlgmapcolor.h"
#include "dialogs/dlgmapdirections.h"
#include "dialogs/dlgmapmovement.h"

#include "cactionmanager.h"
#include "cdialoglist.h"
#include "cglobalsettings.h"
#include "cstatus.h"

CMapManager::CMapManager (QWidget *parent, KMuddyMapper *mapper, int sessId) :
  cActionBase ("map-manager", 0),
  m_sessId (sessId),
  mapperPlugin (mapper)
{
  kDebug() << "constructor begins";

  // register action handlers
  addEventHandler ("dialog-create", 50, PT_STRING);
  addEventHandler ("dialog-save", 50, PT_STRING);

  mapData = new CMapData();

  // Setup vars
  loginRoom = NULL;
  currentRoom = NULL;
  elementEdit = NULL;

  /** Create undo/redo history */
  commandHistory = new KUndoStack();
  //FIXME_jp: Needs to be configurable
  commandHistory->setUndoLimit(30);
  commandHistory->clear();
  historyGroup = NULL;
  m_commandsActive = true;

  initFileFilters();

  activeView = 0;

  setDefaultOptions();

  speedwalkActive = false;
  pathToWalk.setAutoDelete(true);

  speedwalkProgressDlg = new DlgSpeedwalkProgress();
  speedwalkProgressDlg->hide();
  connect(speedwalkProgressDlg,SIGNAL(abortSpeedwalk()),this,SLOT(slotAbortSpeedwalk()));

  m_zoneCount = 0;
  m_levelCount = 0;
  m_zoneManager = NULL;

  setUndoActive (false);
  createNewMap();  // because some things break if a map doesn't exist
  activeView = new CMapView(this, parent);

  m_zoneManager = new CMapZoneManager(sessId, this);
  if (!m_zoneManager->zonesModel()->rowCount())
    m_zoneManager->createZone (i18n ("Map #1"));
  m_zoneManager->loadZone(0);

  initPlugins();
  activeView->initGUI();

  readOptions();

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
  delete m_zoneManager;
  delete activeView;

  if (commandHistory)
    delete commandHistory;

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
  /*KPageDialog *dlg = */(KPageDialog *) cDialogList::self()->getDialog ("profile-prefs");

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
    plugin->createProfileConfigPanes();
}

void CMapManager::createGlobalConfigPanes ()
{
  KPageDialog *dlg = (KPageDialog *) cDialogList::self()->getDialog ("app-prefs");

  KPageWidgetItem *item;
  QFrame *frmdir = new QFrame (dlg);
  item = dlg->addPage (frmdir, i18n ("Mapper: Directions"));
  item->setIcon (KIcon ("gear"));
  QFrame *frmmovement = new QFrame (dlg);
  item = dlg->addPage (frmmovement, i18n ("Mapper: Movement"));
  item->setIcon (KIcon ("run"));
  QFrame *frmcolor = new QFrame (dlg);
  item = dlg->addPage (frmcolor, i18n ("Mapper: Colors"));
  item->setIcon (KIcon ("colorize"));
  QFrame *frmspeedwalk = new QFrame (dlg);
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

QList<CMapPropertiesPaneBase *> CMapManager::createPropertyPanes(elementTyp type,CMapElement *element,QWidget *parent)
{
  QList<CMapPropertiesPaneBase *> res;
  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
    res.append(plugin->createPropertyPanes(type, element, parent));
  return res;
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
    plugin = factory->create<CMapPluginBase> (activeView);

    if (!plugin)
    {
      kWarning() << "Plugin creation failed!";
    }
    else
    {
      pluginList.append(plugin);
  
      kDebug() << "Tools in plugin : " << plugin->getToolList()->count();
      foreach (CMapToolBase *tool, *plugin->getToolList())
      {
        toolList.append(tool);
      }

//      getActiveView()->insertChildClient(plugin);
      
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

     kDebug() << "XML File : " << activeView->xmlFile(); 
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

CMapZone *CMapManager::getZone(bool noCreate)
{
  CMapZone *zone = mapData->rootZone;
  if ((!zone) && (!noCreate))
    zone = new CMapZone(this);

  return zone;
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
      displayLevel(firstRoom->getLevel(), true);
  }
}

void CMapManager::displayLevel(CMapLevel *level, bool centerView)
{
  CMapView *mapView = getActiveView();
  mapView->showPosition(level, centerView);
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
  CMapView *mapView = getActiveView();
  mapView->showPosition(pos,level);
}

/** Used to set properties of the view widget */
void CMapManager::setPropertiesAllViews(QCursor *cursor,bool mouseTracking)
{  
  activeView->setCursor(*cursor);
  activeView->setMouseTracking(mouseTracking);
}

/** Used to unselect all the elements in a level */
void CMapManager::unselectElements(CMapLevel *level)
{
  QList<CMapElement *> lst = level->getAllElements();
  foreach (CMapElement *element, lst)
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
        else
        {
          int x = properties.readEntry("X",-5);
          int y = properties.readEntry("Y",-5);

          foreach (CMapText *text, *level->getTextList())
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

/** Used to erase the map. This will erase all elements and can't be undone */
void CMapManager::eraseMap(void)
{
  if (!mapData->rootZone) return;

  eraseZone(mapData->rootZone);
  delete mapData->rootZone;
  mapData->rootZone = NULL;

  m_zoneCount = 0;
  m_levelCount = 0;

  if (activeView) activeView->setLevel(NULL);

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

  QList<CMapLevel *> levels;
  for (unsigned int idx = 0; idx < zone->levelCount(); ++idx)
    levels.append(zone->getLevel(idx));
  foreach (CMapLevel *level, levels)
  {
    // TODO: this seems to not delete things correctly
    foreach (CMapRoom *room, *level->getRoomList())
    {        
      room->getPathList()->clear();
      room->getConnectingPathList()->clear();
    }
    level->getRoomList()->clear();
    level->getTextList()->clear();

    delete level;
  }
}

/** Create new bottom or top level depending on the given direction */
CMapLevel *CMapManager::createLevel(directionTyp dir)
{
  CMapLevel *result = NULL;

  CMapCmdLevelCreate *cmd = NULL;

  int pos = (dir == UP) ? getZone()->levelCount() : 0;
  if (getUndoActive())
  {
    cmd = new CMapCmdLevelCreate(this,i18n("Create Level"),pos);
    addCommand(cmd);
    
    result = cmd->getLevel();
  }
  else
  {
    result = new CMapLevel(this, pos);
  }
  
  return result;
}

/** This is used to find a level with a given id
  * @param id The id of the level to find
  * @return Null if no level is found otherwise a pointer to the level */
CMapLevel *CMapManager::findLevel(unsigned int id)
{
  CMapZone *zone = getZone();
  for (unsigned int idx = 0; idx < zone->levelCount(); ++idx)
  {
    CMapLevel *level = zone->getLevel(idx);
    if (level->getLevelID() == id)
      return level;
  }

  return NULL;
}

/** Create new map */
void CMapManager::createNewMap()
{
  // Create the root zone
  getMapData()->rootZone = NULL;  

  CMapZone *zone = getZone();

  // Create a empty room in the first level of the new zone
  CMapRoom *room = CMapElementUtil::createRoom(this, QPoint(2 * mapData->gridSize.width(),2 * mapData->gridSize.height()),zone->firstLevel());
  setCurrentRoomWithoutUndo(room);
  setLoginRoomWithoutUndo(room);

  if (!activeView) return;

  if (currentRoom) activeView->showPosition(currentRoom->getLowPos(),zone->firstLevel());

  if (activeView->getCurrentlyViewedLevel()==NULL)  
    activeView->showPosition(loginRoom,true);

  for (CMapPluginBase *plugin = getPluginList()->first(); plugin!=0; plugin = getPluginList()->next())
  {
    plugin->newMapCreated();
  }

  activeView->changed();
}

/** Used to create a new room that can be undone/redone
   * @param pos The position to create the room
   * @param level The level to create the room in
   */
void CMapManager::createRoom(QPoint pos,CMapLevel *level)
{
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
  }
  else
  {
    CMapElementUtil::createRoom(this, pos, level);
  }
}

/** Used to create a new text label */
void CMapManager::createText(QPoint pos,CMapLevel *level,QString str)
{
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
  }
  else
  {
    CMapElementUtil::createText(this, pos, level, str);
  }
}

/** Used to create a new text label */
void CMapManager::createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col)
{
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
  }
  else
  {
    CMapElementUtil::createText(this, pos, level, str, font, col);
  }
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

  foreach (room, *srcLevel->getRoomList())
  {
    if (room->getLowPos() == srcPos)
    {
      srcRoom = room;
      break;
    }
  }

  foreach (room, *destLevel->getRoomList())
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
  CMapPath *result = NULL;

  KMemConfig properties;
  KConfigGroup props = properties.group("Properties");
  // Auto-set the directions if possible
  directionTyp srcDir = srcRoom->bestDirectionToRoom(destRoom);
  directionTyp destDir = getOpsiteDirection(srcDir);
  if (srcDir != SPECIAL) {
    // nothing if there is an exit already
    if (srcRoom->getPathDirection(srcDir, QString())) srcDir = SPECIAL;
    if (destRoom->getPathDirection(destDir, QString())) destDir = SPECIAL;
  }
  if ((srcDir != SPECIAL) && (destDir != SPECIAL)) {
    props.writeEntry("SrcDir", (int) srcDir);
    props.writeEntry("DestDir", (int) destDir);
  }

  DlgMapPathProperties d(this,props,false);

  if (!d.exec()) return NULL;

  kDebug() << "createPath 1";

  srcDir = (directionTyp)props.readEntry("SrcDir",0);
  destDir = (directionTyp)props.readEntry("DestDir",0);
  QString specialCmdSrc = props.readEntry("SpecialCmdSrc");
  QString specialCmdDest = props.readEntry("SpecialCmdDest");

  if (srcRoom->getPathDirection(srcDir,specialCmdSrc) || destRoom->getPathDirection(destDir,specialCmdDest))
  {
    KMessageBox::information (NULL,i18n("A path already exists at this location"),i18n("KMuddy Mapper"));
    return NULL;
  }

  kDebug() << "createPath 2";
  // create
  props.writeEntry("Type",(int)PATH);
  props.writeEntry("SrcRoom",srcRoom->getRoomID());
  props.writeEntry("SrcDir",(int)srcDir);
  props.writeEntry("SrcLevel",srcRoom->getLevel()->getLevelID());
  props.writeEntry("DestRoom",destRoom->getRoomID());
  props.writeEntry("DestDir",(int)destDir);        
  props.writeEntry("DestLevel",destRoom->getLevel()->getLevelID());

  CMapCmdElementCreate *command = new CMapCmdElementCreate(this,i18n("Create Path"));
  command->addElement(&properties);

  addCommand(command);

  QList<CMapElement *> *elements=command->getElements();

  foreach (CMapElement *el, *elements)
    if (el->getElementType()==PATH)
      result = (CMapPath *)el;

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

  if (!undoable)
  {
    setUndoActive(active);
  }

  QList<CMapElement *> *elements=command->getElements();
  foreach (CMapElement *el, *elements)
    if (el->getElementType()==PATH)
      result = (CMapPath *)el;

  return result;
}


/** Find the first room in the map, if one can't be found then create one */
CMapRoom *CMapManager::findFirstRoom(CMapRoom *existingRoom)
{
  CMapZone *zone = getZone();
  for (unsigned int idx = 0; idx < zone->levelCount(); ++idx)
  {
    CMapLevel *level = zone->getLevel(idx);
    foreach (CMapRoom *room, *level->getRoomList())
      if (room!=existingRoom)
        return room;
  }

  // If we get to this point then no room was found so create one
  return CMapElementUtil::createRoom(this, QPoint(2 * mapData->gridSize.width(),2 * mapData->gridSize.height()), getZone()->firstLevel());
}

void CMapManager::deleteLevel(CMapLevel *level)
{
  //FIXME_jp: Check to see what happens when all levels are deleted.
    //          It may be nessecary to create a new one.

  // Delete the level
  CMapCmdLevelDelete *cmd = new CMapCmdLevelDelete(this,i18n("Delete Level"),level);
  addCommand(cmd);
}


/** Check to see if a string is a valid move command
  * @param dirCmd The command that was typed
  * @return True, if a valid command otherwise false */
bool CMapManager::validMoveCmd(QString dirCmd)
{
  if (dirCmd.isEmpty()) return false;

  // check for directions
  for (uint i = 0; i < NUM_DIRECTIONS; ++i)
    if (mapData->directions[i] == dirCmd)
      return true;
  
  if (currentRoom)
    foreach (CMapPath *path, *currentRoom->getPathList())
      if (path->getSpecialExit() && (path->getSpecialCmd()==dirCmd))
        return true;

  return false;
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

  if (element->getElementType() == ZONE)
  {
    // Delete the levels in the zone
    CMapZone *zone = (CMapZone *)element;
    QList<CMapLevel *> levels;
    for (unsigned int idx = 0; idx < zone->levelCount(); ++idx)
      levels.append(zone->getLevel(idx));
    foreach (CMapLevel *level, levels)
      deleteLevel(level);
  }

  deleteElementWithoutGroup(element,delOpsite);

  closeCommandGroup();
}

bool CMapManager::isClean() const
{
  return commandHistory->isClean();
}

/** Used to load a map */
void CMapManager::importMap(const QString& file,CMapFileFilterBase *filter)
{
  QFile f(file);
  if (!f.exists()) {
    createNewMap();
    return;
  }

  setUndoActive(false);
  commandHistory->clear();
  historyGroup = NULL;

  eraseMap();

  // Load the map using the correct filter
  filter->loadData(file);

  if (!getLoginRoom())
  {
    CMapRoom *firstRoom = findFirstRoom(NULL);
    setLoginRoom(firstRoom);
  }

  setCurrentRoomWithoutUndo(loginRoom);

  if (loginRoom && activeView)
  {
    if (activeView->getCurrentlyViewedLevel()==NULL)  
      activeView->showPosition(loginRoom,true);
    setCurrentRoom(loginRoom);
  }

  setUndoActive(true);
}

/** Used to save a map */
void CMapManager::exportMap(const QString& url,CMapFileFilterBase *filter)
{
  filter->saveData(url);
  commandHistory->setClean();
}

/** Used to inform to change the state of the navigation tools */
void CMapManager::activeViewChanged(void)
{
  if (!activeView) return;
  CMapLevel *level = activeView->getCurrentlyViewedLevel();
  if (!level) return;

  // nothing here at this time
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
  if (!activeView) return;

  for (CMapPluginBase *plugin = getPluginList()->first();plugin!=0; plugin= getPluginList()->next())
  {
    plugin->elementChanged(element);
  }

  activeView->changedElement(element);
}

/** Used to inform the various parts of the mapper that a element has added */
void CMapManager::addedElement(CMapElement *element)
{
  if (!activeView) return;
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

  activeView->readOptions();

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

void CMapManager::getCounts(int *levels,int *rooms,int *paths,int *labels)
{
  *rooms = 0;
  *labels = 0;
  *paths = 0;
  CMapZone *zone = getZone();
  *levels = zone->levelCount();

  for (unsigned int idx = 0; idx < zone->levelCount(); ++idx)
  {
    CMapLevel *level = zone->getLevel(idx);

    foreach (CMapRoom *room, *level->getRoomList())
      *paths += room->getPathList()->count();

    *rooms += level->getRoomList()->count();
    *labels += level->getTextList()->count();
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

  activeView->changed();
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
void CMapManager::addCommand(CMapCommand *command)
{
  if (getUndoActive())
  {
    if (historyGroup)
    {
      // If there is a history group, it will -not- execute anything. That will happen once the group gets closed.
      historyGroup->addCommand(command);
    }
    else
    {
      commandHistory->push(command);
    }
  }
  else
  {
    command->redo();
  }
}

void CMapManager::openCommandGroup(QString name)
{
  CMapCmdGroup *group = new CMapCmdGroup(this,name);
  group->setPreviousGroup(historyGroup);
  historyGroup = group;
}

void CMapManager::closeCommandGroup()
{
  CMapCmdGroup *currentGroup = historyGroup;
  CMapCmdGroup *oldGroup =historyGroup->getPreviousGroup();
  historyGroup = oldGroup;
  addCommand(currentGroup);
}

/** move the player relative to the current position of the player
  * @param cmd The move command used to move the player */
void CMapManager::movePlayerBy(QString cmd)
{
  QString specialCmd = "";
  directionTyp dir  = textToDirection(cmd);

  if (dir == SPECIAL)
    specialCmd = cmd;

  movePlayerBy(dir, getActiveView()->getCreateMode(), specialCmd);
}

/** move the player relative to the current position of the player
  * This command has support for special paths, but can only move
  * anlong exsiting special paths. In other words, it is unable to
  * create special paths.
  */
void CMapManager::movePlayerBy(directionTyp dir,bool create,QString specialCmd)
{
  // Make sure that the room we are currently in is visible
  if (!currentRoom) return;

  CMapCmdMovePlayer *cmd = new CMapCmdMovePlayer(this, dir, specialCmd, create);
  addCommand(cmd);
}

/** Used to walk the player in the mud to a given room */
void CMapManager::walkPlayerTo(CMapRoom *toRoom)
{
  QQueue<CMapRoom *> roomsToVisit;
  CMapRoom *destRoom;
  CMapRoom *srcRoom;
  CMapPath *path;
  CMapRoom *foundRoom;
  signed int time = 0;
  bool bFound = false;
  CMapPath *foundPath = NULL;

  int speedWalkAbortCount = 0;

  if ((!currentRoom) || (currentRoom == toRoom)) return;

  if (speedwalkActive)
  {
    KMessageBox::information (NULL,i18n("Speedwalking is already in progress"),i18n("KMuddy Mapper"));
    return;
  }

  speedwalkActive = true;

  pathToWalk.clear();

  // Reset the seach count for all the rooms
  for (unsigned int idx = 0; idx < getZone()->levelCount(); ++idx)
  {
    CMapLevel *level = getZone()->getLevel(idx);
    foreach (CMapRoom *room, *level->getRoomList())
      room->setMoveTime(-1);
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
    foreach (path, *foundRoom->getPathList())
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
    KMessageBox::information (NULL,i18n("The automapper was unable to find a path to requested room"),i18n("KMuddy Mapper"));
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
    foreach (path, *destRoom->getConnectingPathList())
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
      KMessageBox::information (NULL,i18n("Speedwalk abort because move limit was reached"),i18n("KMuddy Mapper"));

      return;
    }
  }

  speedwalkProgressDlg->setTotalSteps(pathToWalk.count());
  speedwalkProgressDlg->setProgress(0);

  cActionManager *am = cActionManager::self();
  cStatus *status = dynamic_cast<cStatus *>(am->object ("status", am->activeSession()));
  if (status)
    status->statusBar()->addPermanentWidget(speedwalkProgressDlg,0);
  speedwalkProgressDlg->show();
  speedwalkProgress = 0;

  // Start walking path
  slotWalkPlayerAlongPath();
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

/** This method is used to move the elements in a zone by the given vector */
void CMapManager::moveMap(QPoint inc,CMapZone *)
{
  CMapCmdMoveMap *cmd = new CMapCmdMoveMap(this,inc,i18n("Move Elements in map"));
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

/** Get the main window */
CMapView *CMapManager::getActiveView()
{  
  return activeView;
}                                                                    

/** Used to repaint all the views */
void CMapManager::redrawAllViews(void)
{
  activeView->changed();
}

CMapFileFilterBase *CMapManager::nativeFilter(bool isLoad)
{
  for (CMapFileFilterBase *filter = m_fileFilter.first();filter!=0;filter=m_fileFilter.next())
  {
    if (isLoad && (!filter->supportLoad())) continue;
    if ((!isLoad) && (!filter->supportSave())) continue;
    if (filter->isNative()) return filter;
  }
  return 0;
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
    importMap(filename,filter);
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

  exportMap(mapDir + "/" + "map"+ filter->getExtension(), filter);
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
    changeProperties(getZone(),"Label","",i18n("Root Zone"));

  }

  CMapLevel *level = getZone()->firstLevel();
  if (level->getNextLevel())
    level = level->getNextLevel();

  QFont font = QFont("times");
  font.setPointSize(25);
  createText(QPoint(160,20),level,"Test Map",font,Qt::black);


  if (!smallMap)
  {
    CMapRoom *test1=CMapElementUtil::createRoom(this, QPoint(14*20,14*20),level);
    changeProperties(test1,"Label","",i18n("Test room 1"));
    changeProperties(test1,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::WEST);
    CMapRoom *test2=CMapElementUtil::createRoom(this, QPoint(20*20,20*20),level);
    changeProperties(test2,"Label","",i18n("Test room 2"));
    changeProperties(test2,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::EAST);
    CMapRoom *test3=CMapElementUtil::createRoom(this, QPoint(14*20,20*20),level);
    changeProperties(test3,"Label","",i18n("Test room 3"));
    changeProperties(test3,"LabelPos",(int)CMapRoom::HIDE,(int)CMapRoom::WEST);
    CMapRoom *test4=CMapElementUtil::createRoom(this, QPoint(20*20,14*20),level);
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
  }
  closeCommandGroup();
  kDebug() << "test map created";
}

