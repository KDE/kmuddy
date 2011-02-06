//
// C++ Interface: scriptingplugin
//
// Description: scriptingplugin
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGPLUGIN_H
#define SCRIPTINGPLUGIN_H

#include <cplugin.h>
#include <QVariantList>

struct cScriptingPluginPrivate;

class cScriptingPlugin : public cPlugin
{
public:
  cScriptingPlugin (QObject *, const QVariantList &);
  virtual ~cScriptingPlugin ();
  
  virtual void sessionSwitch (int sess);
  virtual void connected (int sess);
  virtual void disconnected (int sess);  

protected:
  void updateRunningList ();

  cScriptingPluginPrivate *d;
};


#endif // SCRIPTINGPLUGIN_H

