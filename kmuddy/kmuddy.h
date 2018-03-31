/***************************************************************************
                          kmuddy.h  -  main class that handles interface
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pi Jun 14 12:37:51 CEST 2002
    copyright            : (C) 2002 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMUDDY_H
#define KMUDDY_H

#include "cactionbase.h"
#include "cactionmanager.h"

#include <kmainwindow.h>

class QMenu;
class QTimer;
class KHelpMenu;
class QMenuBar;
class KSystemTrayIcon;
class KToolBar;

class dlgConnect;
class dlgQuickConnect;
class dlgEditProfile;
class dlgGrabKey;
class dlgObjects;
class dlgStatistics;

class cActionManager;
class cTabWidget;

/** KMuddy is the base class of the project */
class KMuddy : public KMainWindow, public cActionBase
{
  Q_OBJECT 
public:
  /** destructor */
  ~KMuddy();

  static KMuddy *self();

  //these functions enable/disable menu/toolbar functions
  //they are called by cConnection
  
  /** disables options valid only when we're connected */
  void disableConnectedOptions ();
  /** enables all such options, advanced options are still disabled */
  void enableConnectedOptions ();
  /** disables advanced options - all that require a standard (not quick) connection,
  as they need to store settings and so on; this includes aliases, macros, triggers,
  auto-mapper, ... */
  void disableAdvancedOptions ();
  /** enables all advanced options */
  void enableAdvancedOptions ();
  /** enable Close tab option */
  void setClosedConn (bool canCT);
  /** disable Close tab option */
  void unsetClosedConn ();

  void focusChange (const QString &window, const QString &command);

  static bool isGoingDown () { return goingDown; };

  /** flashing of window caption */
  void setAllowGlobalNotify (bool notify);
  /** flashing of tabbar text */
  void setAllowLocalNotify (bool notify);
  /** flashing always or triggers only? */
  void setAlwaysNotify (bool notify);
  /** Enable system tray icon? */
  void setSysTrayEnabled (bool enabled);
  /** Enable passive popup notification? */
  void setPassivePopup (bool enabled);
  /** auto-connect? And where? */
  void setAutoConnect (const QString & ac);
  
  /** some trigger requested notification */
  void requestNotify (int sess);

  /** text of the reconnect message */
  QString reconnectText ();
  
  /** update alias groups, ... - called when switching session */
  void updateWindows ();
  
  /** called by cConnection::updateMenus() when switching tabs */
  void setMenuAliasesEnabled (bool val);
  /** called by cConnection::updateMenus() when switching tabs */
  void setMenuTriggersEnabled (bool val);
  /** called by cConnection::updateMenus() when switching tabs */
  void setMenuTimersEnabled (bool val);
  /** called by cConnection::updateMenus() when switching tabs */
  void setMenuShortcutsEnabled (bool val);

  void setGrabDialog (dlgGrabKey *dlg) { grabdlg = dlg; };

  KToolBar *buttonBar() { return buttonbar; }

public slots:
  void showGauges (bool val);
  void setFullScreen (bool val);
  void toggleShowMenu ();
protected slots:

  /** close current tab - only valid if that connection has been closed */
  void closeTab ();
  void showAndHandleConnectDialog ();
  void showAndHandleQuickConnectDialog ();
  /** called when user clicks Connect in dlgQuickConnect */
  void doQuickConnect ();
  /** called when user clicks Connect in dlgConnect */
  void doConnect ();
  /** called by doConnect and when auto-connecting on start-up */
  void doProfileConnect (const QString &name, bool sendNothing);
  void doDisconnect ();
  void doReconnect ();

  void pasteCommand ();
  void pasteInput ();

  void changeSession (int tab);

  /** lots of slots that handle menus; were handled by slots in objects,
  now we have cSession, so we have to handle them here */
  void addSelectionToClipboard ();
  void setParsing (bool value);
  void clearCommandQueue ();
  void showObjectsDialog ();
  void handleWindowsDialog ();
  void saveProfile ();
  void switchAliases (bool value);
  void switchTriggers (bool value);
  void switchTimers (bool value);
  void switchShortcuts (bool value);
  void showSettingsDialog ();
  void showConnPrefsDialog ();
  void configureTranscript ();
  void dumpBuffer ();
  void makeDecision ();
  void importProfile ();
  void exportProfile ();
  void lineUp ();
  void lineDown ();
  void pageUp ();
  void pageDown ();
  void aconUp ();
  void aconDown ();

  void prevTab ();
  void nextTab ();
  void switchTab1 () { switchTab(0); };
  void switchTab2 () { switchTab(1); };
  void switchTab3 () { switchTab(2); };
  void switchTab4 () { switchTab(3); };
  void switchTab5 () { switchTab(4); };
  void switchTab6 () { switchTab(5); };
  void switchTab7 () { switchTab(6); };
  void switchTab8 () { switchTab(7); };
  void switchTab9 () { switchTab(8); };
  void switchTab10 () { switchTab(9); };
protected:
  /** protected constructor */
  KMuddy ();
  static KMuddy *_self;

  virtual void eventNothingHandler (QString event, int session) override;
  virtual void eventIntHandler (QString event, int session, int, int) override;
  virtual void eventChunkHandler (QString event, int session, cTextChunk *) override;
  virtual void eventStringHandler (QString event, int session, QString &par1,
      const QString &par2) override;

  void prepareObjects ();
  void killObjects ();
  /** called when we try to close the window (Alt+F4 or the Close icon) */
  virtual bool queryClose () override;
  
  /** calls cSessionManager::self()->activeSession. Provided for convenience. */
  int activeSession ();
  
  /** event filter, used to grab macro keys */
  bool eventFilter (QObject *o, QEvent *e) override;

  /** save session properties */
  void saveProperties (KConfigGroup &config) override;
  /** read session properties */
  void readProperties (const KConfigGroup &config) override;

  dlgGrabKey *grabdlg;

  /** switch to tab <which> */
  void switchTab (int index);
  
  /** True when the app is about to terminate */
  static bool goingDown;
  
  /**menus*/
  QMenu *connectionMenu, *editMenu, *viewMenu, *profileMenu;
  QMenu *toolsMenu, *settingsMenu, *popup;
  KHelpMenu *helpMenu;

  /**System Tray Icon*/
  KSystemTrayIcon *sysIcon;

  //notification
  bool globalnotify, localnotify, alwaysnotify, systrayenabled, passivepopup;

  //auto-connect
  QString autoconnect;

  // the central area
  cTabWidget *central;

  // the button bar
  KToolBar *buttonbar;

  cActionManager *am;

  // some dialog boxes
  dlgConnect *cdlg;
  dlgQuickConnect *qdlg;
  dlgEditProfile *mdlg;
  dlgStatistics *statdlg;
  dlgObjects *objdlg;
};

#endif
