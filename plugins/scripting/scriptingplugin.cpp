//
// C++ Implementation: scriptingplugin
//
// Description: scriptingplugin
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

#include "scriptingplugin.h"

#include "cactionmanager.h"
#include "clistmanager.h"
#include "cmacromanager.h"
#include "cmenumanager.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"

#include "cnotifymanager.h"
#include "crunninglist.h"
#include "cscriptlist.h"
#include "dlgrunninglist.h"

#include <QDir>
#include <kaction.h>
#include <kactioncollection.h>  
#include <kapplication.h>
#include <kmainwindow.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <KLocalizedString>
#include <ktoggleaction.h>

#include <map>

K_PLUGIN_CLASS_WITH_JSON(cScriptingPlugin, "scriptingplugin.json")


// Macros exec and notify:

class cMacroExec : public cMacro {
  public:
    cMacroExec () : cMacro ("exec") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString sname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString pars = params.section (' ', 1, -1, QString::SectionSkipEmpty);
      pars = expandVariables (pars, sess, queue);
      cScriptList *list = dynamic_cast<cScriptList *>(cListManager::self()->getList (sess, "scripts"));
      if (!list) {  // this shouldn't happen
        am->invokeEvent ("message", sess, i18n ("/exec: Script list is not available."));
        return;
      }

      list->runScript (sname, pars);
    }
};

class cMacroNotify : public cMacro {
  public:
    cMacroNotify () : cMacro ("notify") {
      notifymanager = new cNotifyManager;
    }
    ~cMacroNotify () {
      delete notifymanager;
    }
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString pars = expandVariables (params, sess, queue);
      QString ip_port = pars.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString ip_data = pars.section (' ', 1, -1, QString::SectionSkipEmpty);

      bool convert_ok;
      int  port;
      port = ip_port.toInt(&convert_ok);
// Send data to IP port
      if (convert_ok)
        notifymanager->doNotify (port, ip_data);
    }
  private:
    cNotifyManager *notifymanager;
};



struct ScriptingSessionData {
  cRunningList *runningList;
};

struct cScriptingPluginPrivate {
  QAction *showRunningScripts;
  dlgRunningList *rdlg;

  cMacroExec *mexec;
  cMacroNotify *mnotify;
  
  std::map<int, ScriptingSessionData> sessionData;
};

cScriptingPlugin::cScriptingPlugin (QObject *, const QVariantList &)
{
  d = new cScriptingPluginPrivate;

  // set default values for our profile-based values
  cProfileSettings::setDefaultString ("script-directory", QDir::homePath());
  cProfileSettings::setDefaultString ("script-working-directory", QDir::homePath());

  d->mexec = new cMacroExec;
  d->mnotify = new cMacroNotify;

  cListManager *lm = cListManager::self();
  lm->registerType ("scripts", i18n ("Scripts"), cScriptList::newList);

  KMainWindow *mainWindow = cActionManager::self()->mainWindow ();
  d->rdlg = new dlgRunningList (mainWindow);
  d->rdlg->hide ();
  d->rdlg->setObjectName ("runningscripts");
  mainWindow->addDockWidget (Qt::RightDockWidgetArea, d->rdlg);
  d->rdlg->setFloating (true);

  KActionCollection *acol = cActionManager::self()->getACol ();
  d->showRunningScripts = d->rdlg->toggleViewAction ();
  acol->addAction ("ShowRunningScripts", d->showRunningScripts);

  // plug things into the menu
  cMenuManager *menu = cMenuManager::self();
  menu->plug (d->showRunningScripts, "view-profile");
}

cScriptingPlugin::~cScriptingPlugin()
{
  cMenuManager *menu = cMenuManager::self();
  menu->unplug (d->showRunningScripts);

  cListManager *lm = cListManager::self();
  lm->unregisterType ("scripts");

  delete d->rdlg;
  delete d->mexec;
  delete d->mnotify;
  delete d;
}

void cScriptingPlugin::sessionSwitch (int)
{
  updateRunningList ();
}

void cScriptingPlugin::connected (int sess)
{
  ScriptingSessionData sd;
  sd.runningList = 0;
  if (cProfileManager::self()->settings (sess))  // this means that it's a profile connection
    sd.runningList = new cRunningList (sess);
  d->sessionData[sess] = sd;
  if (cActionManager::self()->activeSession() == sess)
    sessionSwitch (sess);
}

void cScriptingPlugin::disconnected (int sess)
{
  if (d->sessionData.count (sess)) {
    delete d->sessionData[sess].runningList;
    d->sessionData[sess].runningList = 0;
  }
  d->sessionData.erase (sess);

  updateRunningList ();
}

void cScriptingPlugin::updateRunningList ()
{
  if (!d->rdlg) return;
  cActionManager *am = cActionManager::self();
  int s = am->activeSession();
  cRunningList *rl = dynamic_cast<cRunningList *>(am->object ("runninglist", s));
  d->rdlg->switchRunningList (rl);
}

#include "scriptingplugin.moc"
