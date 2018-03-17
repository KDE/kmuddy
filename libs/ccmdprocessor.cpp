/***************************************************************************
                          ccmdprocessor.cpp  -  command processor
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pi Jul 5 2002
    copyright            : (C) 2002 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ccmdprocessor.h"

#include "cexpresolver.h"
#include "cglobalsettings.h"
#include "cmacromanager.h"
#include "cvariablelist.h"

#include <KLocalizedString>

#include <map>

class cExpCache {
  public:
    cExpCache ();
    ~cExpCache ();
    arith_exp *expression (const QString &src);
    void addExpression (const QString &src, arith_exp *exp);
    void clear ();
  protected:
    std::map<QString, arith_exp *> cache;
};


cExpCache::cExpCache ()
{
}

cExpCache::~cExpCache ()
{
  clear ();
}

arith_exp *cExpCache::expression (const QString &src)
{
  if (cache.count (src))
    return cache[src];
  return 0;
}

void cExpCache::addExpression (const QString &src, arith_exp *exp)
{
  // clear the cache if it's grown way too big
  // it's unlikely that anyone will ever hit this limit, but just in case ...
  if (cache.size() > 10000) clear();
  
  if (cache.count (src)) delete cache[src];
  cache[src] = exp;
}

void cExpCache::clear ()
{
  std::map<QString, arith_exp *>::iterator it;
  for (it = cache.begin(); it != cache.end(); ++it)
    delete it->second;
  cache.clear ();
}


cCmdProcessor::cCmdProcessor (int sess) : cActionBase ("cmdprocessor", sess)
{
  focusstr = ":";
  resolver = new cExpResolver (sess);
  expcache = new cExpCache;
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cCmdProcessor::~cCmdProcessor()
{
  removeGlobalEventHandler ("global-settings-changed");
  delete resolver;
  delete expcache;
}

void cCmdProcessor::eventNothingHandler (QString event, int /*session*/)
{
  if (event == "global-settings-changed") {
    setFocusCommandString (cGlobalSettings::self()->getString ("str-focus"));
  }
}

void cCmdProcessor::setFocusCommandString (QString str)
{
  focusstr = str.trimmed();
}

void cCmdProcessor::processCommand (const QString &command, cCmdQueue *queue)
{
  QString cmd = command;  // we need to modify the command ...

  // expand internal scripting and variables, then send the command ...
  // also process command focusing if needed
  // internal scripting must be expanded first, because it can contain variable names, and
  // expandVariables would therefore mess things up ...
  expandInternalScripting (cmd, queue);
  expandVariables (cmd, queue);

  // focusing command?
  bool focused = false;
  int pos;
  if ((pos = isFocusCommand (cmd)) != -1) {
    focused = true;
    if (processFocusCommand (cmd, pos) == -1)
      focused = false;
  }
  
  if (!focused)
    // send the command !
    invokeEvent ("send-command", sess(), cmd);
}

void cCmdProcessor::expandInternalScripting (QString &command, cCmdQueue *queue)
{
  // require profile connection ...
  if (!settings()) return;

  resolver->setQueue (queue);
  if (command.indexOf ('[') == -1)  // no internal scripting in this command ...
    return;
  
  QString res, script;
  bool changed = false;
  // walk through the command and expand [] sequences as scripting, putting the result into
  // the "res" string
  int len = command.length();
  bool backslash = false, in = false, instring = false;
  for (int i = 0; i < len; ++i) {
    char ch = command[i].toLatin1();
    if (backslash) {
      backslash = false;
      if (in)
        script += command[i];
      else
        res += command[i];
      continue;
    }
    if (ch == '\\') {
      backslash = true;
      if (in)
        script += command[i];
      else
        res += command[i];
      continue;
    }
    if (!in) {
      if (ch == '[') {
        in = true;
        instring = false;
        script = "";
        continue;
      }
      // add the letter to the result
      res += command[i];
    }
    else {
      if (instring) {
        script += command[i];
        if (ch == '"')
          instring = false;
      } else {
        if (ch == ']')
        {
          // end of script - evaluate it, assuming it's not empty
          script = script.trimmed ();
          if (script.length() > 0) {
            bool ok = true;
            cValue val = eval (script, queue, ok);
            if (ok) {
              res += val.asString ();
              changed = true;
            } else {
              // invokeEvent ("message", sess(), i18n ("Error in the script. Sending as-is."));
              res += QChar('[') + script + QChar(']');
            }
          } else {
            res += QChar('[') + script + QChar(']');
          }
          script = "";
          in = false;
        }
        else
          script += command[i];
      }
    }
  }
  
  if (in)  // unterminated script = no script
    res += QChar ('[') + script;

  // if there was some change, update the command !
  if (changed)
    command = res;
}

cValue cCmdProcessor::eval (const QString &e, cCmdQueue *queue, bool &ok)
{
  resolver->setQueue (queue);
  ok = true;

  // end of script - evaluate it, assuming it's not empty
  QString script = e.trimmed ();
  if (script.length() == 0)
    return cValue::empty();

  // try to fetch expression from the cache
  arith_exp *exp = expcache->expression (script);
  // expression is not in cache - need to create, compile and add to the cache
  if (!exp) {
    exp = new arith_exp;
    ok = exp->compile (script);
    if (ok)
      expcache->addExpression (script, exp);
    else
      delete exp;  // wrong expression - get rid of it
  }
  if (ok)
    return exp->evaluate (resolver);
  else
    return cValue::empty();
}

void cCmdProcessor::expandVariables (QString &command, cCmdQueue *queue)
{
  cVariableList *vl = dynamic_cast<cVariableList *>(object ("variables"));
  if (vl)
    command = vl->expandVariables (command, true, queue);
}

int cCmdProcessor::isFocusCommand (const QString &command)
{
  int pos;
  if ((focusstr.length() > 0) && (command.startsWith(focusstr))) {
    QString cmd = command.trimmed(); //removes leading/trailing spaces
    if ((pos = cmd.indexOf(focusstr, focusstr.length())) == -1)
      return -1; //return false if a second focustr can't be found
    return pos;
  }
  return -1; //should not get to here
}

int cCmdProcessor::processFocusCommand (const QString &text, int pos)
{
  QString window, command;
  window = command = text;

  window.remove(pos, window.length());
  window.remove(0,focusstr.length()); //remove first focusstr

  command.remove(0,pos);
  command.remove(0,focusstr.length()); //remove next

  if ( (command.length() < 1) || (window == focusstr) || (window.length() < 1))
    return -1; //if nothing was passed after 2nd focusstr, return

  invokeEvent ("focus-change", 0, window, command);

  return 0;
}

void cCmdProcessor::processMacro (const QString &mname, const QString &params, cCmdQueue *queue)
{
  cMacroManager *macros = dynamic_cast<cMacroManager *>(object ("macros", 0));
  if (!macros) return;
  
  if (!settings()) // -> not a profile-based connection
  {
    //exclaim that macros are not available
    invokeEvent ("message", sess(), i18n ("Sorry, but macro calls are only available for profile connections."));
    return;
  }

  QString mnamel = mname.toLower ();
  QString pars = params;  // needed to get rid of the "const" modifier
  
  expandInternalScripting (pars, queue);
  // we don't expand variables - macros must do that themselves
  // TODO: maybe allow the macro to specify whether it needs unexpanded vars ?
  if (!macros->callMacro (mnamel, pars, sess(), queue))
    invokeEvent ("message", sess(), i18n ("This macro does not exist."));
    
}

