//
// C++ Implementation: cpluginmanager
//
// Description: plug-in manager
//
/*
Copyright 2006-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cpluginmanager.h"

#include "cactionmanager.h"
#include "cplugin.h"

#include <kdebug.h>
#include <kdialog.h>
#include <QFrame>
#include <QVBoxLayout>
#include <kplugininfo.h>
#include <kpluginselector.h>
#include <ktrader.h>
//#include <kparts/componentfactory.h>
#include <kparts/plugin.h>

#include <list>

using namespace std;

cPluginManager *cPluginManager::_self = 0;

cPluginManager *cPluginManager::self ()
{
  if (!_self)
    _self = new cPluginManager;
  return _self;
}

cPluginManager::cPluginManager () : cActionBase ("pluginmanager", 0)
{
  findPlugins ();
  activeSess = 0;
  lastGagged = false;
  pluginDialog = 0;

  // global objects receive events from all sessions, hence this will work well
  addEventHandler ("send-command", 20, PT_STRING);
  addEventHandler ("raw-data", 20, PT_STRING);
  addEventHandler ("raw-data-comp", 20, PT_STRING);
  addEventHandler ("got-prompt", 20, PT_TEXTCHUNK);
  addEventHandler ("got-line", 20, PT_TEXTCHUNK);
  addEventHandler ("got-line-after-triggers", 20, PT_TEXTCHUNK);
  addEventHandler ("will-gag", 20, PT_NOTHING);
  addEventHandler ("connected", 20, PT_NOTHING);
  addEventHandler ("disconnected", 20, PT_NOTHING);
  addEventHandler ("save", 20, PT_NOTHING);
  addEventHandler ("session-activated", 20, PT_INT);
  addEventHandler ("session-created", 20, PT_INT);
  addEventHandler ("session-destroyed", 20, PT_INT);  
}


cPluginManager::~cPluginManager ()
{
  //unload plug-ins
  unloadAll ();
  
  removeEventHandler ("raw-data");
  removeEventHandler ("raw-data-comp");
  removeEventHandler ("send-command");
  removeEventHandler ("raw-data");
  removeEventHandler ("raw-data-comp");
  removeEventHandler ("got-prompt");
  removeEventHandler ("got-line");
  removeEventHandler ("got-line-after-triggers");
  removeEventHandler ("will-gag");
  removeEventHandler ("connected");
  removeEventHandler ("disconnected");
  removeEventHandler ("save");
  removeEventHandler ("session-activated");
  removeEventHandler ("session-created");
  removeEventHandler ("session-destroyed");
  
  _self = 0;
}

void cPluginManager::eventIntHandler (QString event, int, int par1, int)
{
  if (event == "session-activated") {
    passSessionSwitch (par1);
  }  
  if (event == "session-created") {
    passSessionAdd (par1);
  }  
  if (event == "session-destroyed") {
    passSessionRemove (par1);
  }  
}

void cPluginManager::eventStringHandler (QString event, int session, QString &par1, const QString &)
{
  if (event == "send-command") {
    passCommand (session, par1);
  }
  else if (event == "raw-data-comp") {
    QByteArray par1Data = par1.toLatin1();
    char *data = par1Data.data();
    passRawData (session, data);
    par1 = data;  // make changes take effect
  }
  else if (event == "raw-data") {
    QByteArray par1Data = par1.toLatin1();
    char *data = par1Data.data();
    passRawData (session, data);
    par1 = data;  // make changes take effect
  }
}

void cPluginManager::eventNothingHandler (QString event, int session)
{
  if (event == "will-gag") {
    lastGagged = true;
  }
  else if (event == "connected") {
    passConnected (session);
  }
  else if (event == "disconnected") {
    passDisconnected (session);
  }
  else if (event == "save") {
    passSave (session);
  }
}

void cPluginManager::eventChunkHandler (QString event, int session, cTextChunk *par)
{
  if (event == "got-prompt") {
    passPrompt (session, par);
  }
  if (event == "got-line") {
    passInput (session, 1, par, false);
  }
  if (event == "got-line-after-triggers") {
    passInput (session, 2, par, lastGagged);
    lastGagged = false;
  }
}

void cPluginManager::findPlugins ()
{
  pluginInfo.clear ();
  KPluginInfo::List pi = KPluginInfo::fromServices (KServiceTypeTrader::self()->query (
      QString::fromLatin1("KMuddy/Plugin"),
      QString::fromLatin1("(Type == 'Service') and ([X-KMuddy-Version] == 2)")));
  
  // fill in the associative array with plugin info
  KPluginInfo::List::Iterator it;
  for (it = pi.begin(); it != pi.end(); ++it) {
    KPluginInfo info = *it;
    kDebug() << "Found plugin: " << info.name();
    pluginInfo[info.name()] = info;
  }
}

KPluginInfo cPluginManager::getPluginInfo (const QString &name)
{
  if (pluginInfo.count (name))
    return pluginInfo[name];
  return KPluginInfo(QString());
}

bool cPluginManager::loadPlugin (const QString &name)
{
  if (isLoaded (name))    //plug-in is already loaded!
    return false;
  if (!(pluginInfo.count (name)))  //we know nothing of that plug-in
    return false;
  
  KService::Ptr service = pluginInfo[name].service();
  KPluginFactory *factory = KPluginLoader (*service).factory();
  cPlugin *plugin = factory->create<cPlugin> (0);

  if (!plugin) {
    kDebug() << "Failed to load plugin: " << name;
  pluginInfo[name].setPluginEnabled (false);
    return false;
  }

  pluginInfo[name].setPluginEnabled (true);
  loadedPlugins[name] = plugin;
  plugins.insert (pair<int, cPlugin *>(plugin->priority(), plugin));
  
  // call plugin's session loading for each active session - the plugin can load the data and
  // the like here
  cActionManager *am = cActionManager::self();
  map<int, bool>::iterator sit;
  for (sit = sessions.begin(); sit != sessions.end(); ++sit) {
    plugin->sessionAdd (sit->first, false);
    cActionBase *conn = am->object ("connection", sit->first);
    if (conn && conn->attrib ("connected"))
      plugin->connected (sit->first);  // if the session is connected, notify the plugin of that
  }
  plugin->setActiveSession (activeSess);
  plugin->sessionSwitch (activeSess);
  kDebug() << "Loaded plugin: " << name;
  return true;
}

bool cPluginManager::unloadPlugin (const QString &name)
{
  if (!(isLoaded (name)))    //plug-in is not loaded!
    return false;
  cPlugin *p = loadedPlugins[name];
  loadedPlugins.erase (name);
  pluginInfo[name].setPluginEnabled (false);
  //remove plugin from the priority list
  for (it = plugins.begin(); it != plugins.end(); ++it)
    if (it->second == p)
    {
      plugins.erase (it);
      break;
    }
  
  // call plugin's session unloading for each active session - the plugin can save the data and
  // the like here
  map<int, bool>::iterator sit;
  for (sit = sessions.begin(); sit != sessions.end(); ++sit) {
    p->sessionRemove (sit->first, false);
  }
  
  kDebug() << "Unloaded plugin: " << name;

  delete p;
  return true;
}

bool cPluginManager::isLoaded (const QString &name)
{
  return (loadedPlugins.count (name) != 0);
}

void cPluginManager::loadAll ()
{
  map<QString, KPluginInfo>::iterator it;
  for (it = pluginInfo.begin(); it != pluginInfo.end(); ++it)
    if (!(noload.contains (it->first)))
      loadPlugin (it->first);
}

void cPluginManager::unloadUnwanted ()
{
  QStringList::iterator it;
  for (it = noload.begin(); it != noload.end(); ++it)
    if (isLoaded (*it))
      unloadPlugin (*it);
}

void cPluginManager::unloadAll ()
{
  //unloading a plug-in modifies the map, invalidating iterators...
  //I therefore have to create a list of loaded plug-in names and use that instead
  list<QString> loadedList;
  map<QString, cPlugin *>::iterator lpi;
  for (lpi = loadedPlugins.begin(); lpi != loadedPlugins.end(); ++lpi)
    loadedList.push_back (lpi->first);
  list<QString>::iterator it;
  for (it = loadedList.begin(); it != loadedList.end(); ++it)
    unloadPlugin (*it);
}

void cPluginManager::showPluginsDialog ()
{
  pluginDialog = new KDialog (0);
  pluginDialog->setCaption (i18n ("Plugins"));
  pluginDialog->setButtons (KDialog::Ok | KDialog::Cancel | KDialog::Apply);
  pluginDialog->setInitialSize (QSize (400, 300));

  pluginSelector = new KPluginSelector (pluginDialog);
  pluginDialog->setMainWidget (pluginSelector);

  connect (pluginDialog, SIGNAL (okClicked()), this, SLOT (applyPluginDialog ()));
  connect (pluginDialog, SIGNAL (applyClicked()), this, SLOT (applyPluginDialog ()));

  QList<KPluginInfo> list;
  map<QString, KPluginInfo>::iterator itp;
  for (itp = pluginInfo.begin(); itp != pluginInfo.end(); ++itp)
    list.append (itp->second);
  pluginSelector->addPlugins (list, KPluginSelector::IgnoreConfigFile);
  // pluginSelector->load ();

  pluginDialog->exec ();

  delete pluginDialog;
  pluginDialog = 0;
}

void cPluginManager::applyPluginDialog ()
{
  // we construct the new list of disabled plug-ins
  pluginSelector->updatePluginsState ();
  QStringList nl;
  map<QString, KPluginInfo>::iterator itp;
  for (itp = pluginInfo.begin(); itp != pluginInfo.end(); ++itp)
    if (!itp->second.isPluginEnabled())
      nl += itp->second.name();
  noload = nl;
  
  // unload unwanted plug-ins, load newly wanted ones
  unloadUnwanted ();
  loadAll ();
}

void cPluginManager::passSessionAdd (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->sessionAdd (sess, true);
  sessions[sess] = true;
}

void cPluginManager::passSessionRemove (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->sessionRemove (sess, true);
  sessions.erase (sess);
}

void cPluginManager::passSessionSwitch (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
  {
    it->second->sessionSwitch (sess);
    it->second->setActiveSession (sess);
  }
  activeSess = sess;
}

void cPluginManager::passConnected (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->connected (sess);
}

void cPluginManager::passDisconnected (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->disconnected (sess);
}

void cPluginManager::passSave (int sess)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->save (sess);
}

void cPluginManager::passRawData (int sess, char *data)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->rawData (sess, data);
}

void cPluginManager::passDecompressedData (int sess, char *data)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->decompressedData (sess, data);
}

void cPluginManager::passInput (int sess, int phase, cTextChunk *chunk, bool gagged)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->processInput (sess, phase, chunk, gagged);
}

void cPluginManager::passPrompt (int sess, cTextChunk *chunk)
{
  for (it = plugins.begin(); it != plugins.end(); ++it)
    it->second->processPrompt (sess, chunk);
}

void cPluginManager::passCommand (int sess, QString &command)
{
  bool dontSend = false;
  for (it = plugins.begin(); it != plugins.end(); ++it) {
    it->second->processCommand (sess, command, dontSend);
    if (dontSend) break;
  }
  if (dontSend) command = QString();
}

#include "cpluginmanager.moc"
