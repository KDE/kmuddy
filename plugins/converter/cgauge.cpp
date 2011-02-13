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

