//
// C++ Implementation: cbuttonlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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

