//
// C++ Implementation: plugintemplate
//
// Description: plugintemplate
//
//
// Author: YOUR-NAME, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "plugin_template.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY (KMuddyPluginTemplateFactory, registerPlugin<KMuddyPluginTemplate>();)
K_EXPORT_PLUGIN (KMuddyPluginTemplateFactory("kmuddy"))

KMuddyPluginTemplate::KMuddyPluginTemplate (QObject *, const QVariantList &)
{
  // your code here ...
}

KMuddyPluginTemplate::~KMuddyPluginTemplate()
{
  // your code here ...
}



