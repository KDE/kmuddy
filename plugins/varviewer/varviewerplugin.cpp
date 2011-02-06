//
// C++ Implementation: varviewerplugin
//
// Description: Variable Viewer plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "varviewerplugin.h"

#include "dlgvarviewer.h"

#include "cactionmanager.h"
#include "cmenumanager.h"

#include <kactioncollection.h>
#include <kmainwindow.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY (cVarViewerPluginFactory, registerPlugin<cVarViewerPlugin>();)
K_EXPORT_PLUGIN (cVarViewerPluginFactory("kmuddy"))

struct cVarViewerPluginPrivate
{
  dlgVarViewer *viewer;
  QAction *viewAction;
};

cVarViewerPlugin::cVarViewerPlugin (QObject *, const QVariantList &)
{
  d = new cVarViewerPluginPrivate;

  KMainWindow *mainWindow = cActionManager::self()->mainWindow ();
  d->viewer = new dlgVarViewer (mainWindow);
  d->viewer->hide ();
  d->viewer->setObjectName ("varviewer");
  mainWindow->addDockWidget (Qt::RightDockWidgetArea, d->viewer);
  d->viewer->setFloating (true);

  KActionCollection *acol = cActionManager::self()->getACol ();
  d->viewAction = d->viewer->toggleViewAction ();
  acol->addAction ("ShowVariables", d->viewAction);

  // plug things into the menu
  cMenuManager *menu = cMenuManager::self();
  menu->plug (d->viewAction, "view-global");

}

cVarViewerPlugin::~cVarViewerPlugin()
{
  KMainWindow *mainWindow = cActionManager::self()->mainWindow ();
  mainWindow->removeDockWidget (d->viewer);

  cMenuManager *menu = cMenuManager::self();
  menu->unplug (d->viewAction);

  delete d->viewer;
  delete d;
}



