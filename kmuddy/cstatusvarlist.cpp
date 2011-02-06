//
// C++ Implementation: cstatusvarlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cstatusvarlist.h"

#include "cactionmanager.h"
#include "cstatus.h"
#include "cstatusvar.h"
#include "cstatusvareditor.h"

struct cStatusVarList::Private {
  QString varName;
  bool updateNeeded, traversing, loaded;
  QString statusBar;
};

cStatusVarList::cStatusVarList () :
    cList ("statusvars")
{
  d = new Private;

  d->updateNeeded = false;
  d->traversing = false;
  d->loaded = false;

  addStringProperty ("variable", "Variable to react on");
  addStringProperty ("max-variable", "Max variable to react on");
  addStringProperty ("caption", "Caption of the gauge item");
  addBoolProperty ("percentage", "Show as percentage", false);
}

cStatusVarList::~cStatusVarList()
{
  delete d;
}

cListObject *cStatusVarList::newObject ()
{
  return new cStatusVar (this);
}

cListEditor *cStatusVarList::editor (QWidget *parent)
{
  return new cStatusVarEditor (parent);
}

void cStatusVarList::variableChanged (const QString &varname)
{
  d->varName = varname;
  
  d->updateNeeded = false;
  d->traversing = true;
  traverse (STATUSVAR_MATCH);
  d->traversing = false;
  if (d->updateNeeded)
    updateStatusBar ();
}

void cStatusVarList::listLoaded ()
{
  d->loaded = true;
  scheduleUpdate ();
}

void cStatusVarList::scheduleUpdate ()
{
  if (!d->loaded) return;  // not loaded yet - nothing to do
  // if we're working, schedule a delayed update, otherwise an immediate update
  if (d->traversing)
    d->updateNeeded = true;
  else
    updateStatusBar ();
}

void cStatusVarList::updateStatusBar ()
{
  d->statusBar.clear();
  traverse (STATUSVAR_UPDATE);
  cStatus *status = dynamic_cast<cStatus *>(cActionManager::self()->object ("status", session()));
  status->displayVariables (d->statusBar);
}

void cStatusVarList::addToStatusBar (const QString &text)
{
  if (!text.isEmpty())
    d->statusBar += text + "  ";
}

QString cStatusVarList::variableName () const
{
  return d->varName;
}

