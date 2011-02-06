//
// C++ Implementation: cbutton
//
// Description: One button (no GUI)
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

#include "cbutton.h"

#include "cactionmanager.h"
#include "cbuttonlist.h"
#include "cscripteval.h"
#include "kmuddy.h"

#include <kicon.h>
#include <ktoolbar.h>

#include <QAction>

struct cButton::Private {
  QString command, command2;
  QAction *action;
};

cButton::cButton (cList *list) : cListObject (list)
{
  d = new Private;
  d->action = new QAction (0);
  connect (d->action, SIGNAL (triggered (bool)), this, SLOT (execute (bool)));
}

cButton::~cButton()
{
  delete d->action;
  delete d;
}

void cButton::attribChanged (const QString &name)
{
  if (name == "command") {
    d->command = strVal ("command");
  }
  else if (name == "command-released") {
    d->command2 = strVal ("command-released");
  }
  else if (name == "caption") {
    d->action->setText (strVal ("caption"));
    updateVisibleName ();
  }
  else if (name == "icon") {
    d->action->setIcon (KIcon (strVal ("icon")));
  }
  else if (name == "pushdown") {
    d->action->setCheckable (boolVal ("pushdown"));
  }
}

void cButton::updateVisibleName()
{
  if (d->action->text().isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (d->action->text());
}

cList::TraverseAction cButton::traverse (int traversalType)
{
  if (traversalType == BUTTON_UPDATE) {
    addButton ();
    return cList::Continue;
  }
  return cList::Stop;
}

void cButton::execute (bool checked)
{
  cActionManager *am = cActionManager::self();
  int sess = list()->session();

  bool second = false;
  QString cmd = d->command;
  if (d->action->isCheckable() && (!checked)) {
    cmd = d->command2;
    second = true;
  }
  if (!cmd.isEmpty())
    am->invokeEvent ("command", sess, cmd);

  // execute the script, if any
  QString script = second ? strVal ("script-release") : strVal ("script");
  if (!script.isEmpty()) {
    cScriptEval *eval = dynamic_cast<cScriptEval *>(am->object ("scripteval", sess));
    if (eval) eval->eval (script);
  }
}

void cButton::addButton ()
{
  KToolBar *bar = KMuddy::self()->buttonBar ();
  bar->addAction (d->action);
}

void cButton::objectMoved ()
{
  ((cButtonList *) list())->updateButtons();
}

void cButton::objectEnabled ()
{
  ((cButtonList *) list())->updateButtons();
}

void cButton::objectDisabled ()
{
  ((cButtonList *) list())->updateButtons();
}

#include "cbutton.moc"
