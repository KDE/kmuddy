//
// C++ Interface: cpluginmanager
//
// Description: plug-in manager
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H


#include <cactionbase.h>
#include <kmuddy_export.h>

#include <qobject.h>
#include <qstringlist.h>
#include <map>

using namespace std;

class cPlugin;
class cTextChunk;
class KPluginInfo;
class KDialog;
class KPluginSelector;

/**
Plug-in manager handles plug-in information, loads plug-ins, and passes data/events to them as needed.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cPluginManager : public QObject, public cActionBase
{
Q_OBJECT
 public:
  virtual ~cPluginManager ();

  static cPluginManager *self();

  /** load a plug-in into memory, if possible */
  bool loadPlugin (const QString &name);
  /** unload a plug-in from memory, if possible */
  bool unloadPlugin (const QString &name);
  bool isLoaded (const QString &name);

  /** list of plug-ins that should not be loaded - from preferences */
  void setNoAutoLoadList (QStringList noAutoLoad) { noload = noAutoLoad; };
  QStringList noAutoLoadList () { return noload; };
  
  /** Load all available plug-ins expect those that should not be loaded.
  This ensures that all newly installed plug-ins get loaded initially. */
  void loadAll ();
  /** unload all plug-ins that are currently loaded, but are included in noload list */
  void unloadUnwanted ();
  /** unload all plug-ins */
  void unloadAll ();

public slots:
  /** show the Plugins dialog */
  void showPluginsDialog ();
  
protected slots:
  /** apply settings from the plugins dialog */
  void applyPluginDialog ();
  
protected:
  cPluginManager ();
  
  static cPluginManager *_self;

  virtual void eventIntHandler (QString event, int session, int par1, int par2);
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &par2);
  virtual void eventNothingHandler (QString event, int session);
  virtual void eventChunkHandler (QString event, int session, cTextChunk *par);
  
  void findPlugins ();
  
  void passSessionAdd (int sess);
  void passSessionRemove (int sess);
  void passSessionSwitch (int sess);
  void passConnected (int sess);
  void passDisconnected (int sess);
  void passRawData (int sess, char *);
  void passDecompressedData (int sess, char *);
  void passInput (int sess, int phase, cTextChunk *chunk, bool gagged);
  void passPrompt (int sess, cTextChunk *chunk);
  void passCommand (int sess, QString &command);
  
  KPluginInfo getPluginInfo (const QString &name);
  map<QString, KPluginInfo> pluginInfo;
  map<QString, cPlugin *> loadedPlugins;
  /** plug-ins sorted by priority */
  multimap<int, cPlugin *> plugins;
  /** iterator, it's not a local variable, so that passing data to plug-ins is a bit faster :) */
  multimap<int, cPlugin *>::iterator it;
  QStringList noload;
  /** existing sessions */
  map<int, bool> sessions;
  /** active session */
  int activeSess;
  bool lastGagged;

  KDialog *pluginDialog;
  KPluginSelector *pluginSelector;
};

#endif
