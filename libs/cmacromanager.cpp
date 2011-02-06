//
// C++ Implementation: cMacroManager, cMacro, cFunction
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cmacromanager.h"
#include "cactionmanager.h"
#include "cvariablelist.h"

#include <map>

using namespace std;

cMacroManager *cMacroManager::_self = 0;

struct cMacroManagerPrivate {
  map<QString, cMacro *> macros;
  map<QString, cFunction *> functions;
};

cMacroManager * cMacroManager::self()
{
  if (!_self)
    _self = new cMacroManager;
  return _self;
}

cMacroManager::cMacroManager ()
    : cActionBase ("macros", 0)
{
  d = new cMacroManagerPrivate;
}

cMacroManager::~cMacroManager ()
{
  delete d;
  _self = 0;
}

void cMacroManager::addMacro (const QString &name, cMacro *macro)
{
  if (d->macros.count (name)) return;
  if (!macro) return;
  d->macros[name] = macro;
}

void cMacroManager::removeMacro (const QString &name)
{
  d->macros.erase (name);
}

cMacro *cMacroManager::macro (const QString &name)
{
  if (!d->macros.count (name)) return 0;
  return d->macros[name];
}

bool cMacroManager::callMacro (const QString &name, const QString &params,
    int sess, cCmdQueue *queue)
{
  if (!d->macros.count (name)) return false;
  d->macros[name]->eval (params, sess, queue);
  return true;
}

void cMacroManager::addFunction (const QString &name, cFunction *function)
{
  if (d->functions.count (name)) return;
  if (!function) return;
  d->functions[name] = function;
}

void cMacroManager::removeFunction (const QString &name)
{
  d->functions.erase (name);
}

bool cMacroManager::functionExists (const QString &name)
{
  return (d->functions.count (name) != 0);
}

cValue cMacroManager::callFunction (const QString &name, std::list<cValue> &params,
    int sess, cCmdQueue *queue)
{
  cValue empty;
  if (!functionExists (name)) return empty;
  return d->functions[name]->eval (params, sess, queue);
}

/************************  cMacro  ***************************/

cMacro::cMacro (const QString &name)
{
  n = name;
  cMacroManager::self()->addMacro (n, this);
  am = cActionManager::self();
}

cMacro::~cMacro ()
{
  cMacroManager::self()->removeMacro (n);
}

cVariableList *cMacro::varList (int sess)
{
  return dynamic_cast<cVariableList *>(am->object ("variables", sess));
}

QString cMacro::expandVariables (const QString &str, int sess, cCmdQueue *queue) {
  return varList(sess)->expandVariables (str, true, queue);
}

/************************  cFunction  ***************************/

cFunction::cFunction (const QString &name)
{
  n = name;
  cMacroManager::self()->addFunction (n, this);
  am = cActionManager::self();
}

cFunction::~cFunction ()
{
  cMacroManager::self()->removeFunction (n);
}

cVariableList *cFunction::varList (int sess)
{
  return dynamic_cast<cVariableList *>(am->object ("variables", sess));
}

