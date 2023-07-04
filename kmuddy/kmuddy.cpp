/***************************************************************************
                          kmuddy.cpp  -  main class that handles interface
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Jun 14 12:37:51 CEST 2002
    copyright            : (C) 2002-2007 by Tomas Mecir
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

#include <config-mxp.h>

#include "kmuddy.h"

#include "cactionmanager.h"
#include "cbuttonlist.h"
#include "ccmdparser.h"
#include "cconnection.h"
#include "cconsole.h"
#include "cgaugebar.h"
#include "cinputline.h"
#include "clistmanager.h"
#include "cmacromanager.h"
#include "cmenumanager.h"
#include "coutput.h"
#include "cpluginmanager.h"
#include "csession.h"
#include "csessionmanager.h"
#include "cshortcutlist.h"
#include "csoundplayer.h"
#include "ctabwidget.h"
#include "ctranscript.h"
#include "ctelnet.h"

#include "cglobalsettings.h"
#include "cmultilineinput.h"

#ifdef HAVE_MXP
#include "cmxpconsole.h"
#endif

#include "dialogs/dlgappsettings.h"
#include "dialogs/dlgconnect.h"
#include "dialogs/dlgquickconnect.h"
#include "dialogs/dlgeditprofile.h"
#include "dialogs/dlggrabkey.h"
#include "dialogs/dlgimportexportprofile.h"
#include "dialogs/dlgobjects.h"
#include "dialogs/dlgstatistics.h"

#include <stdlib.h>
#include <ctime>

#include <QAction>
#include <QDesktopWidget>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QTimer>

#include <kaboutapplicationdialog.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kinputdialog.h>
#include <KIconLoader>
#include <klocale.h>
#include <kmessagebox.h>
#include <kshortcut.h>
#include <kstatusbar.h>
#include <ktoggleaction.h>
#include <ktoolbar.h>
#include <kwindowsystem.h>
#include <kpassivepopup.h>
#include <KSystemTrayIcon>
#include <kdebug.h>

KMuddy *KMuddy::_self = nullptr;

//are the actions ready?
bool actionsReady;

bool KMuddy::goingDown = false;

KMuddy *KMuddy::self() {
  if (!_self)
    _self = new KMuddy;
  return _self;
}

KMuddy::KMuddy() : KMainWindow(nullptr), cActionBase ("kmuddy", 0), sysIcon(nullptr)
{
  //initialize random number generator (needed by cMSP)
  srand (time (nullptr));

  central = nullptr;
  actionsReady = false;
  globalnotify = localnotify = alwaysnotify = false;
  cdlg = nullptr;
  qdlg = nullptr;
  mdlg = nullptr;
  statdlg = nullptr;
  objdlg = nullptr;
  grabdlg = nullptr;

  _self = this;
  prepareObjects ();
}

KMuddy::~KMuddy()
{
  goingDown = true;
  killObjects ();
}

void KMuddy::eventNothingHandler (QString event, int session)
{
  if (event == "notify-request")
    requestNotify (session);

  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    setAllowGlobalNotify (gs->getBool ("global-notify"));
    setAllowLocalNotify (gs->getBool ("local-notify"));
    setAlwaysNotify (gs->getBool ("always-notify"));
    setSysTrayEnabled (gs->getBool ("systray-enabled"));
    setPassivePopup (gs->getBool ("passive-popup"));
    if (!systrayenabled && sysIcon) {
      // Remove the system tray icn
      delete sysIcon;
      sysIcon = nullptr;
    }
    else if (systrayenabled && !sysIcon){
      // Load the system tray icon
      sysIcon = new KSystemTrayIcon(QIcon::fromTheme ("kmuddy.png"), this);
      sysIcon->show ();
      QAction* quitAction = sysIcon->actionCollection()->action("file_quit");
      quitAction->disconnect ();
      connect (quitAction, SIGNAL (activated()), this, SLOT(close()));
    }
    setAutoConnect (gs->getString ("auto-connect"));
  }
  if (event == "disconnected") {
    // disconnected - hide some things if that session is active
    if (cActionManager::self()->activeSession() == session) {
      buttonbar->hide ();
      showGauges (false);
    }
  }
}

void KMuddy::eventIntHandler (QString event, int, int par, int)
{
  if (event == "session-activated") {
    // session activated - adjust the button bar accordingly
    cButtonList *bl = dynamic_cast<cButtonList *>(cListManager::self()->getList (par, "buttons"));
    if (!bl) {
      // this session has no buttonlist - nothing to do
      buttonbar->hide();
      return;
    }
    // update the button bar
    bl->updateButtons ();
  }
}

void KMuddy::eventChunkHandler (QString event, int session, cTextChunk *)
{
  if ((event == "displayed-line") || (event == "displayed-prompt"))
    if (alwaysnotify) requestNotify (session);
}

void KMuddy::eventStringHandler (QString event, int, QString &par1,
    const QString &par2)
{
  if (event == "focus-change")
    focusChange (par1, par2);
}

// some externs ... these are in macros.cpp
void registerInternalMacros ();
void unregisterInternalMacros ();

#define KEY_SHIFT Qt::ShiftModifier
#define KEY_CTRL Qt::ControlModifier
#define KEY_ALT Qt::AltModifier

void KMuddy::prepareObjects ()
{
  // set the main window pointer
  cActionManager::self()->setMainWindow (this);

  // plug-in manager
  cPluginManager::self();  // initialise the plugin manager
  registerInternalMacros ();

  // get action collection, creating it if needed
  // it must exist before the settings are loaded
  cActionManager::self()->createACol (this);
  KActionCollection *acol = cActionManager::self()->getACol ();
  // associate this window with the collection - needed for shortcuts to always work correctly
  acol->addAssociatedWidget (this);

  cGlobalSettings *globalsettings = cGlobalSettings::self();

  // initialise dock windows
  cMultiLineInput::self();
  statdlg = new dlgStatistics (this);
  statdlg->hide ();
  statdlg->setObjectName ("statistics");
  addDockWidget (Qt::RightDockWidgetArea, statdlg);
  statdlg->setFloating (true);
#ifdef HAVE_MXP
  cMXPConsole::self();
#endif

  // initialise sound objects
  new cSoundPlayer (true);   // wave player
  new cSoundPlayer (false);  // midi player

  // add application directory to icon search path
  KIconLoader::global()->addAppDir ("kmuddy");

  // action manager
  am = cActionManager::self();
  addEventHandler ("displayed-line", 100, PT_TEXTCHUNK);
  addEventHandler ("displayed-prompt", 100, PT_TEXTCHUNK);
  addEventHandler ("notify-request", 50, PT_NOTHING);
  addEventHandler ("global-settings-changed", 50, PT_NOTHING);
  addEventHandler ("focus-change", 50, PT_STRING);
  addEventHandler ("session-activated", 50, PT_INT);
  addEventHandler ("disconnected", 50, PT_NOTHING);

  // now we create the menubar
  cMenuManager::self()->setMenuBar (menuBar());

  //Connection
  QAction *stdconnect = new QAction (this);
  stdconnect->setText (i18n ("&Connect..."));
  stdconnect->setIcon (QIcon::fromTheme ("network-connect"));
  stdconnect->setShortcut (KEY_CTRL+Qt::Key_N);
  connect (stdconnect, SIGNAL (triggered()), this, SLOT (showAndHandleConnectDialog()));
  acol->addAction ("Connect", stdconnect);

  QAction *qconnect = new QAction (this);
  qconnect->setText (i18n ("&QuickConnect..."));
  qconnect->setIcon (QIcon::fromTheme ("network-connect"));
  qconnect->setShortcut (KEY_CTRL+Qt::Key_O);
  connect (qconnect, SIGNAL (triggered()), this, SLOT (showAndHandleQuickConnectDialog()));
  acol->addAction ("QuickConnect", qconnect);
  QAction *dodisconnect = new QAction (this);
  dodisconnect->setText (i18n ("&Disconnect"));
  dodisconnect->setIcon (QIcon::fromTheme ("network-disconnect"));
  dodisconnect->setShortcut (KEY_CTRL+Qt::Key_D);
  connect (dodisconnect, SIGNAL (triggered()), this, SLOT (doDisconnect()));
  acol->addAction ("Disconnect", dodisconnect);
  QAction *reconnect = new QAction (this);
  reconnect->setText (i18n ("&Reconnect"));
  reconnect->setShortcut (KEY_CTRL+Qt::Key_R);
  connect (reconnect, SIGNAL (triggered()), this, SLOT (doReconnect()));
  acol->addAction ("Reconnect", reconnect);
  QAction *closetab = new QAction (this);
  closetab->setText (i18n ("Close &tab"));
  closetab->setShortcut (KEY_CTRL+Qt::Key_W);
  connect (closetab, SIGNAL (triggered()), this, SLOT (closeTab()));
  acol->addAction ("CloseTab", closetab);
  closetab->setEnabled (false);
  reconnect->setEnabled (false);

  //Edit
  KActionMenu *pastemenu = new KActionMenu (this);
  pastemenu->setText (i18n ("Paste &As"));
  pastemenu->setIcon (QIcon::fromTheme ("edit-paste"));
  acol->addAction ("PasteMenu", pastemenu);
  //"Paste As" items
  QAction *pastecommand = new QAction (this);
  pastecommand->setText (i18n ("&Command"));
  connect (pastecommand, SIGNAL (triggered()), this, SLOT (pasteCommand()));
  acol->addAction ("PasteCommand", pastecommand);
  QAction *pasteinput = new QAction (this);
  pasteinput->setText (i18n ("&Input"));
  connect (pasteinput, SIGNAL (triggered()), this, SLOT (pasteInput()));
  acol->addAction ("PasteInput", pasteinput);
  pastemenu->addAction(pastecommand);
  pastemenu->addAction(pasteinput);

  QAction *clipcopy = new QAction (this);
  clipcopy->setText (i18n ("Copy &Selection"));
  clipcopy->setIcon (QIcon::fromTheme ("edit-copy"));
  clipcopy->setShortcut (KEY_CTRL+Qt::Key_C);
  connect (clipcopy, SIGNAL (triggered()), this, SLOT (addSelectionToClipboard()));
  acol->addAction ("ClipboardCopy", clipcopy);
  KToggleAction *parsing = new KToggleAction (this);
  parsing->setText (i18n ("Enable command &parsing"));
  parsing->setIcon (QIcon::fromTheme ("go-jump"));
  parsing->setShortcut (KEY_CTRL+KEY_SHIFT+Qt::Key_P);
  connect (parsing, SIGNAL (triggered (bool)), this, SLOT (setParsing (bool)));
  parsing->setChecked (true);  //enabled by default
  acol->addAction ("EnableParsing", parsing);
  QAction *cancelpending = new QAction (this);
  cancelpending->setText (i18n ("Ca&ncel pending commands"));
  cancelpending->setIcon (QIcon::fromTheme ("list-remove"));
  connect (cancelpending, SIGNAL (triggered()), this, SLOT (clearCommandQueue()));
  acol->addAction ("CancelPending", cancelpending);

  //View
  QAction *showmultiline = cMultiLineInput::self()->dialog()->toggleViewAction ();
  showmultiline->setIcon (QIcon::fromTheme ("format-justify-left"));
  showmultiline->setShortcut (KEY_CTRL+KEY_SHIFT+Qt::Key_M);
  acol->addAction ("ShowMultiLine", showmultiline);
  KToggleAction *showgaugebar = new KToggleAction (this);
  showgaugebar->setText (i18n ("Show &gaugebar"));
  connect (showgaugebar, SIGNAL (toggled (bool)), this, SLOT (showGauges (bool)));
  showgaugebar->setChecked (false);
  acol->addAction ("ShowGaugeBar", showgaugebar);
  QAction *showstatistics = statdlg->toggleViewAction ();
  acol->addAction ("ShowStatistics", showstatistics);
#ifdef HAVE_MXP
  QAction *showmxpconsole = cMXPConsole::self()->dialog()->toggleViewAction ();
  acol->addAction ("ShowMXPConsole", showmxpconsole);
#endif

  //Profile
  QAction *objects = new QAction (this);
  objects->setText (i18n ("&Object Editor..."));
  objects->setShortcut (KEY_CTRL+KEY_ALT+Qt::Key_O);
  connect (objects, SIGNAL (triggered()), this, SLOT (showObjectsDialog()));
  acol->addAction ("Objects", objects);
  QAction *windows = new QAction (this);
  windows->setText (i18n ("&Output windows..."));
  connect (windows, SIGNAL (triggered()), this, SLOT (handleWindowsDialog()));
  acol->addAction ("OutputWindows", windows);
  KToggleAction *switchaliases = new KToggleAction (this);
  switchaliases->setText (i18n ("A&liases enabled"));
  switchaliases->setShortcut (KEY_CTRL+KEY_SHIFT+Qt::Key_A);
  connect (switchaliases, SIGNAL (triggered (bool)), this, SLOT (switchAliases (bool)));
  switchaliases->setChecked (true);
  acol->addAction ("EnableAliases", switchaliases);
  KToggleAction *switchtriggers = new KToggleAction (this);
  switchtriggers->setText (i18n ("Tri&ggers enabled"));
  switchtriggers->setShortcut (KEY_CTRL+KEY_SHIFT+Qt::Key_T);
  connect (switchtriggers, SIGNAL (triggered (bool)), this, SLOT (switchTriggers (bool)));
  switchtriggers->setChecked (true);
  acol->addAction ("EnableTriggers", switchtriggers);
  KToggleAction *switchtimers = new KToggleAction (this);
  switchtimers->setText (i18n ("Tim&ers enabled"));
  connect (switchtimers, SIGNAL (triggered (bool)), this, SLOT (switchTimers (bool)));
  switchtimers->setChecked (true);
  acol->addAction ("EnableTimers", switchtimers);
  KToggleAction *switchshortcuts = new KToggleAction (this);
  switchshortcuts->setText (i18n ("Macro &keys enabled"));
  connect (switchshortcuts, SIGNAL (triggered (bool)), this, SLOT (switchShortcuts (bool)));
  switchshortcuts->setChecked (true);
  acol->addAction ("EnableMacroKeys", switchshortcuts);
  QAction *connprefs = new QAction (this);
  connprefs->setText (i18n ("&MUD Preferences..."));
  connprefs->setIcon (QIcon::fromTheme ("user-identity"));
  connect (connprefs, SIGNAL (triggered()), this, SLOT (showConnPrefsDialog()));
  acol->addAction ("ConnPrefs", connprefs);
  QAction *save = new QAction (this);
  save->setText (i18n ("&Save Profile"));
  save->setShortcut (KEY_CTRL+Qt::Key_S);
  connect (save, SIGNAL (triggered()), this, SLOT (saveProfile()));
  acol->addAction ("SaveProfile", save);

  //Tools
  QAction *sesstranscript = new QAction (this);
  sesstranscript->setText (i18n ("&Session transcript..."));
  sesstranscript->setIcon (QIcon::fromTheme ("utilities-log-viewer"));
  connect (sesstranscript, SIGNAL (triggered()), this, SLOT (configureTranscript()));
  acol->addAction ("Transcript", sesstranscript);
  QAction *dumpbuffer = new QAction (this);
  dumpbuffer->setText (i18n ("Dump output &buffer..."));
  connect (dumpbuffer, SIGNAL (triggered()), this, SLOT (dumpBuffer()));
  acol->addAction ("DumpBuffer", dumpbuffer);
  QAction *decide = new QAction (this);
  decide->setText (i18n ("&Decision assistant"));
  connect (decide, SIGNAL (triggered()), this, SLOT (makeDecision()));
  acol->addAction ("Decision", decide);
/*  QAction *importprofile = new QAction (this);
  importprofile->setText (i18n ("&Import profile..."));
  connect (importprofile, SIGNAL (triggered()), this, SLOT (importProfile()));
  acol->addAction ("ImportProf", importprofile);
  QAction *exportprofile = new QAction (this);
  exportprofile->setText (i18n ("&Export profile..."));
  connect (exportprofile, SIGNAL (triggered()), this, SLOT (exportProfile()));
  acol->addAction ("ExportProf", exportprofile); */

  //Settings
  QAction *appconfig = new QAction (this);
  appconfig->setText (i18n ("&Global settings..."));
  appconfig->setIcon (QIcon::fromTheme ("configure"));
  connect (appconfig, SIGNAL (triggered()), this, SLOT (showSettingsDialog()));
  acol->addAction ("Global settings", appconfig);
  QAction *pluginsconfig = new QAction (this);
  pluginsconfig->setText (i18n ("&Plugins..."));
  connect (pluginsconfig, SIGNAL(triggered()), cPluginManager::self(), SLOT(showPluginsDialog()));
  acol->addAction ("Plugins", pluginsconfig);
  QAction *showmenubar = acol->addAction (KStandardAction::ShowMenubar, "ShowMenuBar", this, SLOT (toggleShowMenu()));
  KToggleAction *fullscreenmode = new KToggleAction (this);
  fullscreenmode->setText (i18n ("F&ull screen mode"));
  fullscreenmode->setIcon (QIcon::fromTheme ("view-fullscreen"));
  fullscreenmode->setShortcut (KEY_CTRL+KEY_SHIFT+Qt::Key_F);
  connect (fullscreenmode, SIGNAL (toggled (bool)), this,
           SLOT (setFullScreen(bool)));
  acol->addAction ("SetFullScreen", fullscreenmode);

  //Help

  //other actions
  QAction *lineup = new QAction (this);
  lineup->setText (i18n ("Shift line up"));
  lineup->setShortcut (KEY_SHIFT+Qt::Key_Up);
  connect (lineup, SIGNAL (triggered()), this, SLOT (lineUp()));
  acol->addAction ("LineUp", lineup);
  QAction *linedown = new QAction (this);
  linedown->setText (i18n ("Shift line down"));
  linedown->setShortcut (KEY_SHIFT+Qt::Key_Down);
  connect (linedown, SIGNAL (triggered()), this, SLOT (lineDown()));
  acol->addAction ("LineDown", linedown);
  QAction *pageup = new QAction (this);
  pageup->setText (i18n ("Shift page up"));
  pageup->setShortcut (KEY_SHIFT+Qt::Key_PageUp);
  connect (pageup, SIGNAL (triggered()), this, SLOT (pageUp()));
  acol->addAction ("PageUp", pageup);
  QAction *pagedown = new QAction (this);
  pagedown->setText (i18n ("Shift page down"));
  pagedown->setShortcut (KEY_SHIFT+Qt::Key_PageDown);
  connect (pagedown, SIGNAL (triggered()), this, SLOT (pageDown()));
  acol->addAction ("PageDown", pagedown);
  QAction *aconup = new QAction (this);
  aconup->setText (i18n ("Split-screen up"));
  aconup->setShortcut (KEY_CTRL+KEY_ALT+Qt::Key_Up);
  connect (aconup, SIGNAL (triggered()), this, SLOT (aconUp()));
  acol->addAction ("AconUp", aconup);
  QAction *acondown = new QAction (this);
  acondown->setText (i18n ("Split-screen down"));
  acondown->setShortcut (KEY_CTRL+KEY_ALT+Qt::Key_Down);
  connect (acondown, SIGNAL (triggered()), this, SLOT (aconDown()));
  acol->addAction ("AconDown", acondown);
  QAction *prevtab = new QAction (this);
  prevtab->setText (i18n ("Previous tab"));
  prevtab->setShortcut (KEY_ALT+Qt::Key_PageUp);
  connect (prevtab, SIGNAL (triggered()), this, SLOT (prevTab()));
  acol->addAction ("PreviousTab", prevtab);
  QAction *nexttab = new QAction (this);
  nexttab->setText (i18n ("Next tab"));
  nexttab->setShortcut (KEY_ALT+Qt::Key_PageDown);
  connect (nexttab, SIGNAL (triggered()), this, SLOT (nextTab()));
  acol->addAction ("NextTab", nexttab);
  QAction *tabnum[10];
  tabnum[0] = new QAction (this);
  tabnum[0]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "1"));
  tabnum[0]->setShortcut (KEY_ALT+Qt::Key_1);
  connect (tabnum[0], SIGNAL (triggered()), this, SLOT (switchTab1()));
  acol->addAction ("SwitchTab1", tabnum[0]);
  tabnum[1] = new QAction (this);
  tabnum[1]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "2"));
  tabnum[1]->setShortcut (KEY_ALT+Qt::Key_2);
  connect (tabnum[1], SIGNAL (triggered()), this, SLOT (switchTab2()));
  acol->addAction ("SwitchTab2", tabnum[1]);
  tabnum[2] = new QAction (this);
  tabnum[2]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "3"));
  tabnum[2]->setShortcut (KEY_ALT+Qt::Key_3);
  connect (tabnum[2], SIGNAL (triggered()), this, SLOT (switchTab3()));
  acol->addAction ("SwitchTab3", tabnum[2]);
  tabnum[3] = new QAction (this);
  tabnum[3]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "4"));
  tabnum[3]->setShortcut (KEY_ALT+Qt::Key_4);
  connect (tabnum[3], SIGNAL (triggered()), this, SLOT (switchTab4()));
  acol->addAction ("SwitchTab4", tabnum[3]);
  tabnum[4] = new QAction (this);
  tabnum[4]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "5"));
  tabnum[4]->setShortcut (KEY_ALT+Qt::Key_5);
  connect (tabnum[4], SIGNAL (triggered()), this, SLOT (switchTab5()));
  acol->addAction ("SwitchTab5", tabnum[4]);
  tabnum[5] = new QAction (this);
  tabnum[5]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "6"));
  tabnum[5]->setShortcut (KEY_ALT+Qt::Key_6);
  connect (tabnum[5], SIGNAL (triggered()), this, SLOT (switchTab6()));
  acol->addAction ("SwitchTab6", tabnum[5]);
  tabnum[6] = new QAction (this);
  tabnum[6]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "7"));
  tabnum[6]->setShortcut (KEY_ALT+Qt::Key_7);
  connect (tabnum[6], SIGNAL (triggered()), this, SLOT (switchTab7()));
  acol->addAction ("SwitchTab7", tabnum[6]);
  tabnum[7] = new QAction (this);
  tabnum[7]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "8"));
  tabnum[7]->setShortcut (KEY_ALT+Qt::Key_8);
  connect (tabnum[7], SIGNAL (triggered()), this, SLOT (switchTab8()));
  acol->addAction ("SwitchTab8", tabnum[7]);
  tabnum[8] = new QAction (this);
  tabnum[8]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "9"));
  tabnum[8]->setShortcut (KEY_ALT+Qt::Key_9);
  connect (tabnum[8], SIGNAL (triggered()), this, SLOT (switchTab9()));
  acol->addAction ("SwitchTab9", tabnum[8]);
  tabnum[9] = new QAction (this);
  tabnum[9]->setText (i18nc ("%1 - tab number", "Switch to tab %1", "10"));
  tabnum[9]->setShortcut (KEY_ALT+Qt::Key_0);
  connect (tabnum[9], SIGNAL (triggered()), this, SLOT (switchTab10()));
  acol->addAction ("SwitchTab10", tabnum[9]);

  actionsReady = true;

  cMenuManager *menu = cMenuManager::self();

  //create all the menus
  connectionMenu = new QMenu (this);
  editMenu = new QMenu (this);
  viewMenu = new QMenu (this);
  profileMenu = new QMenu (this);
  toolsMenu = new QMenu (this);
  settingsMenu = new QMenu (this);
  helpMenu = new KHelpMenu (this, QString(), false);
  
  menu->addMenuPosition ("menu-base");
  menu->addMenuPosition ("menu-plugin");
  menu->addMenuPosition ("menu-settings");
  menu->addMenuPosition ("menu-help");

  menu->addMenu (connectionMenu, i18n ("&Connection"), "menu-base");
  menu->addMenu (editMenu, i18n ("&Edit"), "menu-base");
  menu->addMenu (viewMenu, i18n ("&View"), "menu-base");
  menu->addMenu (profileMenu, i18n ("&Profile"), "menu-base");
  menu->addMenu (toolsMenu, i18n ("&Tools"), "menu-base");
  menu->addMenu (settingsMenu, i18n ("&Settings"), "menu-settings");
  menu->addMenu (helpMenu->menu(), i18n ("&Help"), "menu-help");

  // create item slots and add entries to them

  //menu Connection
  menu->addItemPosition ("connection-slot1", connectionMenu);
  menu->addItemPosition ("connection-slot2", connectionMenu);
  menu->addItemPosition ("connection-quit", connectionMenu);
  
  menu->plug (stdconnect, "connection-slot1");
  menu->plug (qconnect, "connection-slot1");
  menu->plug (dodisconnect, "connection-slot1");
  menu->plug (reconnect, "connection-slot2");
  menu->plug (closetab, "connection-slot2");
  menu->plug (KStandardAction::quit(this, SLOT(close()), nullptr), "connection-quit");

  //menu Edit
  menu->addItemPosition ("edit-slot1", editMenu);
  menu->addItemPosition ("edit-slot2", editMenu);
  menu->addItemPosition ("edit-slot3", editMenu);
  menu->plug (clipcopy, "edit-slot1");
  menu->plug (pastemenu, "edit-slot1");
  menu->plug (parsing, "edit-slot2");
  menu->plug (cancelpending, "edit-slot3");
  
  //menu View
  menu->addItemPosition ("view-global", viewMenu);
  menu->addItemPosition ("view-profile", viewMenu);
  
  menu->plug (showmultiline, "view-global");
  menu->plug (showstatistics, "view-global");
  menu->plug (showgaugebar, "view-profile");
#ifdef HAVE_MXP
  menu->plug (showmxpconsole, "view-profile");
#endif

  //menu Profile
  menu->addItemPosition ("profile-objects", profileMenu);
  menu->addItemPosition ("profile-prefs", profileMenu);
  menu->addItemPosition ("profile-switches", profileMenu);
  menu->addItemPosition ("profile-other", profileMenu);

  menu->plug (objects, "profile-objects");
  menu->plug (windows, "profile-objects");
  menu->plug (connprefs, "profile-prefs");
  menu->plug (switchaliases, "profile-switches");
  menu->plug (switchtriggers, "profile-switches");
  menu->plug (switchtimers, "profile-switches");
  menu->plug (switchshortcuts, "profile-switches");
  menu->plug (save, "profile-other");

  //menu Tools
  menu->addItemPosition ("tools-slot1", toolsMenu);
  menu->addItemPosition ("tools-slot2", toolsMenu);
  menu->addItemPosition ("tools-slot3", toolsMenu);
  
  menu->plug (sesstranscript, "tools-slot1");
  menu->plug (dumpbuffer, "tools-slot1");
  menu->plug (decide, "tools-slot2");
//  menu->plug (importprofile, "tools-slot3");
//  menu->plug (exportprofile, "tools-slot3");

  //menu Settings
  menu->addItemPosition ("settings-slot1", settingsMenu);
  menu->addItemPosition ("settings-slot2", settingsMenu);
  menu->addItemPosition ("settings-slot3", settingsMenu);
  
  menu->plug (showmenubar, "settings-slot1");
  menu->plug (fullscreenmode, "settings-slot2");
  menu->plug (appconfig, "settings-slot3");
  menu->plug (pluginsconfig, "settings-slot3");

  //menu Help

  // global actions are tied to the main window
  addAction (lineup);
  addAction (linedown);
  addAction (pageup);
  addAction (pagedown);
  addAction (aconup);
  addAction (acondown);
  addAction (prevtab);
  addAction (nexttab);
  for (int i = 0; i < 10; ++i) addAction (tabnum[i]);

  buttonbar = new KToolBar ("buttonbar", this, Qt::TopToolBarArea);
  buttonbar->setWindowTitle ("Button toolbar");
  buttonbar->hide ();

  central = new cTabWidget (this);
  central->setMinimumSize (400, 300);
  central->setTabShape (QTabWidget::Triangular);
  connect (central, SIGNAL (currentChanged (int)), this, SLOT (changeSession (int)));
  cSessionManager::self()->setMainWidget (central);

  //prepare the new session
  cSessionManager::self()->addSession (false);

  //set up the window
  setCentralWidget (central);

  KApplication::kApplication()->processEvents ();

  //read saved keys for acol object
  acol->setConfigGroup ("Shortcuts");
  acol->readSettings ();

  // load all the plug-ins
  cPluginManager::self()->loadAll ();

  //we're not connected on startup, so let's disable some options...
  disableConnectedOptions ();
  disableAdvancedOptions ();

  //apply global settings
  globalsettings->notifyChange ();

  //look if there's some auto-connect profile set
  if (!autoconnect.isEmpty())
    doProfileConnect (autoconnect, false);

  //let the window remember its position!
  setAutoSaveSettings ();

  //make sure that menu bar is visible each time kmuddy is started
  if (!menuBar()->isVisible())
    toggleShowMenu ();
  
}

void KMuddy::pasteCommand ()
{
  //Should we allow it to addCommand even with no global clipboard input?
  //If we do, sends a newline (no big deal, may even be a 'feature')
  //All depends on how we want this set up; atm not allowing empty clipboard

  if ((KApplication::kApplication())->clipboard()->text().isNull())
    return;

  QString txt =
    (KApplication::kApplication())->clipboard()->text(QClipboard::Clipboard);
  am->invokeEvent ("send-command", activeSession(), txt);
}

void KMuddy::pasteInput ()
{
  cInputLine *inputline = dynamic_cast<cInputLine *>(am->object ("inputline", activeSession()));
  inputline->paste ();
}

void KMuddy::killObjects ()
{
  //destroy objects and set pointers to 0 - needed so that we can prevent
  //some crashes if some object calls a function of an object that has already
  //been destroyed; this gives him a chance to verify its state...

  //menubar and KActions are not deleted...

  //session objects are NOT deleted here - doing so causes kmuddy to crash

  removeEventHandler ("displayed-line");
  removeEventHandler ("displayed-prompt");
  removeEventHandler ("notify-request");
  removeEventHandler ("global-settings-changed");
  removeEventHandler ("focus-change");
  removeEventHandler ("session-activated");
  removeEventHandler ("disconnected");

  delete cGlobalSettings::self();

  delete cPluginManager::self();
  unregisterInternalMacros ();
  delete cMacroManager::self();

  //also delete some dialogs...
  delete statdlg;
  delete objdlg;
  delete qdlg;

  delete sysIcon;
}

void KMuddy::saveProperties (KConfigGroup & /*config*/)
{
  //nothing here for now, maybe I'll include re-connecting later?
}

void KMuddy::readProperties (const KConfigGroup & /*config*/)
{
  //nothing here now now, maybe I'll include re-connecting later?
}

void KMuddy::disableConnectedOptions ()
//may contain further commands...
{
  if (!actionsReady) return;
  KActionCollection *acol = am->getACol ();
  QAction *dodisconnect = acol->action ("Disconnect");
  QAction *sesstranscript = acol->action ("Transcript");
  QAction *dumpbuffer = acol->action ("DumpBuffer");

  dodisconnect->setEnabled (false);
  sesstranscript->setEnabled (false);
  dumpbuffer->setEnabled (false);

  disableAdvancedOptions ();
}

void KMuddy::enableConnectedOptions ()
//may contain further commands...
{
  if (!actionsReady) return;
  KActionCollection *acol = am->getACol ();
  QAction *dodisconnect = acol->action ("Disconnect");
  QAction *reconnect = acol->action ("Reconnect");
  QAction *sesstranscript = acol->action ("Transcript");
  QAction *dumpbuffer = acol->action ("DumpBuffer");
  
  if (!(dodisconnect->isEnabled()))
    dodisconnect->setEnabled (true);
  if (!(sesstranscript->isEnabled()))
    sesstranscript->setEnabled (true);
  if (!(dumpbuffer->isEnabled()))
    dumpbuffer->setEnabled (true);
  reconnect->setEnabled (false);
}

void KMuddy::disableAdvancedOptions ()
//may contain further commands...
{
  if (!actionsReady) return;
  KActionCollection *acol = am->getACol ();
 
  acol->action ("ShowGaugeBar")->setEnabled (false);
  profileMenu->setEnabled (false);

  //!!! must also disable all profile-related actions!!! (otherwise
  //they're still accessible with hotkeys)
  acol->action("OutputWindows")->setEnabled (false);
  acol->action("EnableAliases")->setEnabled (false);
  acol->action("EnableTriggers")->setEnabled (false);
  acol->action("EnableTimers")->setEnabled (false);
  acol->action("EnableMacroKeys")->setEnabled (false);
  acol->action("ConnPrefs")->setEnabled (false);
}

void KMuddy::enableAdvancedOptions ()
//may contain further commands...
{
  if (!actionsReady) return;
  KActionCollection *acol = am->getACol ();
  
  acol->action ("ShowGaugeBar")->setEnabled (true);
  profileMenu->setEnabled (true);

  acol->action("OutputWindows")->setEnabled (true);
  acol->action("EnableAliases")->setEnabled (true);
  acol->action("EnableTriggers")->setEnabled (true);
  acol->action("EnableTimers")->setEnabled (true);
  acol->action("EnableMacroKeys")->setEnabled (true);
  acol->action("ConnPrefs")->setEnabled (true);
}

void KMuddy::setClosedConn (bool canCT)
{
  if (!actionsReady) return;
  
  KActionCollection *acol = am->getACol ();
  acol->action ("CloseTab")->setEnabled (canCT);
  acol->action ("Reconnect")->setEnabled (true);
}

void KMuddy::unsetClosedConn ()
{
  if (!actionsReady) return;
  
  KActionCollection *acol = am->getACol ();
  acol->action ("CloseTab")->setEnabled (false);
  acol->action ("Reconnect")->setEnabled (false);
}

void KMuddy::closeTab ()
{
  //close that session...
  cConnection *conn = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));

  if (conn->isConnected ())
    doDisconnect();
  if (!conn->isConnected())
    cSessionManager::self()->removeSession (activeSession(), false);
}

void KMuddy::setFullScreen (bool val)
{
  KActionCollection *acol = am->getACol ();
  QAction *showmenubar = acol->action ("ShowMenuBar");
  if (val) {
    showFullScreen();
    if (menuBar()->isVisible())
      toggleShowMenu();
    showmenubar->setChecked( ! menuBar()->isHidden() );
  }
  else
    if (isFullScreen()) {
      showNormal();
      if (!menuBar()->isVisible())
        toggleShowMenu();
      showmenubar->setChecked( ! menuBar()->isHidden() );
    }
}

void KMuddy::toggleShowMenu()
{
  if (menuBar()->isVisible())
    menuBar()->hide();
  else
    menuBar()->show();
}

void KMuddy::showGauges (bool val)
{
  cGaugeBar *gauges = dynamic_cast<cGaugeBar *>(am->object ("gaugebar", activeSession()));

  bool shown = gauges->isVisible ();
  if (val)
    gauges->show ();
  else
    gauges->hide ();

  //keep action in sync (this function can be explicitly called)
  KActionCollection *acol = am->getACol ();
  acol->action ("ShowGaugeBar")->setChecked (val);

  if (val != shown) {
    //showing/hiding toolbar shifts view - fix it!
    //!!! THIS IS FAR FROM BEING IDEAL !!!
    list<int>::iterator it;
    list<int> sess = am->sessionList ();
    for (it = sess.begin(); it != sess.end(); ++it) {
      cConsole *console = (dynamic_cast<cOutput *>(am->object("output", *it)))->console();
      console->pageDown ();
    }
  }
}

// TODO: this should get moved to session manager and command processor
// session manager would return session ID with a given prefix, command
// processor would send command ...
void KMuddy::focusChange (const QString &window, const QString &command)
{
  QString sessName;
  int id = -1;

  list<int>::iterator it;
  list<int> sess = am->sessionList ();
  for (it = sess.begin(); it != sess.end(); ++it) {
    sessName = am->callAction ("session", "name", *it);
    if (sessName.startsWith(window)) {
      id = *it;
      //breaking here means that if more than one session matching
      //window name is found, that sessionName will use the first
      //ie - sessions: 'abc' and 'abc123' - whichever has the lowest
      //sessionid will be the session the command is sent to if only
      //'abc' is passed as the WINDOW
      break;
    }
  }
  if (id <= 0)
    return; //return without sending command

  // send the command to the necessary session - it will be sent as-is
  am->invokeEvent ("send-command", id, command);
}

QString KMuddy::reconnectText ()
{
  KActionCollection *acol = am->getACol ();
  QAction *reconnect = acol->action ("Reconnect");
  return i18n ("To reconnect, press %1 or use Connection / Reconnect.", reconnect->shortcut().toString());;
}

void KMuddy::updateWindows ()
{
  // TODO: the windows could react on events and update themselves ...
  // then we won't need this at all ...

  // update the statistics window
  if (statdlg != nullptr)
    statdlg->update ();
}

void KMuddy::changeSession (int tab)
{
  int sess = cSessionManager::self()->getSessionByTab (tab);
  cSessionManager::self()->changeSession (sess);
}

int KMuddy::activeSession ()
{
  return am->activeSession();
}

void KMuddy::setAutoConnect (const QString &ac)
{
  autoconnect = ac;
}

void KMuddy::prevTab ()
{
  int idx = central->currentIndex ();
  idx--;
  if (idx < 0)
    idx = central->count() - 1;
  central->setCurrentIndex (idx);
}

void KMuddy::nextTab ()
{
  int idx = central->currentIndex ();
  idx++;
  if (idx >= central->count())
    idx = 0;
  central->setCurrentIndex (idx);
}

void KMuddy::switchTab (int index)
{
  central->setCurrentIndex (index);
}

void KMuddy::setAllowGlobalNotify (bool notify)
{
  globalnotify = notify;
}


void KMuddy::setAllowLocalNotify (bool notify)
{
  localnotify = notify;
}

void KMuddy::setAlwaysNotify (bool notify)
{
  alwaysnotify = notify;
}

void KMuddy::setSysTrayEnabled (bool enabled)
{
  systrayenabled = enabled;
}

void KMuddy::setPassivePopup (bool enabled)
{
  passivepopup = enabled;
}

void KMuddy::requestNotify (int sess)
{
  //global notify
  if (globalnotify && (!isActiveWindow ())){
    // ask the window manager to draw attention to the window
    KWindowSystem::demandAttention (topLevelWidget()->winId());
    if (systrayenabled && sysIcon && passivepopup){
      QPixmap px;
      px.load("kmuddy.png");
      KPassivePopup::message (KPassivePopup::Balloon, "KMuddy", "Activity in the KMuddy Window", px, sysIcon, 1200);
    }
  }

  //local notify
  if (localnotify && (activeSession() != sess))
    cSessionManager::self()->setNotifyFlag (sess);
}

void KMuddy::setMenuAliasesEnabled (bool val)
{
  KActionCollection *acol = am->getACol ();
  acol->action ("EnableAliases")->setChecked (val);
}

void KMuddy::setMenuTriggersEnabled (bool val)
{
  KActionCollection *acol = am->getACol ();
  acol->action ("EnableTriggers")->setChecked (val);
}

void KMuddy::setMenuTimersEnabled (bool val)
{
  KActionCollection *acol = am->getACol ();
  acol->action ("EnableTimers")->setChecked (val);
}

void KMuddy::setMenuShortcutsEnabled (bool val)
{
  KActionCollection *acol = am->getACol ();
  acol->action ("EnableMacroKeys")->setChecked (val);
}

void KMuddy::doDisconnect ()
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->handleDisconnect ();
}

void KMuddy::doReconnect ()
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->reconnect ();

  // update alias/trigger groups and similar things ...
  updateWindows ();
}

//THESE METHODS WERE ORIGINALLY IN cConnection; now they're here because
//of multi-tab support

void KMuddy::doQuickConnect ()
{
  //hide the dialog so that it doesn't look like we've crashed
  qdlg->hide ();

  //create a new session object and related stuff
  int s = cSessionManager::self()->addSession (false);
  cConnection *connection = dynamic_cast<cConnection *>(am->object ("connection", s));
  //now establish that connection!
  connection->establishQuickConnection (qdlg->host(), qdlg->port());

  updateWindows ();
}

void KMuddy::doConnect ()
{
  //hide the dialog, so that it doesn't look like we've crashed
  cdlg->hide ();

  QString name = cdlg->selectedProfile ();
  bool sendNothing = cdlg->sendNothing ();

  doProfileConnect (name, sendNothing);
}

void KMuddy::doProfileConnect (const QString &name, bool sendNothing)
{
  //create cSession object and related stuff
  int s = cSessionManager::self()->addSession (true);

  // offline connection
  if (cdlg && cdlg->isOffLine()) {
    cTelnet *telnet = dynamic_cast<cTelnet *>(am->object ("telnet", s));
    telnet->setOffLineConnection(true);
  }

  //now establish that connection!
  cConnection *connection = dynamic_cast<cConnection *>(am->object ("connection", s));
  connection->establishConnection (name, sendNothing);

  updateWindows ();
}

void KMuddy::showAndHandleConnectDialog ()
{
  //so first we have to create the dialog...
  cdlg = new dlgConnect (this);
  connect (cdlg, SIGNAL (accepted()), this, SLOT (doConnect()));

  //dialog is ready - show it!
  cdlg->exec ();

  //further action is handled by signal issued by OK button, so that we only
  //have to destroy the dialog...
  delete cdlg;
  cdlg = nullptr;
}

void KMuddy::showAndHandleQuickConnectDialog ()
{
  //so first we have to create the dialog...
  if (!qdlg) qdlg = new dlgQuickConnect (this);
  connect (qdlg, SIGNAL (accepted()), this, SLOT (doQuickConnect()));

  //dialog is ready - show it!
  qdlg->exec ();

  //further action is handled by signal issued by OK button
}

/** lots of slots that handle menus; were handled by slots in objects,
now we have cSession, so we have to handle it here */
void KMuddy::addSelectionToClipboard ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->console()->addSelectionToClipboard(QClipboard::Clipboard);
}

void KMuddy::setParsing (bool value)
{
  list<int>::iterator it;
  list<int> sess = am->sessionList ();
  for (it = sess.begin(); it != sess.end(); ++it) {
    cCmdParser *cmdparser = dynamic_cast<cCmdParser *>(am->object ("cmdparser", *it));
    cmdparser->setParsing (value);
  }
}

void KMuddy::clearCommandQueue ()
{
  // NOTHING HERE
  // TODO: make it work when command queues get implemented
  am->invokeEvent ("message", activeSession(), "Feature not implemented yet.");
}

void KMuddy::showObjectsDialog ()
{
  if (!objdlg)
    objdlg = new dlgObjects (this);
  objdlg->show ();
}

void KMuddy::handleWindowsDialog ()
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->handleWindowsDialog();
}

void KMuddy::saveProfile ()
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->saveSession();
  am->invokeEvent ("message", activeSession(), i18n ("Profile has been saved. Note that profiles are automatically saved upon disconnecting, and also every five minutes."));
}

void KMuddy::switchAliases (bool value)
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->switchAliases(value);
}

void KMuddy::switchTriggers (bool value)
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->switchTriggers(value);
}

void KMuddy::switchTimers (bool value)
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->switchTimers(value);
}

void KMuddy::switchShortcuts (bool value)
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->switchShortcuts(value);
}

void KMuddy::showSettingsDialog ()
{
  dlgAppSettings::showSettingsDialog();
}

void KMuddy::showConnPrefsDialog ()
{
  cConnection *connection;
  connection = dynamic_cast<cConnection *>(am->object ("connection", activeSession()));
  connection->showConnPrefsDialog();
}

void KMuddy::configureTranscript ()
{
  cTranscript *transcript;
  transcript = dynamic_cast<cTranscript *>(am->object ("transcript", activeSession()));
  transcript->configure();
}

void KMuddy::dumpBuffer ()
{
  cTranscript *transcript;
  transcript = dynamic_cast<cTranscript *>(am->object ("transcript", activeSession()));
  transcript->dumpBuffer();
}

void KMuddy::makeDecision ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->makeDecision();
}

void KMuddy::importProfile ()
{
  dlgImportExportProfile *iep = new dlgImportExportProfile (true, this);
  iep->doThings ();
  delete iep;
}

void KMuddy::exportProfile ()
{
  dlgImportExportProfile *iep = new dlgImportExportProfile (false, this);
  iep->doThings ();
  delete iep;
}

void KMuddy::lineUp ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->console()->lineUp();
}

void KMuddy::lineDown ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->console()->lineDown();
}

void KMuddy::pageUp ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->console()->pageUp();
}

void KMuddy::pageDown ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->console()->pageDown();
}

void KMuddy::aconUp ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->aconUp();
}

void KMuddy::aconDown ()
{
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", activeSession()));
  output->aconDown();
}

bool KMuddy::eventFilter (QObject *o, QEvent *e)
{
  //only handle keypresses, nothing else...
  if (e->type() != QEvent::KeyPress)
    return KMainWindow::eventFilter (o, e);

  bool report = true;
  QKeyEvent *qke = (QKeyEvent *) e;
  int key = qke->key ();
  Qt::KeyboardModifiers mods = qke->modifiers ();

  //do not report modifiers
  switch (key) {
    case Qt::Key_Shift:
    case Qt::Key_Control:
    case Qt::Key_Meta:
    case Qt::Key_Alt:
      report = false;
      break;
    default:
      report = true;
      break;
  };

  if (report)
  {
    if (grabdlg)
    {
      grabdlg->gotKey (key, mods);
      return true;
    }
    else
    {
      cShortcutList *sl;
      sl = (cShortcutList *) cListManager::self()->getList (activeSession(), "macrokeys");
      if (sl && sl->handleKey (key, mods))
        return true;
    }
  }
  return KMainWindow::eventFilter (o, e);
}

bool KMuddy::queryClose ()
{
  list<int>::iterator it;
  list<int> sess = am->sessionList ();
  for (it = sess.begin(); it != sess.end(); ++it) {
    //switch to that session => the user knows which one is about to close
    cSessionManager::self()->setSession (*it);
    kapp->processEvents ();
    //disconnect, asking the user whether he really wants to
    cConnection *connection = dynamic_cast<cConnection *>(am->object ("connection", *it));
    if (!connection->handleDisconnect())
      return false;
  }

  return true;
}

#include "moc_kmuddy.cpp"
