//
// C++ Implementation: cexpresolver
//
// Description: Variable/function resolver for the expression parser.
//
/*
Copyright 2006-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cexpresolver.h"

#include "cactionmanager.h"
#include "cmacromanager.h"
#include "cvariablelist.h"

#include <klocale.h>

cExpResolver::cExpResolver (int _sess)
{
  sess = _sess;
}

cValue cExpResolver::get (QString varName)
{
  // we have to request the list on every call, because we aren't notified
  // about disconnects/reconnects, so if we were to keep a pointer to
  // the variable list, we could easily end up with a wrong one
  cVariableList *vars = dynamic_cast<cVariableList *>(cActionManager::self()->object("variables", sess));

  cValue *val = 0;
  if (vars) val = vars->value(varName, queue);
  if (val) return *val;
  // can't retrieve value for some reason - return empty one
  return cValue::empty();
}

cValue cExpResolver::function_call (const QString &functionName, list<cValue> &arguments)
{
  if (!cMacroManager::self()->functionExists (functionName)) {
    cActionManager::self()->invokeEvent ("message", sess,
        i18n ("Function %1 does not exist - assuming empty return value.").arg (functionName));
    return cValue::empty();
  }
  return cMacroManager::self()->callFunction (functionName, arguments, sess, queue);
}

