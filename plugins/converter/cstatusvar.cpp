//
// C++ Implementation: cstatusvar
//
// Description: One variable entry in the status bar
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cstatusvar.h"

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

