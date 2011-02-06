//
// C++ Interface: plugintemplate
//
// Description: plugintemplate
//
//
// Author: YOUR-NAME, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PLUGINTEMPLATE_H
#define PLUGINTEMPLATE_H

#include <cplugin.h>
#include <QVariantList>

class KMuddyPluginTemplate : public cPlugin
{
public:
  KMuddyPluginTemplate (QObject *, const QVariantList &);
  virtual ~KMuddyPluginTemplate ();
};


#endif // PLUGIN_TEMPLATE_H

