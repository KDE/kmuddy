//
// C++ Implementation: cvartrigger
//
// Description: One variable trigger.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

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


