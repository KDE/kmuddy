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
#include <ksimpleconfig.h>
#include <kstatusbar.h>
#include <kconfiggroup.h>

#include "cactionbase.h"

#include "cmapcommand.h"
#include "cmapdata.h"

#include <kmuddy_export.h>

class CMapPath;
class CMapText;
class CMapRoom;
class CMapView;
class CMapLevel;
class CMapToolBase;
class CMapPluginBase;
class CMapFilter;
class CMapCmdGroup;
class CMapFileFilterBase;
class CMapClipboard;
class CMapElementUtil;
class CMapPropertiesPaneBase;
class CMapZoneManager;

class DlgMapRoomProperties;
class DlgMapTextProperties;
class DlgMapPathProperties;
class DlgSpeedwalkProgress;
class DlgMapDirections;
class DlgMapMovement;
class DlgMapColor;
class DlgMapSpeedwalk;

class KMuddyMapper;

class KComponentData;
class QActionGroup;
class KUndoStack;

/**This is used to make the mapper perform different tasks
  *@author KMud Development Team
  */

class KMUDDY_EXPORT CMapManager : public QObject, public cActionBase
{
        Q_OBJECT
public:
  /** Constructor used to creat the map manager */
  CMapManager (QWidget *parent, KMuddyMapper *mapper, int sess);
  ~CMapManager();

  void eventStringHandler (QString event, int, QString &par1, const QString &);

  /** Used to get a list of the plugins */
  Q3PtrList<CMapPluginBase> *getPluginList();
  /**
   * Used to create a new view of the map
   * actually returns a CMapView*,  dynamic_cast if necessary
   */
  void openMapView();
  /**
   * Used to change the view to the given level
   */
  void displayLevel(CMapLevel *level, bool centerView);
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
  CMapLevel *createLevel(directionTyp dir);
  /** Used to create a new room */
  void createRoom(QPoint pos,CMapLevel *level);
  /** Used to create a new path, this method will display the path properties
   * dialog to obtain the porperties of the path to be created               */
  CMapPath *createPath(CMapRoom *srcRoom,CMapRoom *destRoom);
  /** Used to create a new path*/
  CMapPath *createPath(QPoint srcPos,CMapLevel *srcLevel,directionTyp srcDir,
                       QPoint destPos,CMapLevel *destLevel,directionTyp destDir);
  /** Used to create a new path*/
  CMapPath *createPath (CMapRoom *srcRoom,directionTyp srcDir,CMapRoom *destRoom,directionTyp destDir,bool undoable = true,bool twoWay = true);
  /** Used to create a new text label */
  void createText(QPoint pos,CMapLevel *level,QString str,QFont font,QColor col);
  /** Used to create a new text label */
  void createText(QPoint pos,CMapLevel *level,QString str="");

  bool isClean() const;
  CMapFileFilterBase *nativeFilter(bool isLoad = true);
  /** Used to load a map */
  void importMap(const QString& url,CMapFileFilterBase *filter);
  /** Used to save a map */
  void exportMap(const QString& url,CMapFileFilterBase *filter);

  void setDefaultOptions();
  /** Used to read the map options */
  void readOptions();
  /** Used to write the map options */
  void saveGlobalConfig();

  /** Used to alter the path properties */
  bool propertiesPath(CMapPath *path);
  /** Used to alter the room properties */
  bool propertiesRoom(CMapRoom *room);
  /** Used to alter the text properties */
  bool propertiesText(CMapText *text);

  /** This method is used to covert cords so that they snap to the grid */
  QPoint cordsSnapGrid(QPoint oldPos);

  /** Used to get a pointer to the map data */
  CMapData *getMapData() const;
  /** Pointer to the zone info that stores levels */
  CMapZone *getZone(bool noCreate = false);

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
  CMapView *getActiveView();
  /** Used to inform the various parts of the mapper that a element has changed */
  void changedElement(CMapElement *element);
  /** Used to inform the various parts of the mapper that a element has added */
  void addedElement(CMapElement *element);
  /** Used to walk the player in the mud to a given room */
  void walkPlayerTo(CMapRoom *toRoom);
  /** Used to inform to change the state of the navigation tools */
  void activeViewChanged(void);

  /** Used to cound the diffent elements of the map */
  void getCounts(int *levels,int *rooms,int *paths,int *labels);

  /** Get the opsite direction */
  directionTyp getOpsiteDirection(directionTyp dir);

  /** Used to add a command to the command history */
  void addCommand(CMapCommand *command);

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

  /** Obtain the command history */
  KUndoStack *getCommandHistory() { return commandHistory; };
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
  /** Find the first room in the map,if one can't be found then create one */
  CMapRoom *findFirstRoom(CMapRoom *exsitingRoom);
  /** This is used to tell the mapper if commands should be added to the history list */
  void setUndoActive(bool active);
  int getUndoActive(void);
  /** Used to erase the map. This will erase all elements and can't be undone */
  void eraseMap(void);
  void eraseZone(CMapZone *zone);

  CMapZoneManager *zoneManager() { return m_zoneManager; }

  /** Used to repaint all the views */
  void redrawAllViews(void);

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

  /** Return property panes from all the plugins */
  virtual QList<CMapPropertiesPaneBase *> createPropertyPanes(elementTyp type,CMapElement *element,QWidget *parent);

  QString defaultSavePath () const;

  KComponentData instance ();

public:
  /** A count of the number of levels created */
  unsigned int m_levelCount;
  /** A count of the number of zones created */
  unsigned int m_zoneCount;

public slots:
  /** Used to delete a element from the map */
  void deleteElement(CMapElement *element,bool delOpsite = true);
  /** Used to abort a speedwalk that is in progress */
  void slotAbortSpeedwalk(void);
  

private slots:
  void slotWalkPlayerAlongPath(void);
  
private:
  /** Used to delete a element from the map, should only be used by the deleteElementMethod() */
  void deleteElementWithoutGroup(CMapElement *element,bool delOpsite = true);
  /** This will setup the import/export file filters */
  void initFileFilters();
  /** Used to create the plugins */
  void initPlugins();

private:
  /** A list of import/export filters */
  Q3PtrList<CMapFileFilterBase> m_fileFilter;
  /** If this is true then commands are added to the history, otherwise they are not */
  bool m_commandsActive;
  /** The filter used to proces mud input/output */
  CMapFilter *filter;
  /** list of all the zones */
  CMapZoneManager *m_zoneManager;
  /** out session */
  int m_sessId;
  /** The element currenly being edited */
  CMapElement *elementEdit;
  /** The current tool */
  CMapToolBase *currentTool;
  /** A list of all loaded tools */
  Q3PtrList<CMapToolBase> toolList;
  /** The undo/redo history */
  KUndoStack *commandHistory;
  /** Points the command history currently being used */
  CMapCmdGroup *historyGroup;
  /** Used to store the amount of steps taken in the speedwalk */
  int speedwalkProgress;
  /** A pointer to the active map view */
  CMapView *activeView;
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

  /** Used to tell if speedwalk is active */
  bool speedwalkActive;
  /** The rooms that are to be speed walked */
  Q3PtrStack<QString> pathToWalk;

  KMuddyMapper *mapperPlugin;
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
};

#endif
