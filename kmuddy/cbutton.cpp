//
// C++ Implementation: cbutton
//
// Description: One button (no GUI)
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
