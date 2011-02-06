//
// C++ Implementation: cgauge
//
// Description: One gauge (no GUI)
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cgauge.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cGauge::cGauge (int _sess) : sess(_sess)
{
  varvalue = maxvarvalue = 0;
  _hidden = false;
  _color = Qt::white;
}

cGauge::~cGauge()
{
}

cSaveableField *cGauge::newInstance ()
{
  return new cGauge (sess);
}

void cGauge::setVariable (const QString &val)
{
  _variable = val;
  varvalue = 0;
}

void cGauge::setMaxVariable (const QString &val)
{
  _maxvariable = val;
  maxvarvalue = 0;
}

void cGauge::setCaption (const QString &val)
{
  _caption = val;
}

void cGauge::setColor (const QColor &c)
{
  _color = c;
}

void cGauge::setHidden (bool h)
{
  _hidden = h;
}

void cGauge::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);

  setVariable (g.readEntry ("Variable", QString()));
  setMaxVariable (g.readEntry ("Max variable", QString()));
  setCaption (g.readEntry ("Caption", QString()));
  setColor (g.readEntry ("Color", (QColor) Qt::white));
  setHidden (g.readEntry ("Hidden", false));
}

