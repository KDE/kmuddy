//
// C++ Implementation: cvartrigger
//
// Description: One variable trigger.
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

#include "cvartrigger.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cVarTrigger::cVarTrigger (int _sess) : sess(_sess)
{
}


cVarTrigger::~cVarTrigger()
{
}

cSaveableField *cVarTrigger::newInstance ()
{
  return new cVarTrigger (sess);
}

void cVarTrigger::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);
  setVarName (g.readEntry ("Variable name", ""));

  newtext.clear();
  int cmdcount = g.readEntry ("Command count", 0);
  for (int i = 1; i <= cmdcount; i++)
  {
    QString cmdline = g.readEntry ("Command line " +
          QString::number (i), "");
    newtext.push_back (cmdline);
  }
}

void cVarTrigger::setVarName (const QString &varname)
{
  QString v = varname;
  //remove all leading $-characters, if any
  while ((!v.isEmpty()) && (v[0] == '$'))
    v = v.mid (1);
  //set the variable name, if it's non-empty
  if (!v.isEmpty())
    var = v;
}


