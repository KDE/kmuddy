//
// C++ Implementation: cstatusvar
//
// Description: One variable entry in the status bar
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

#include "cstatusvar.h"

#include "cactionmanager.h"
#include "cstatusvarlist.h"
#include "cvariablelist.h"

struct cStatusVar::Private {
  QString variable, maxvariable;
  QString caption;
  bool percentage;
  cVariableList *vars;
};

cStatusVar::cStatusVar (cList *list) : cListObject (list)
{
  d = new Private;
  d->percentage = false;
  d->vars = 0;
}

cStatusVar::~cStatusVar()
{
  delete d;
}

void cStatusVar::objectMoved ()
{
  // object position changed - must update the status bar
  ((cStatusVarList *) list())->scheduleUpdate();
}

void cStatusVar::objectEnabled ()
{
  // object position changed - must update the status bar
  ((cStatusVarList *) list())->scheduleUpdate();
}

void cStatusVar::objectDisabled ()
{
  // object position changed - must update the status bar
  ((cStatusVarList *) list())->scheduleUpdate();
}

void cStatusVar::attribChanged (const QString &name)
{
  if (name == "variable") {
    d->variable = strVal ("variable");
    updateVisibleName ();
    // remove leading $
    // note that this will lead to this function being called again
    if ((!d->variable.isEmpty()) && (d->variable[0] == '$'))
      setStr ("variable", d->variable.mid (1));
    ((cStatusVarList *) list())->scheduleUpdate();
  }
  else if (name == "max-variable") {
    d->maxvariable = strVal ("max-variable");
    // remove leading $
    // note that this will lead to this function being called again
    if ((!d->maxvariable.isEmpty()) && (d->maxvariable[0] == '$'))
      setStr ("max-variable", d->maxvariable.mid (1));
    ((cStatusVarList *) list())->scheduleUpdate();
  }
  else if (name == "caption") {
    d->caption = strVal ("caption");
    updateVisibleName ();
    ((cStatusVarList *) list())->scheduleUpdate();
  }
  else if (name == "percentage") {
    d->percentage = boolVal ("percentage");\
    ((cStatusVarList *) list())->scheduleUpdate();
  }
}

void cStatusVar::updateVisibleName()
{
  if (d->variable.isEmpty() && d->caption.isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (d->caption.isEmpty() ? d->variable : d->caption + " (" + d->variable + ")");
}

cList::TraverseAction cStatusVar::traverse (int traversalType)
{
  if (traversalType == STATUSVAR_MATCH) {
    cStatusVarList *svl = (cStatusVarList *) list();
    QString variable = svl->variableName();
    if ((variable == d->variable) || (variable == d->maxvariable))
      // we'll need an update after this is done
      svl->scheduleUpdate ();
    return cList::Continue;
  }
  if (traversalType == STATUSVAR_UPDATE) {
    updateBar ();
    return cList::Continue;
  }
  return cList::Stop;
}

void cStatusVar::updateBar ()
{
  QString text;

  if (!d->vars)
    d->vars = dynamic_cast<cVariableList *>(cActionManager::self()->object ("variables", list()->session()));

  text = d->caption.isEmpty() ? QString() : (d->caption + " ");

  if (d->percentage) {
    // if we want percentage, we'll need integer values
    int val = d->vars->getIntValue (d->variable);
    int maxVal = d->vars->exists (d->maxvariable) ? d->vars->getIntValue (d->maxvariable) : 100;
    
    int percent = (maxVal == 0) ? (val * 100 / maxVal) : 0;
    text += QString::number (percent) + "%";
  } else {
    // no percentage - text values
    text += d->vars->getValue (d->variable);
    if (d->vars->exists (d->maxvariable))
      text += "/" + d->vars->getValue (d->maxvariable);
  }

  ((cStatusVarList *) list())->addToStatusBar (text);
}

