//
// C++ Implementation: vartrigplugin
//
// Description: vartrigplugin
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "vartrigplugin.h"

#include "clistmanager.h"
#include "cprofilemanager.h"

#include "cvartriggerlist.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <klocale.h>

K_PLUGIN_FACTORY (kmuddyvartrigplugin, registerPlugin<cVarTrigPlugin>();)
K_EXPORT_PLUGIN (kmuddyvartrigplugin("kmuddy"))

cVarTrigPlugin::cVarTrigPlugin (QObject *, const QVariantList &)
    : cActionBase ("vartrigplugin", 0)
{
  cListManager *lm = cListManager::self();
  lm->registerType ("vartriggers", i18n ("Variable Triggers"), cVarTriggerList::newList);

  addEventHandler ("var-changed", 50, PT_STRING);
}

cVarTrigPlugin::~cVarTrigPlugin()
{
  removeEventHandler ("var-changed");

  cListManager *lm = cListManager::self();
  lm->unregisterType ("vartriggers");
}

void cVarTrigPlugin::eventStringHandler (QString event, int session,
    QString &par1, const QString &)
{
  if (event != "var-changed") return;

  cListManager *lm = cListManager::self();
  cVarTriggerList *vtl = (cVarTriggerList *) lm->getList (session, "vartriggers");
  if (vtl) vtl->variableChanged (par1);
}


