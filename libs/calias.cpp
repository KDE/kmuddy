//
// C++ Implementation: calias
//
// Description: 
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cactionmanager.h"
#include "calias.h"
#include "caliaslist.h"
#include "ccmdqueue.h"
#include "cexpresolver.h"
#include "cpattern.h"
#include "cscripteval.h"

// this structure holds the alias settings cached for faster access
struct cAlias::Private {
  bool includeprefixsuffix;
  bool sendoriginal;
  bool global;
  QString condition;
  arith_exp *exp;
  cPattern p;
  cExpResolver *resolver;
};

cAlias::cAlias (cList *list) : cListObject (list)
{
  d = new Private;
  
  // these defaults must match the default property values set in cAliasList

  //default compare type is "begins with"
  d->p.setMatching (cPattern::begin);
  //do not send original command by default
  d->sendoriginal = false;
  //include prefix/suffix
  d->includeprefixsuffix = true;
  d->global = false;
  d->exp = nullptr;
  d->resolver = nullptr;
}

cAlias::~cAlias ()
{
  delete d->exp;
  delete d->resolver;
  delete d;
}

void cAlias::updateVisibleName ()
{
  QString pattern = strVal ("pattern");
  if (pattern.isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (pattern);
}

void cAlias::attribChanged (const QString &name)
{
  if (name == "pattern") {
    d->p.setPattern (strVal ("pattern"));
    updateVisibleName ();
    return;
  }
  if (name == "matching") {
    int m = intVal ("matching");
    cPattern::PatternType pt;
    switch (m) {
      case 0: pt = cPattern::exact; break;
      case 1: pt = cPattern::substring; break;
      case 2: pt = cPattern::begin; break;
      case 3: pt = cPattern::end; break;
      case 4: pt = cPattern::regexp; break;
      default: pt = cPattern::begin;
    }
    d->p.setMatching (pt);
    return;
  }
  if (name == "cs") {
    d->p.setCaseSensitive (boolVal ("cs"));
    return;
  }
  if (name == "prefix-suffix") {
    d->includeprefixsuffix = boolVal ("prefix-suffix");
    return;
  }
  if (name == "orig") {
    d->sendoriginal = boolVal ("orig");
    return;
  }
  if (name == "whole-words") {
    d->p.setWholeWords (boolVal ("whole-words"));
    return;
  }
  if (name == "global") {
    d->global = boolVal ("global");
    return;
  }
  if (name == "condition") {
    d->condition = strVal ("condition");
    // TODO: this is duplicated for every place with conditions
    // find out if we could create a common class for this
    delete d->exp;
    d->exp = nullptr;
  
    // no expression ? nothing to do !
    if (d->condition.trimmed().isEmpty()) return;

    // parse the condition
    arith_exp *exp = new arith_exp;
    bool ok = exp->compile (d->condition);
    if (ok)
      d->exp = exp;
    else
      // cannot parse condition - no conditional matching ...
      delete exp;
    return;
  }
}

cList::TraverseAction cAlias::traverse (int traversalType)
{
  if (traversalType == ALIAS_MATCH)
    return doMatch ();
  return cList::Stop;  // unknown action
}

cList::TraverseAction cAlias::doMatch ()
{
  cAliasList *al = (cAliasList *) list();
  // fetch the string from the alias list
  QString string = al->stringToMatch();
  int mpos = 0;  // reset the index where matching will start
  //match against alias
  bool everMatched = false;
  while (true) {
    if (!d->p.match (string, mpos)) break;  // match the string

    // matched
    bool cond = testCondition ();  // match the condition

    // also matched - execute this alias !
    // but don't break the loop if the condition didn't match, as global matching
    // relies on that
    if (cond) {
      everMatched = true;
      executeAlias ();
    }

    if (!d->global) break;  // only continue if it's a global alias
    
    // global - update matching position, or terminate if no more matching should occur
    // if last length is 0, we must advance by 1 to avoid an endless loop ...
    int shift = (d->p.getLastLength() == 0) ? 1 : d->p.getLastLength();
    mpos = d->p.getLastPos() + shift;
    if (mpos >= string.length())
      break;
  }

  // TODO: better control of when to continue / stop
  return everMatched ? cList::Stop : cList::Continue;
}

// TODO: this is duplicated for every place with conditions
// find out if we could create a common class for this
bool cAlias::testCondition ()
{
  // no condition -> always matches ...
  if (!d->exp) return true;

  if (!d->resolver) d->resolver = new cExpResolver (list()->session());

  // set up pseudo-variable expansion
  cCmdQueue *queue = new cCmdQueue (list()->session());
  d->resolver->setQueue (queue);
  queue->fillFromPattern (&d->p);
  cValue val = d->exp->evaluate (d->resolver);
  delete queue;
  d->resolver->setQueue(nullptr);

  // test passes if the evaluator returns non-zero ...
  return (val.asInteger() != 0);
}

void cAlias::executeAlias ()
{
  // execute the script, if any
  // this needs to be done before the actual alias expansion
  QString script = strVal ("script");
  if (!script.isEmpty()) {
    cActionManager *am = cActionManager::self();
    int sess = list()->session();
    cScriptEval *eval = dynamic_cast<cScriptEval *>(am->object ("scripteval", sess));
    if (eval) eval->eval (script, d->p.scriptVariables());
  }

  cAliasList *al = (cAliasList *) list();
  al->setMatched ();
  if (d->sendoriginal) al->wantOriginalCommand ();

  for (int i = 1; i <= strListCount ("newtext"); ++i) {
    QString cmd = strListValue ("newtext", i);
    d->p.expandPseudoVariables (cmd);
    if (d->includeprefixsuffix)
      al->addCommand (d->p.getPrefix() + cmd + d->p.getSuffix());
    else
      al->addCommand (cmd);
  }
}


