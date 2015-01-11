/***************************************************************************
                          cmapmanager.h  -  description
                             -------------------
    begin                : Wed Mar 7 2001
    copyright            : (C) 2001 by KMud Development Team
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

#ifndef CMAPMANAGER_H
#define CMAPMANAGER_H


#include <qfont.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpoint.h>
#include <qobject.h>
#include <q3ptrlist.h>
#include <qworkspace.h>
#include <q3ptrstack.h>
#include <qcursor.h>

#include <kurl.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kaction.h>
#include <kselectaction.h>
#include <ktoggleaction.h>
#include <k3command.h>
#include <kxmlguiwindow.h>
#include <ksimpleconfig.h>
#include <kstatusbar.h>
#include <kconfiggroup.h>

#include "cactionbase.h"

#include "cmapdata.h"

#include <kmuddy_export.h>

class CMapZone;
class CMapPath;
class CMapText;
class CMapRoom;
class CMapView;
class CMapViewBase;
class CMapLevel;
class CMapToolBase;
class CMapPluginBase;
class CMapFilter;
class CMapCmdGroup;
class CMapFileFilterBase;
class CMapClipboard;
class CMapElementUtil;

class DlgMapRoomProperties;
class DlgMapTextProperties;
class DlgMapPathProperties;
class DlgSpeedwalkProgress;
class DlgMapDirections;
class DlgMapMovement;
class DlgMapColor;
class DlgMapSpeedwalk;

class KMuddyMapper;

class KVBox;
class KComponentData;
class QActionGroup;

/**This is used to make the mapper perform different tasks
  *@author KMud Development Team
  */

class KMUDDY_EXPORT CMapManager : public KXmlGuiWindow, public cActionBase
{
        Q_OBJECT
public:
  /** Constructor used to creat the map manager */
  CMapManager (KMuddyMapper *mapper);
  ~CMapManager();

  void eventStringHandler (QString event, int, QString &par1, const QString &);

  /** Used to get a list of the plugins */
  Q3PtrList<CMapPluginBase> *getPluginList();
  /**
   * Used to create a new view of the map
   * actually returns a CMapViewBase*,  dynamic_cast if necessary
   */
  void openMapView();
  /**
   * Used to create a new view of the a given level
   */
  void openNewMapView(CMapLevel *level);
  /**
   * Used to create a new view of the a given level and
   * center the view on the given position.
   */
  void openNewMapView(QPoint pos,CMapLevel *level);
  /** Create new map */
  void createNewMap();
  /** Used to set properties of the view widget */
  void setPropertiesAllViews(QCursor *cursor,bool mouseTracking);
  /** Create new bottom or top level depending on the given direction */
  CMapLevel *createLevel(directionTyp dir,CMapZone *intoZone);
  /** Used to create a new room */
  CMapRoom *createRoom(QPoint pos,CMapLevel *level);
  /** Used to create a new path, this method will display the path properties
   * dialog to obtain the porperties of the path to be created               */
  CMapPath *createPath(CMapRoom *srcRoom,CMapRoom *destRoom);
  /** Used to create a new path*/
  CMapPath *createPath(QPoint srcPos,CMapLevel *srcLevel,directionTyp srcDir,
                       QPoint destPos,CMapLevel *destLevel,directionTyp destDir);
  /** Used to create a new path*/
  CMapPath *createPath (CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir,bool undoable = true);
  /** Used to create a new zone */
  CMapZone *createZone(QPoint pos,CMapLevel *level,bool levelCreate=true);
  /** Used to create a new text label */
  CMapText *createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col);
  /** Used to create a new text label */
  CMapText *createText(QPoint pos,CMapLevel *level,QString str="");

  /** Used to load a map */
  void importMap(const KUrl& url,CMapFileFilterBase *filter);
  /** Used to save a map */
  void exportMap(const KUrl& url,CMapFileFilterBase *filter);

  void setDefaultOptions();
  /** Used to read the map options */
  void readOptions();
  /** Used to write the map options */
  void saveGlobalConfig();

  /** Used to alter the path properties */
  bool propertiesPath(CMapPath *path);
  /** Used to alter the room properties */
  bool propertiesRoom(CMapRoom *room);
  /** Used to alter the zone properties */
  bool propertiesZone(CMapZone *zone);
  /** Used to alter the text properties */
  bool propertiesText(CMapText *text);

  /** This method is used to covert cords so that they snap to the grid */
  QPoint cordsSnapGrid(QPoint oldPos);

  /** Used to get a pointer to the map data */
  CMapData *getMapData() const;

  /** Used to find any elements at the given location
    * @param pos The location to look for the element
    * @param level The level to look for the element on
    * @return NULL if no element was found otherwise a pointer to the element */
  CMapElement *findElementAt(QPoint pos,CMapLevel *level);
  /** Used to find any rooms at the given location
    * @param pos The location to look for the room
    * @param level The level to look for the room on
    * @return NULL if no room was found otherwise a pointer to the room */
  CMapRoom *findRoomAt(QPoint pos,CMapLevel *level);


  /** Used to set the login room */
  void setLoginRoom(CMapRoom *room);
  /** Uesd to return teh login room */
  CMapRoom *getLoginRoom();
  /** Used to set the current room */
  void setCurrentRoom(CMapRoom *room);
  /** Uesd to return the current room */
  CMapRoom *getCurrentRoom();
  /** move the player relative to the current position of the player
    * This command has support for special paths, but can only move
    * anlong exsiting special paths. In other words, it is unable to
    * create special paths.
    */
  void movePlayerBy(directionTyp dir,bool create,QString specialCmd);
  /** move the player relative to the current position of the player
    * @param cmd The move command used to move the player */
  void movePlayerBy(QString cmd);
  /** This method is used to convert a direction into a offset */
  void directionToCord(directionTyp dir, QSize distance,QPoint *pos);
  /** This method is used to move the elements in a zone by the given vector */
  void moveMap(QPoint inc,CMapZone *zone);
  /** This method is used to make a path one way */
  void makePathOneWay(CMapPath *path);
  /** This method is used to make a path two way */
  void makePathTwoWay(CMapPath *path);

  /** Used to convert a text direction to a direction type */
  directionTyp textToDirection(QString text);
  /** Used to convert a direction type to a text direction */
  QString directionToText(directionTyp dir,QString specialCmd);

  /** Get the level that has focues at the moment */
  CMapViewBase *getActiveView(void);
  /** Used to inform the various parts of the mapper that a element has changed */
  void changedElement(CMapElement *element);
  /** Used to inform the various parts of the mapper that a element has added */
  void addedElement(CMapElement *element);
  /** Used to walk the player in the mud to a given room */
  void walkPlayerTo(CMapRoom *toRoom);
  /** Used to inform to change the state of the navigation tools */
  void activeViewChanged(void);

  /** Used to cound the diffent elements of the map */
  void getCounts(int *levels,int *rooms,int *paths,int *labels,int *zones);

  /** Get the opsite direction */
  directionTyp getOpsiteDirection(directionTyp dir);

  /** Used to add a command to the command history */
  void addCommand(K3Command *command,bool execute = true);

  /** Used to set the current tool */
  void setCurrentTool(CMapToolBase *tool);
  /** Usd to get the current tool */
  CMapToolBase *getCurrentTool(void);
  /** Used to unselect all the elements in a level */
  void unselectElements(CMapLevel *level);
  /** Used to tell that map views that a level has chaned */
  void levelChanged(CMapLevel *level);
  /** This method puts a element into edit state */
  void setEditElement(CMapElement *element);
  /** This is used to remove edit state from element being edited */
  void unsetEditElement(void);
  /** This gets the element that is being edited */
  CMapElement *getEditElement(void);
  /** This cancels any editing */
  void stopEditing(void);
  /** Check to see if a string is a valid move command
    * @param dirCmd The command that was typed
    * @return True, if a valid command otherwise false */
  bool validMoveCmd(QString dirCmd);
  /** This is used to find a level with a given id
    * @param id The id of the level to find
    * @return Null if no level is found otherwise a pointer to the level */
  CMapLevel *findLevel(unsigned int id);
  /** This is used to find a zone with a given id
    * @param id The id of the zone to find
    * @return Null if no zone is found otherwise a pointer to the zone */
  CMapZone *findZone(unsigned int id);
  /** Used to create a new command group */
  void openCommandGroup(QString name);
  /** Used to close a command group */
  void closeCommandGroup(void);
  /** Should only be called by CMapRoom.setCurrentRoom() */
  void setCurrentRoomWithoutUndo(CMapRoom *room);
  /** Should only be called by CMapRoom.setLoginRoom() */
  void setLoginRoomWithoutUndo(CMapRoom *room);
  /** Delete a level from the map */
  void deleteLevel(CMapLevel *level);
  /** Update Zone List */
  void updateZoneListCombo(void);
  /** Find the first room in the map,if one can't be found then create one */
  CMapRoom *findFirstRoom(CMapRoom *exsitingRoom);
  /**
     * This method is used to set the element when that a popup menu is to be showed for
     */
  void setSelectedElement(CMapElement *element);
  /**
   * This methid is used to set the position that a element was selected when the contex menu is shown
     */
  void setSelectedPos(QPoint pos);
  /**
     * This method is used to get the element that a popup menu is being displayed for
     */
  CMapElement *getSelectedElement(void);
  /** This is used to tell the mapper if commands should be added to the history list */
  void setUndoActive(bool active);
  int getUndoActive(void);
  /** Used to erase the map. This will erase all elements and can't be undone */
  void eraseMap(void);

  /** This method tells the plugins that the mapper wants to add a room to the speedwalk list
    * @param room The room to add to the speedwalk list
    */
  void addSpeedwalkRoom(CMapRoom *room);

  /** This method is used to find a element from a list of properties
      * @param properties The list of proerties
      * @return The element if it's found otherwise NULL */
  CMapElement *findElement(KConfigGroup properties);

  void createGlobalConfigPanes();
  void createProfileConfigPanes();

  QString defaultSavePath () const;

  KComponentData instance ();

  QActionGroup *toolGroup () { return m_toolGroup; }

public:
  /** A count of the number of levels created */
  int m_levelCount;
  /** A count of the number of zones created */
  int m_zoneCount;

signals:
  void closed ();

public slots:
  /** Used to delete a element from the map */
  void deleteElement(CMapElement *element,bool delOpsite = true);
  /** Used to abort a speedwalk that is in progress */
  void slotAbortSpeedwalk(void);
  

private slots:
  /** This method is called to create a new map, when the new map menu option is selected */
  void slotFileNew();
  /** This method is called to load a map from a file when the load menu option is selected */
  void slotFileLoad();
  /** This method is called to save the map to a file when the "save as" menu option is selected */
  void slotFileSave();
  /** This methid is called to display map information when the information menu option is selected */
  void slotFileInfo();

  void slotToolsGrid();
  void slotToolsLevelUp();
  void slotToolsLevelDown();
  void slotToolsLevelDelete();
  void slotToolsZoneUp();
  void slotToolsDeleteZone();
  void slotToolsCreateMode();
  void slotToolsZoneProperties();
  void slotSelectZone();

  void slotViewUpperLevel();
  void slotViewLowerLevel();
  void slotViewToolsToolbar();
  void slotViewNavToolbar();

  void slotWalkPlayerAlongPath(void);

  /** Used to change the position of room/zone labels */
  void slotChangeLabelPos();
  /** Used to room under the point the current room */
  void slotRoomSetCurrentPos(void);
  /** Used to room under the point the login room */
  void slotRoomSetLogin(void);
  /** Used to set speedwalk to the room under the pointer */
  void slotRoomSpeedwalkTo(void);
  /** Used to delete the room under the pointer */
  void slotRoomDelete(void);
  /** Used to display the properties of the room under the pointer */
  void slotRoomProperties(void);
  /** Used to make  the path under the pointer one way */
  void slotPathOneWay(void);
  /** Used to make the the path under the pointer two way */
  void slotPathTwoWay(void);
  /** Used to add a bend to the path under the pointer */
  void slotPathAddBend(void);
  /** Used to delete the path segment under the pointer */
  void slotPathDelBend(void);
  /** Used to edit the bends of the path under the pointer */
  void slotPathEditBends(void);
  /** Used to delete the path under the pointer */
  void slotPathDelete(void);
  /** Used to display the properties of the path under the pointer */
  void slotPathProperties(void);
  /** Used to delete the text element under the pointer */
  void slotTextDelete(void);
  /** Used to display the text properties of the text element under the pointer */
  void slotTextProperties(void);
  /** Used to open the zone under the mouse pointer */
  void slotZoneOpen(void);
  /** Used to delete the zone under the mouse pointer */
  void slotZoneDelete(void);
  /** Used to display the properties of the zone under the pointer */
  void slotZoneProperties(void);
  /** Used to open a zone into a new view */
  void slotZoneOpenNewView(void);
  
private:
  /** Used to erase a zone recursively */
  void eraseZone(CMapZone *zone);
  /** Used to delete a element from the map, should only be used by the deleteElementMethod() */
  void deleteElementWithoutGroup(CMapElement *element,bool delOpsite = true);
  /** Used to setup the menus */
  void initMenus();
  /** Used to create the plugins */
  void initPlugins();
  /** This will setup the import/export file filters */
  void initFileFilters();

  /**
   * Used to enable/disable the view actions
   * @param If true then enable the actions otherwise disable the actions
   */
  void enableViewControls(bool enable);
  /**
   * This method is used to disable/enable mapper actions that are not done by enableViewControls()
   * @param If true then enable the actions otherwise disable the actions
   */
  void enableNonViewActions(bool enabled);
  /** Used to repaint all the views */
  void redrawAllViews(void);
  
  virtual bool queryClose ();

private:
  /** This is used to add and delete map elements and levels */
  CMapElementUtil *m_elementUtils;
  /** This is the map clipboard used for copy and paste actions */
  CMapClipboard *m_clipboard;
  /** A list of import/export filters */
  Q3PtrList<CMapFileFilterBase> m_fileFilter;
  /** The element that is selected ( when the right mouse button menu is show for it) */
  CMapElement *m_selectedElement;
  /** The position that the mouse is in when the context menu is show for a element */
  QPoint m_selectedPos;
  /** If this is true then commands are added to the history, otherwise they are not */
  bool m_commandsActive;
  /** The filter used to proces mud input/output */
  CMapFilter *filter;
  /** The element currenly being edited */
  CMapElement *elementEdit;
  /** The current tool */
  CMapToolBase *currentTool;
  /** A list of all loaded tools */
  Q3PtrList<CMapToolBase> toolList;
  /** The undo/redo history */
  K3CommandHistory *commandHistory;
  /** Points the the command history currently being used */
  CMapCmdGroup *historyGroup;
  /** Used to store the amount of steps taken in the speedwalk */
  int speedwalkProgress;
  /** A pointer to the active map view */
  CMapViewBase *activeView;
  /** The zone combo box */
  KSelectAction *zoneMenu;
  /** A pointer to the current room */
  CMapRoom *currentRoom;
  /** A pointer to the login room */
  CMapRoom *loginRoom;
  /** A pointer to the map data */
  CMapData *mapData;
  /** A list of all the plugins */
  Q3PtrList<CMapPluginBase> pluginList;
  /** pointer to the speedwalk status indicaor */
  DlgSpeedwalkProgress *speedwalkProgressDlg;

  // config pages:
  DlgMapDirections *mapDirection;
  DlgMapMovement *mapMovement;
  DlgMapColor *mapColor;
  DlgMapSpeedwalk *mapSpeedwalk;

  //Actions
  KAction *m_fileNew;
  KAction *m_fileLoad;
  KAction *m_fileSave;
  KAction *m_fileInfo;

  KToggleAction *m_toolsGrid;
  KToggleAction *m_toolsCreate;
  KAction *m_toolsUpLevel;
  KAction *m_toolsDownLevel;
  KAction *m_toolsDeleteLevel;
  KAction *m_toolsUpZone;
  KAction *m_toolsZoneProp;
  KAction *m_toolsDeleteZone;

  KToggleAction *m_viewLowerLevel;
  KToggleAction *m_viewUpperLevel;
  KToggleAction *m_viewToolsToolbar;
  KToggleAction *m_viewNavToolbar;

  KSelectAction *labelMenu;

  QActionGroup *m_toolGroup;

  /** Used to tell if speedwalk is active */
  bool speedwalkActive;
  /** The rooms that are to be speed walked */
  Q3PtrStack<QString> pathToWalk;

  KMuddyMapper *mapperPlugin;
  KVBox *container;
private:
  /** This method is used to save the map to the default location */
  void loadMap(void);
  /** This method is used to load the map from the default location */
  void saveMap(void);

  /** This is a debug function and not for genreal use */
  void changeProperties(CMapElement *element,QString key,QString oldData,QString newData);
  /** This is a debug function and not for genreal use */
  void changeProperties(CMapElement *element,QString key,int oldData,int newData);

public:
  /** This is a debug function and not for genreal use */
  void generateTestMap();
  void listZones(CMapZone *zone);
};

#endif
