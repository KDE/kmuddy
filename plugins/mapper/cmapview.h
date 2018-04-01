/***************************************************************************
                               cmapview.h
                             -------------------
    begin                : Mon Mar 19 2001
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

#ifndef CMAPVIEW_H
#define CMAPVIEW_H

#include <qlayout.h>

#include <kxmlguiwindow.h>
#include <klocale.h>
#include <kmuddy_export.h>

class QLabel;
class QScrollArea;
class QPushButton;
class QAction;
class KToggleAction;
class KSelectAction;
class QActionGroup;

class CMapWidget;
class CMapManager;
class CMapElement;
class CMapLevel;
class CMapRoom;
class CMapZone;
class CMapViewStatusbar;
class HVBoxLayout;
class CMapClipboard;


/**This widget is used as a view of the map.
  *It should be create only from the map manager.
  *@author Kmud Developer Team
  */
class KMUDDY_EXPORT CMapView : public KXmlGuiWindow
{
   Q_OBJECT

public:
  CMapView(CMapManager *manager, QWidget *parent);
  virtual ~CMapView();

  void initGUI();

  CMapWidget *getMapWidget() { return mapWidget; }
  CMapManager *getManager() { return mapManager; }

  // FIXME: Follow/create mode needs to be saved/restored somewere
  /** Set the state of the follow combo box */
  void setFollowMode(bool follow);
  /** Get the state of the follow combo box */
  bool getFollowMode();
  /** Set the state of the create combo box */
  void setCreateMode(bool create);
  /** Get the state of the create combo box */
  bool getCreateMode();

  /** Used to let the map manager know if it should register the focus of this widget */
  virtual bool acceptFocus();

  /** Used to get the currently viewed zone */
  virtual CMapZone *getCurrentlyViewedZone(void);
  /** Used to get the currently viewed level */
  virtual CMapLevel *getCurrentlyViewedLevel(void);
  /** This method is called when the current position of the player changes */
  virtual void playerPositionChanged(CMapRoom *);
  /** Used to set the current level. This is for internal use */
  virtual void setLevel(CMapLevel *level);

  /** Tell this map view to display a different level. view wiil
  * center on the first room */
  void showPosition(CMapLevel *level,bool centerView=true);
  /** Tell this map view to display the level with a given room in it
  * @param room The room to center on
  * @param centerView used to tell center view
  */
  void showPosition(CMapRoom *room,bool centerView=true);
  /** Tell this map widget to display a different zone */
  virtual void showPosition(QPoint pos,CMapLevel *level,bool centerView=true);
  /** This method is used to set the element when that a popup menu is to be showed for */
  void setSelectedElement(CMapElement *element);
  /** This methid is used to set the position that a element was selected when the contex menu is shown */
  void setSelectedPos(QPoint pos);
  /** This method is used to get the element that a popup menu is being displayed for */
  CMapElement *getSelectedElement();

  /** This method is called when an element is added */
  virtual void addedElement(CMapElement *element);
  /** This method is called when an element is deleted */
  virtual void deletedElement(CMapLevel *deletedFromLevel);
  /** This method is called when an element is changed */
  virtual void changedElement(CMapElement *element);
  /** This method is called when a map level is changed */
  virtual void changedLevel(CMapLevel *level);
  /** This method is called when something else is changed, it should trigger a repaint */
  virtual void changed();
  /** Asks the widget to repaint itself */
  virtual void requestPaint();
  /** Get the max x cord of all elements */
  int getMaxX(void);
  /** Get the max y cord of all elements */
  int getMaxY(void);

  /** Used to find out if a element is visible in the view */
  virtual bool isElementVisible(CMapElement *element);
  /** Used to find out if a level is visible in the view */
  virtual bool isLevelVisible(CMapLevel *level);

  /* Used to calculate the correct size for the widget */
  void checkSize(QPoint pos);

  /** This is used ensure a location is visiable for views that scroll */
  virtual void ensureVisible(QPoint pos);

  /** This will pass cursor changes to the mapWidet */
  void setCursor ( const QCursor & cursor);

  void readOptions();

  QActionGroup *toolGroup () { return m_toolGroup; }

protected:
  /** Used to get the width of the widget */
  virtual int getWidth(void);
  /** Used to gt the height of the widget */
  virtual int getHeight(void);
  virtual void resizeEvent (QResizeEvent *) override;
  virtual void slotWidgetBeingClosed();
  virtual void slotDockWindowClose();
  virtual void closeEvent ( QCloseEvent *e) override;
  /** Used to set the view to active */
  virtual void focusInEvent(QFocusEvent *e) override;

private slots:
  void slotToolsGrid();
  void slotToolsLevelUp();
  void slotToolsLevelDown();
  void slotToolsLevelDelete();
  void slotToolsZoneCreate();
  void slotToolsZoneDelete();

  void slotViewUpperLevel();
  void slotViewLowerLevel();

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
  /** Used to make the path under the pointer two way */
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

signals:
  void closed ();

private:
  void initMenus();
  /** Used by slotToolsLevel(Up/Down) */
  void levelShift(bool up);
  virtual bool queryClose () override;

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


private:
  /** A pointer to the map manager */
  CMapManager *mapManager;
  /** A pointer to the current level for this map widget */
  CMapLevel *currentLevel;

  /** The element that is selected ( when the right mouse button menu is show for it) */
  CMapElement *m_selectedElement;
  /** The position that the mouse is in when the context menu is show for a element */
  QPoint m_selectedPos;

  /** This is the map clipboard used for copy and paste actions */
  CMapClipboard *m_clipboard;
  /** Main layout */
  HVBoxLayout *layout;
  /** A toggle button to tell the map view to follow the player. */
  QPushButton *cmdFollowMode;
  /** A toggle button to automatically create rooms as we walk. */
  QPushButton *cmdCreateMode;
  /** The scrolling area wrapping the map widget */
  QScrollArea *scroller;
  /** The Widget used to draw the map */
  CMapWidget *mapWidget;
  /** The status bar */
  CMapViewStatusbar *statusbar;
  /** The max size of all the components */
  QSize maxSize;

  //Actions
  KToggleAction *m_toolsGrid;
  QAction *m_toolsUpLevel;
  QAction *m_toolsDownLevel;
  QAction *m_toolsDeleteLevel;
  QAction *m_toolsCreateZone;
  QAction *m_toolsDeleteZone;

  KToggleAction *m_viewLowerLevel;
  KToggleAction *m_viewUpperLevel;

  KSelectAction *labelMenu;

  QActionGroup *m_toolGroup;


};

#endif

