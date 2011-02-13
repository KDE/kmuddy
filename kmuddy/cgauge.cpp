//
// C++ Implementation: cgauge
//
// Description: One gauge (no GUI)
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

#include "cgauge.h"

#include "cactionmanager.h"
#include "cgaugelist.h"
#include "cgaugebar.h"
#include "cgaugebaritem.h"
#include "cvariablelist.h"

struct cGauge::Private {
  QString variable, maxvariable;
  QString caption;
  QColor color;
  cGaugeBarItem *gaugeitem;
  cVariableList *vars;
};

cGauge::cGauge (cList *list) : cListObject (list)
{
  d = new Private;
  
  d->color = Qt::white;
  d->vars = 0;
  
  QWidget *gaugeBar = dynamic_cast<QWidget *>(cActionManager::self()->object ("gaugebar", list->session()));
  d->gaugeitem = new cGaugeBarItem (gaugeBar);

  cGaugeList *gl = (cGaugeList *) list;
  gl->addGauge (this);
}

cGauge::~cGauge()
{
  delete d->gaugeitem;
  d->gaugeitem = 0;
  delete d;
}

void cGauge::attribChanged (const QString &name)
{
  if (name == "variable") {
    d->variable = strVal ("variable");
    updateVisibleName ();
    // remove leading $
    // note that this will lead to this function being called again
    if ((!d->variable.isEmpty()) && (d->variable[0] == '$'))
      setStr ("variable", d->variable.mid (1));
    updateGauge ();
  }
  else if (name == "max-variable") {
    d->maxvariable = strVal ("max-variable");
    // remove leading $
    // note that this will lead to this function being called again
    if ((!d->maxvariable.isEmpty()) && (d->maxvariable[0] == '$'))
      setStr ("max-variable", d->maxvariable.mid (1));
    updateGauge ();
  }
  else if (name == "caption") {
    d->caption = strVal ("caption");
    updateVisibleName ();
    updateGauge ();
  }
  else if (name == "color") {
    int color = intVal ("color");
    d->color.setBlue (color % 256);
    color /= 256;
    d->color.setGreen (color % 256);
    color /= 256;
    d->color.setRed (color % 256);
    updateGauge ();
  }
}

void cGauge::updateVisibleName()
{
  if (d->variable.isEmpty() && d->caption.isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (d->caption.isEmpty() ? d->variable : d->caption + " (" + d->variable + ")");
}

cList::TraverseAction cGauge::traverse (int traversalType)
{
  if (traversalType == GAUGE_MATCH) {
    cGaugeList *gl = (cGaugeList *) list();
    QString variable = gl->variableName();
    if ((variable == d->variable) || (variable == d->maxvariable))
      updateGauge ();
    return cList::Continue;
  }
  if (traversalType == GAUGE_UPDATE) {
    // the gauge isn't on the bar currently, so add it there
    cGaugeBar *bar = dynamic_cast<cGaugeBar *>(cActionManager::self()->object ("gaugebar", list()->session()));
    bar->addGauge (this);
    updateGauge ();
    return cList::Continue;
  }
  return cList::Stop;
}

void cGauge::updateGauge ()
{
  if (!d->vars)
    d->vars = dynamic_cast<cVariableList *>(cActionManager::self()->object ("variables", list()->session()));
  int val = d->vars->getIntValue (d->variable);
  int maxVal = d->vars->exists (d->maxvariable) ? d->vars->getIntValue (d->maxvariable) : 100;
  int value = (maxVal != 0) ? (val * 100 / maxVal) : 0;
  if (value < 0) value = 0;
  if (value > 100) value = 100;

  d->gaugeitem->setValue (value);
  d->gaugeitem->setText (d->caption);
  d->gaugeitem->setColor (d->color);
}

cGaugeBarItem *cGauge::gaugeItem ()
{
  return d->gaugeitem;
}

void cGauge::objectMoved ()
{
  ((cGaugeList *) list())->updateGauges();
}

void cGauge::objectEnabled ()
{
  ((cGaugeList *) list())->updateGauges();
}

void cGauge::objectDisabled ()
{
  ((cGaugeList *) list())->updateGauges();
}

