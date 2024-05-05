//
// C++ Implementation: vartrigplugin
//
// Description: vartrigplugin
//
/*
Copyright 2009-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "vartrigplugin.h"

#include "clistmanager.h"
#include "cprofilemanager.h"

#include "cvartriggerlist.h"

#include <kpluginfactory.h>
#include <KLocalizedString>

K_PLUGIN_CLASS_WITH_JSON(cVarTrigPlugin, "vartrigplugin.json")

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


#include "vartrigplugin.moc"
