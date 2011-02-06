//
// C++ Implementation: cgaugelist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cgaugelist.h"

#include "cgauge.h"
#include "cgaugebar.h"
#include "cgaugeeditor.h"
#include "kmuddy.h"

#include <list>

struct cGaugeList::Private {
  QString varName;
  bool loaded;
  std::list<cGauge *> gauges;  // a list of all existing gauges
};

cGaugeList::cGaugeList () :
    cList ("gauges")
{
  d = new Private;

  d->loaded = false;

  addStringProperty ("variable", "Variable to react on");
  addStringProperty ("max-variable", "Max variable to react on");
  addStringProperty ("caption", "Caption of the gauge item");
  addIntProperty ("color", "Color to use", 0);
}

cGaugeList::~cGaugeList()
{
  delete d;
}

void cGaugeList::addGauge (cGauge *g)
{
  d->gauges.push_back (g);
}

void cGaugeList::removeGauge (cGauge *g)
{
  d->gauges.remove (g);
}

cListObject *cGaugeList::newObject ()
{
  return new cGauge (this);
}

cListEditor *cGaugeList::editor (QWidget *parent)
{
  return new cGaugeEditor (parent);
}

void cGaugeList::variableChanged (const QString &varname)
{
  d->varName = varname;
  traverse (GAUGE_MATCH);
}

void cGaugeList::listLoaded ()
{
  d->loaded = true;
  updateGauges ();
}

void cGaugeList::updateGauges ()
{
  if (!d->loaded) return;  // not loaded yet - nothing to do
  cGaugeBar *bar = dynamic_cast<cGaugeBar *>(cActionManager::self()->object ("gaugebar", session()));
  if (!bar) return;

  // first hide everything
  std::list<cGauge *>::iterator it;
  for (it = d->gauges.begin(); it != d->gauges.end(); ++it)
    bar->removeGauge (*it);

  // then update/show what has to be shown
  // doing it this way ensures that disabling a group hides all child elements
  traverse (GAUGE_UPDATE);

  //show the gaugebar if we have some unhidden item
  if (bar->gauges())
    KMuddy::self()->showGauges (true);
}

QString cGaugeList::variableName () const
{
  return d->varName;
}

