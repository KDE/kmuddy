//
// C++ Interface: KMuddyMapper
//
// Description: Plugin interface file for the KMuddy mapper.
//
//
// Author: Tomas Mecir, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#include "kmuddy_mapper.h"

#include "cactionmanager.h"
#include "cmenumanager.h"
#include "cprofilemanager.h"

#include "cmapmanager.h"
#include "cmapview.h"
#include "cmapfilter.h"
#include "cmapzonemanager.h"

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <QDebug>
#include <QDockWidget>
#include <map>

K_PLUGIN_FACTORY (KMuddyMapperFactory, registerPlugin<KMuddyMapper>();)
K_EXPORT_PLUGIN (KMuddyMapperFactory("kmuddy"))

struct MapperSession {
  CMapManager *manager;
  CMapFilter *filter;
};

struct KMuddyMapperPrivate {
  KToggleAction *showmapper;
  KComponentData componentData;
  QDockWidget *docker;
  std::map<int, MapperSession *> sessions;
  int currentSession;

  CMapManager *curManager() { return sessions.count(currentSession) ? sessions[currentSession]->manager : 0; };
  CMapZoneManager *curZoneManager() { return sessions.count(currentSession) ? sessions[currentSession]->manager->zoneManager() : 0; };
  CMapFilter *curFilter() { return sessions.count(currentSession) ? sessions[currentSession]->filter : 0; };
};

KMuddyMapper::KMuddyMapper (QObject *, const QVariantList &)
{
  d = new KMuddyMapperPrivate;

  d->componentData = KMuddyMapperFactory::componentData();

  _priority = 200;

  // we need to create an instance of CMapManager, which is the main class
  // The class is stored inside a docker, for now - we may want to get rid of all the xmlgui there
  KMainWindow *mainWindow = cActionManager::self()->mainWindow ();
  d->docker = new QDockWidget (mainWindow);

  d->docker->hide ();
  d->docker->setWindowTitle (i18n ("Mapper"));
  d->docker->setObjectName ("mapper");
  mainWindow->addDockWidget (Qt::RightDockWidgetArea, d->docker);
  d->docker->setFloating (true);
  connect (d->docker, SIGNAL (visibilityChanged(bool)), this, SLOT (mapperClosed()));
  d->docker->setWidget (0);

  d->currentSession = 0;

  KActionCollection *acol = cActionManager::self()->getACol ();
  d->showmapper = new KToggleAction (this);
  d->showmapper->setText (i18n ("Show &mapper"));
  connect (d->showmapper, SIGNAL (triggered (bool)), this, SLOT (showMapper (bool)));
  d->showmapper->setChecked (false);
  acol->addAction ("ShowMapper", d->showmapper);

  // plug things into the menu
  cMenuManager *menu = cMenuManager::self();
  menu->plug (d->showmapper, "view-global");
}

KMuddyMapper::~KMuddyMapper()
{
  // delete the CMapManager, remove menus, and all that
  cMenuManager *menu = cMenuManager::self();
  menu->unplug (d->showmapper);
  delete d->showmapper;
  std::map<int, MapperSession *>::iterator it;
  for (it = d->sessions.begin(); it != d->sessions.end(); ++it) {
    delete it->second->filter;
    delete it->second->manager;
    delete it->second;
  }
  delete d;
}

KComponentData KMuddyMapper::componentData () const {
  return d->componentData;
}

/** Called when a session has been added. If the session has existed before, fresh is set
to false. This can happen if the plug-in is loaded manually. */
void KMuddyMapper::sessionAdd (int sess, bool) {
  if (d->sessions.count(sess)) return;
  MapperSession *mp = new MapperSession;
  mp->manager = new CMapManager (d->docker, this, sess);
  mp->filter = new CMapFilter (mp->manager);
  d->sessions[sess] = mp;
}

/** Called when a session should be removed. Closed is false, if the session isn't being
closed. This hapens when the plug-in is being unloaded manually. */
void KMuddyMapper::sessionRemove (int sess, bool) {
  if (!d->sessions.count(sess)) return;

  // TODO: do things if needed, like closing maps

  // delete the filter and manager
  MapperSession *mp = d->sessions[sess];
  delete mp->filter;
  delete mp->manager;
  delete mp;
  d->sessions.erase(sess);

  if (sess == d->currentSession) d->currentSession = 0;
}

/** called when the user switches to another session. Sess is the number of the new session. */
void KMuddyMapper::sessionSwitch (int sess) {
  if (!d->sessions.count(sess)) sessionAdd(sess, false);
  CMapManager *manager = d->curManager();
  if (manager && manager->getActiveView()) manager->getActiveView()->hide();
  if (!cProfileManager::self()->settings (sess)) {  // this means that it's a profile connection
    d->currentSession = 0;
    return;
  }

  d->currentSession = sess;
  manager = d->curManager();
  if (manager) {
    manager->getActiveView()->show();
    d->docker->setWidget (manager->getActiveView());
  }
}

/** The session has just been connected. Not called when manually loading the plug-in. */
void KMuddyMapper::connected (int sess) {
  if (cActionManager::self()->activeSession() != sess) return;
  // Whatever data was loaded previously is now invalid, we need to do that anew.
  sessionRemove(sess);
  sessionSwitch(sess);
  d->sessions[sess]->manager->zoneManager()->loadMapList();
}

/** The session has just been disconnected. Not called when manually unloading the plug-in. */
void KMuddyMapper::disconnected (int sess) {
  // TODO: deactivate the map perhaps ?
  if (sess != d->currentSession) return;
  CMapManager *manager = d->curManager();
  if (manager) manager->getActiveView()->hide();
  d->docker->setWidget (0);
}

/** Request to load data. */
void KMuddyMapper::load (int sess) {
  if (!d->sessions.count(sess)) return;
  d->sessions[sess]->manager->zoneManager()->loadMapList();
  // TODO: load the current map? First map? Some map?
}

/** Request to save data. */
void KMuddyMapper::save (int sess) {
  if (!d->sessions.count(sess)) return;
  CMapZoneManager *zones = d->sessions[sess]->manager->zoneManager();
  zones->saveMapList();
  zones->save();
}

void KMuddyMapper::processInput (int sess, int phase, cTextChunk * chunk, bool) {
  if (phase != 1) return;  // don't do things twice
  if (sess != d->currentSession) return;
  CMapFilter *filter = d->curFilter();
  if (filter) filter->processServerOutput (chunk->toText());
}

/** Command that is to be sent to the MUD. Aliases have already been expanded.
Command can be modified if desired. If you set dontSend to true, the command won't be
sent and plug-ins with lower priority won't receive this command either. */
void KMuddyMapper::processCommand (int sess, QString &command, bool &) {
  // TODO: send the command for further processing, and handle the situation when the command is to be blocked
  if (sess != d->currentSession) return;
  CMapFilter *filter = d->curFilter();
  if (filter) command = filter->processCommand (command);
}

void KMuddyMapper::showMapper (bool b)
{
  b ? d->docker->show() : d->docker->hide();
}

void KMuddyMapper::mapperClosed ()
{
  d->showmapper->setChecked (d->docker->isVisible());
}

