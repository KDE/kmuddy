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

#include <kconfig.h>
#include <kconfiggroup.h>

cStatusVar::cStatusVar (int _sess): sess(_sess)
{
  varvalue = maxvarvalue = 0;
  _hidden = false;
  _percentage = false;
}

cStatusVar::~cStatusVar()
{
}

cSaveableField *cStatusVar::newInstance ()
{
  return new cStatusVar (sess);
}

void cStatusVar::setVariable (const QString &val)
{
  _variable = val;
  if (!_variable.isEmpty() && _variable[0] == '$')
    _variable = _variable.remove (0, 1);
  varvalue = 0;
}

void cStatusVar::setMaxVariable (const QString &val)
{
  _maxvariable = val;
  if (!_maxvariable.isEmpty() && _maxvariable[0] == '$')
    _maxvariable = _variable.remove (0, 1);
}

void cStatusVar::setCaption (const QString &val)
{
  _caption = val;
}

void cStatusVar::setPercentage (bool percent)
{
  _percentage = percent;
}

void cStatusVar::setHidden (bool h)
{
  _hidden = h;
}

void cStatusVar::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);

  setVariable (g.readEntry ("Variable", QString()));
  setMaxVariable (g.readEntry ("Max variable", QString()));
  setCaption (g.readEntry ("Caption", QString()));
  setPercentage (g.readEntry ("Percentage", false));
  setHidden (g.readEntry ("Hidden", false));
}

