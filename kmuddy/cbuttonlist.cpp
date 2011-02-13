//
// C++ Implementation: cbuttonlist
//
// Description: 
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cbuttonlist.h"

#include "cbutton.h"
#include "cbuttoneditor.h"
#include "kmuddy.h"

#include <ktoolbar.h>

cButtonList::cButtonList () :
    cList ("buttons")
{
  loaded = false;

  addStringProperty ("caption", "Caption of the button");
  addStringProperty ("command", "Command sent upon pressing/enabling the button.");
  addStringProperty ("command-released", "Command sent upon disabling the button.");
  addStringProperty ("icon", "Icon associated with the button.");
  addBoolProperty ("pushdown", "Use a push-down button.", false);
  // script
  addStringProperty ("script", "Script to execute");
  addStringProperty ("script-release", "Script to execute when released");
}

cButtonList::~cButtonList()
{
}

cListObject *cButtonList::newObject ()
{
  return new cButton (this);
}

cListEditor *cButtonList::editor (QWidget *parent)
{
  return new cButtonEditor (parent);
}

void cButtonList::listLoaded ()
{
  loaded = true;
  updateButtons ();
}

void cButtonList::updateButtons ()
{
  if (!loaded) return;  // not loaded yet - nothing to do
  cActionManager *am = cActionManager::self();
  // nothing if our session isn't the active one
  if (am->activeSession() != session()) return;
  KToolBar *bar = KMuddy::self()->buttonBar();

  // hide any old buttons
  bar->clear ();

  // then update/show what has to be shown
  // doing it this way ensures that disabling a group hides all child elements
  traverse (BUTTON_UPDATE);

  // show or hide the buttonbar depending on whether it has any buttons
  bar->actions().empty() ? bar->hide() : bar->show();
}

