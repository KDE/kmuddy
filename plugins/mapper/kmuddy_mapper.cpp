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

#include "cmapmanager.h"
#include "cmapfilter.h"

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <klocale.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <QDebug>

K_PLUGIN_FACTORY (KMuddyMapperFactory, registerPlugin<KMuddyMapper>();)
K_EXPORT_PLUGIN (KMuddyMapperFactory("kmuddy"))

struct KMuddyMapperPrivate {
  CMapManager *manager;
  CMapFilter *filter;
  KToggleAction *showmapper;
  KComponentData componentData;
};

KMuddyMapper::KMuddyMapper (QObject *, const QVariantList &)
{
  d = new KMuddyMapperPrivate;

  d->componentData = KMuddyMapperFactory::componentData();

  _priority = 200;

  // we need to create an instance of CMapManager, which is the main class
  d->manager = new CMapManager (this);
  d->filter = new CMapFilter (d->manager);

  connect (d->manager, SIGNAL (closed()), this, SLOT (mapperClosed()));

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
  delete d->filter;
  delete d->manager;
  delete d;
}

KComponentData KMuddyMapper::componentData () const {
  return d->componentData;
}

/** Called when a session has been added. If the session has existed before, fresh is set
to false. This can happen if the plug-in is loaded manually. */
void KMuddyMapper::sessionAdd (int, bool) {
  // TODO: do things if needed
}

/** Called when a session should be removed. Closed is false, if the session isn't being
closed. This hapens when the plug-in is being unloaded manually. */
void KMuddyMapper::sessionRemove (int, bool) {
  // TODO: do things if needed, like closing maps
}

/** called when the user switches to another session. Sess is the number of the new session. */
void KMuddyMapper::sessionSwitch (int) {
  // TODO: switch to the map for the now-active session
}

/** The session has just been connected. Not called when manually loading the plug-in. */
void KMuddyMapper::connected (int) {
  // TODO: activate the map perhaps ?
}

/** The session has just been disconnected. Not called when manually unloading the plug-in. */
void KMuddyMapper::disconnected (int) {
  // TODO: deactivate the map perhaps ?
}

/** Request to load data. */
void KMuddyMapper::load (int) {
  // TODO: load the map for the given session
}

/** Request to save data. */
void KMuddyMapper::save (int) {
  // TODO: save the map of the given session
}

void KMuddyMapper::processInput (int, int phase, cTextChunk * chunk, bool) {
  if (phase != 1) return;  // don't do things twice
  d->filter->processServerOutput (chunk->toText());
}

/** Command that is to be sent to the MUD. Aliases have already been expanded.
Command can be modified if desired. If you set dontSend to true, the command won't be
sent and plug-ins with lower priority won't receive this command either. */
void KMuddyMapper::processCommand (int, QString &command, bool &) {
  // TODO: send the command for further processing, and handle the situation when the command is to be blocked
  command = d->filter->processCommand (command);
}

void KMuddyMapper::showMapper (bool b)
{
  b ? d->manager->show() : d->manager->hide();
}

void KMuddyMapper::mapperClosed ()
{
  d->showmapper->setChecked (false);
}

