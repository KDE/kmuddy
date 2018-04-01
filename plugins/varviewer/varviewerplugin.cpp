//
// C++ Implementation: varviewerplugin
//
// Description: Variable Viewer plugin.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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


#include "varviewerplugin.moc"
