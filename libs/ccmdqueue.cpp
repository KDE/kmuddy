//
// C++ Implementation: cCmdQueue
//
// Description: one command queue
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "ccmdqueue.h"

#include "cactionmanager.h"
#include "ccmdparser.h"
#include "ccmdprocessor.h"
#include "cmacromanager.h"
#include "cpattern.h"
#include "cvalue.h"


cCmdQueue::cCmdQueue (int _sess) : sess(_sess)
{
  parser = 0;
  preproc = 0;
}

cCmdQueue::~cCmdQueue()
{
  deleteAllCommands ();
  deleteAllVariables ();
  std::map<QString, cExecStack *>::iterator it;
  for (it = stacks.begin(); it != stacks.end(); ++it)
    delete it->second;
  stacks.clear ();
  delete preproc;
}

void cCmdQueue::deleteAllCommands ()
{
  if (!commands.empty()) {
    // delete the queue is needed
    std::list<cCmdQueueEntry *>::iterator it;
    for (it = commands.begin(); it != commands.end(); ++it)
      delete *it;
  }
  commands.clear ();
}

void cCmdQueue::fillFromPattern (const cPattern *p)
{
  setValue ("prefix", p->getVariable ("prefix"));
  setValue ("suffix", p->getVariable ("suffix"));
  setValue ("prefixtrim", p->getVariable ("prefixtrim"));
  setValue ("suffixtrim", p->getVariable ("suffixtrim"));
  setValue ("matched", p->getVariable ("matched"));
  if (p->matching() == cPattern::regexp)
    for (int i = 0; i < p->getBackRefList().count(); ++i)
      setValue (QString::number (i), p->getBackRefList()[i]);
}

bool cCmdQueue::finished ()
{
  return commands.empty();
}

bool cCmdQueue::waiting ()
{
  // TODO: make the real thing, not just this stub !!!
  return false;
}

QString cCmdQueue::getValue (const QString &name)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  if (variables.count (vn))
    return variables[vn]->asString();
  return QString();
}

bool cCmdQueue::varExists (const QString &name)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  return (variables.count (vn) != 0);
}

cValue *cCmdQueue::value (const QString &name)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  if (variables.count (vn))
    return variables[vn];
  return 0;
}

void cCmdQueue::setValue (const QString &name, const QString &value)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  delValue (vn);
  cValue *val = new cValue (value);
  variables[vn] = val;
}

void cCmdQueue::setValue (const QString &name, const cValue &value)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  delValue (vn);
  cValue *val = new cValue (value);
  variables[vn] = val;
}

void cCmdQueue::delValue (const QString &name)
{
  QString vn = name;
  if (vn[0] == '$')
    vn = vn.mid(1);
  if (!variables.count (vn)) return;
  delete variables[vn];
  variables.erase (vn);
}

void cCmdQueue::deleteAllVariables ()
{
  std::map<QString, cValue *>::iterator it;
  for (it = variables.begin(); it != variables.end(); ++it)
    delete it->second;
  variables.clear ();
}

cExecStack *cCmdQueue::execStack (const QString &name)
{
  if (!stacks.count (name))
    stacks[name] = new cExecStack;
  return stacks[name];
}

void cCmdQueue::addCommand (const QString &command, bool parsing, bool expandAliases)
{
  if (!parser)
    parser = dynamic_cast<cCmdParser *>(cActionManager::self()->object ("cmdparser", sess));
  if (!parser) return;
  
  if (parsing && (parser->parsing())) {
    // split the text into individual commands
    QStringList cmds = parser->parse (command, expandAliases);
    QStringList::iterator it;
    for (it = cmds.begin(); it != cmds.end(); ++it) {
      bool raw = parser->mustSendRaw (*it);
      QString mname, mparams;
      if ((!raw) && parser->isMacroCall (*it, mname, mparams)) {
        cCmdQueueEntry *entry = new cCmdQueueEntry;
        entry->isMacro = true;
        entry->canParse = true;
        entry->macroName = mname;
        entry->macroParams = mparams;
        commands.push_back (entry);
      } else {
        cCmdQueueEntry *entry = new cCmdQueueEntry;
        entry->isMacro = false;
        entry->canParse = !raw;
        entry->command = raw ? parser->fixRaw (*it) : (*it);
        commands.push_back (entry);
      }
    }
  }
  else {
    cCmdQueueEntry *entry = new cCmdQueueEntry;
    entry->isMacro = false;
    entry->canParse = false;
    entry->command = command;
    commands.push_back (entry);
  }
}

void cCmdQueue::executeNext ()
{
  if (finished()) return;

  cCmdQueueEntry *qe = commands.front();
  commands.pop_front();
  
  // call preprocessor commands on the entry
  bool ok = preprocess (qe);
  
  if (!ok) {
    // preprocessor command said, don't execute. So we won't.
    delete qe;
    return;
  }
  
  cActionManager *am = cActionManager::self();
  cCmdProcessor *cmdproc = dynamic_cast<cCmdProcessor *>(am->object ("cmdprocessor", sess));
  if (qe->isMacro)
    cmdproc->processMacro (qe->macroName, qe->macroParams, this);
  else {
    // send command; if we cannot parse, it gets sent directly without further parsing
    if (qe->canParse)
      cmdproc->processCommand (qe->command, this);
    else
      am->invokeEvent ("send-command", sess, qe->command);
  }

  delete qe;
}

void cCmdQueue::addPreprocessMacro (cMacro *macro)
{
  if (hasPreprocessMacro (macro)) return;
  if (!preproc) preproc = new std::list<cMacro *>;
  preproc->push_back (macro);
}

void cCmdQueue::removePreprocessMacro (cMacro *macro)
{
  if (!preproc) return;
  if (!hasPreprocessMacro (macro)) return;
  
  std::list<cMacro *>::iterator it;
  for (it = preproc->begin(); it != preproc->end(); ++it)
    if (*it == macro) {
      preproc->erase (it);
      return;
    }
}

bool cCmdQueue::hasPreprocessMacro (cMacro *macro)
{
  if (!preproc) return false;
  std::list<cMacro *>::iterator it;
  for (it = preproc->begin(); it != preproc->end(); ++it)
    if (*it == macro)
      return true;
  return false;
}

bool cCmdQueue::preprocess (cCmdQueueEntry *qe)
{
  if (!preproc) return true;
  
  bool ret = true;
  std::list<cMacro *>::iterator it;
  for (it = preproc->begin(); it != preproc->end(); ++it)
    if (!(*it)->preprocess (this, qe))
      ret = false;
  // If any preprocess command fails, we report failure. All preprocess commands get executed.
  return ret;
}


cExecStack::cExecStack ()
{
}

cExecStack::~cExecStack ()
{
  clear ();
}

void cExecStack::push (const cExecStackItem &item)
{
  stack.push_back (item);
}

cExecStackItem cExecStack::pop ()
{
  cExecStackItem item = top();
  stack.pop_back ();
  return item;
}

cExecStackItem cExecStack::top ()
{
  return stack.back();
}

bool cExecStack::empty ()
{
  return stack.empty();
}

void cExecStack::clear ()
{
  stack.clear ();
}

cExecStackItem::cExecStackItem ()
{
}

cExecStackItem::~cExecStackItem ()
{
  attribs.clear ();
}

int cExecStackItem::attrib (const QString &name)
{
  if (attribs.count (name))
    return attribs[name];
  return 0;
}

void cExecStackItem::setAttrib (const QString &name, int val)
{
  if ((val == 0) && (attribs.count (name)))
    attribs.erase (name);
  attribs[name] = val;
}

