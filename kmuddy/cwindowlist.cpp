//
// C++ Implementation: cwindowlist
//
// Description:
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cwindowlist.h"

#include "cactionmanager.h"
#include "cconsole.h"
#include "cglobalsettings.h"
#include "dialogs/dlgoutputwindow.h"
#include "cprofilemanager.h"

#include <kconfig.h>
#include <kconfiggroup.h>

using namespace std;

cWindowList::cWindowList (int sess)
  : cActionBase ("windowlist", sess)
{
  file = cProfileManager::self()->profilePath (sess) + "/windowlist";
  load ();

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cWindowList::~cWindowList ()
{
  save ();

  removeGlobalEventHandler ("global-settings-changed");

  //destroy all windows
  map<QString, dlgOutputWindow *>::iterator it;
  for (it = windows.begin(); it != windows.end(); ++it)
    delete it->second;
  windows.clear ();
}

void cWindowList::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    adjustFonts (gs->getFont ("console-font"));
    applySettings (gs->getBool ("allow-blink"), gs->getBool ("word-wrap"),
        gs->getInt ("wrap-pos"), gs->getInt ("indent"), gs->getBool ("force-redraw"));
  }
}

bool cWindowList::exists (const QString &name)
{
  if (windows.count (name))
    return true;
  return false;
}

bool cWindowList::add (const QString &name, bool autoadd)
{
  if (exists (name)) return false;
  //won't allow empty window name
  if (name.isEmpty()) return false;
  //create the window
  dlgOutputWindow *owin = new dlgOutputWindow (cActionManager::self()->mainWidget());
  //remember it
  windows[name] = owin;
  owin->setSession (sess());
  owin->setOutputWindowName(name);
  
  //let the window be shown by default, unless added by autorestore
  if(!autoadd)
    owin->show ();
  
  // set settings
  cGlobalSettings::self()->notifyChange();

  return true;
}

bool cWindowList::remove (const QString &name)
{
  if (!exists (name)) return false;

  //get the window
  dlgOutputWindow *owin = windows[name];
  //hide it
  owin->hide ();
  //delete it
  delete owin;
  //remove it from our mapping
  windows.erase (name);
  //add it to the list of windows, that are to be erased from config-file
  toerase.push_back (name);
  return true;
}

QStringList cWindowList::windowList ()
{
  QStringList lst;
  map<QString, dlgOutputWindow *>::iterator it;
  for (it = windows.begin(); it != windows.end(); ++it)
    lst.push_back (it->first);
  return lst;
}

bool cWindowList::show (const QString &name)
{
  if (!exists (name)) return false;

  dlgOutputWindow *owin = windows[name];

  owin->show ();
  return true;
}

bool cWindowList::hide (const QString &name)
{
  if (!exists (name)) return false;

  dlgOutputWindow *owin = windows[name];

  owin->hide ();
  return true;
}

void cWindowList::toggle (const QString &name)
{
  if (!exists (name)) return;
  if (windows[name]->isVisible())
    windows[name]->hide ();
  else
    windows[name]->show ();
}

bool cWindowList::isShown (const QString &name)
{
  if (!exists (name)) return false;
  return windows[name]->isVisible();
}

void cWindowList::textToWindow (const QString &name, cTextChunk *chunk)
{
  if (!exists (name)) return;

  //add text to the window
  windows[name]->addLine(chunk);
  // switch-window is used so that others can determine which window was used
  invokeEvent ("switch-window", sess(), name);
  invokeEvent ("displayed-line", sess(), chunk);
}

void cWindowList::load ()
{
  QStringList groups;
  QPoint pos;
  bool vis;

  KConfig *config = new KConfig (file);

  groups = config->groupList();

  if(groups.empty() == true)
    return;

  //traverse through list of groups, loading each window
  for ( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it )
    {
      name = *it;
      if(name.indexOf("KMuddywin_", 0) != -1)
      {
        KConfigGroup g = config->group (name);
        name.replace("KMuddywin_", QString());
        if(!add(name, true))
          return;
        windows[name]->setInitialSize(g.readEntry("Size", QSize()));
        pos = g.readEntry("Position", QPoint());
        windows[name]->move(pos);
        vis = g.readEntry("Visible", true);
        if(vis)
          show(name);
      }
    }
  delete config;
}

void cWindowList::save ()
{
  KConfig *config = new KConfig (file);

  //erase removed windows from the configfile
  if(!toerase.empty())
  {
    for(QStringList::Iterator it = toerase.begin(); it != toerase.end(); ++it)
    {
      config->deleteGroup("KMuddywin_"+*it);
    }
  }

  //save all windows
  map<QString, dlgOutputWindow *>::iterator it;
  for (it = windows.begin(); it != windows.end(); ++it)
  {
    KConfigGroup g = config->group ("KMuddywin_"+it->first);
    g.writeEntry("Size", it->second->size());
    g.writeEntry("Position", it->second->pos());
    g.writeEntry("Visible", it->second->isVisible());
  }

  delete config;
}

void cWindowList::adjustFonts(QFont font)
{
  map<QString, dlgOutputWindow *>::iterator it;
  for (it = windows.begin(); it != windows.end(); ++it)
  {
    it->second->setFont(font);
  }
}

void cWindowList::applySettings (bool allowblinking, bool wordwrapping,
    int wrappos, int indentvalue, int forceredraw)
{
  map<QString, dlgOutputWindow *>::iterator it;
  for (it = windows.begin(); it != windows.end(); ++it)
  {
    cConsole *console = it->second->console();
    console->setEnableBlinking (allowblinking);
    console->setWordWrapping (wordwrapping);
    console->setWrapPos (wrappos);
    console->setIndentation (indentvalue);
    console->setRepaintCount (forceredraw);
  }
}


