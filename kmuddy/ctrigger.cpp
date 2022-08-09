/***************************************************************************
                          ctrigger.cpp  -  trigger
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne okt 13 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
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

#include "ctrigger.h"

#include "csoundplayer.h"

#include "cactionmanager.h"
#include "cansiparser.h"
#include "ccmdqueue.h"
#include "cexpresolver.h"
#include "coutput.h"
#include "cpattern.h"
#include "cscripteval.h"
#include "ctextchunk.h"
#include "ctextprocessor.h"
#include "ctriggerlist.h"
#include "cvariablelist.h"

#include <list>

// this struct holds some trigger settings for faster access
struct cTrigger::Private {
  bool global;
  bool dontsend;
  bool promptDetect;
  QString condition;
  cList::TraverseAction ifMatch, ifNotMatch;
  arith_exp *exp;
  cPattern p;
  cExpResolver *resolver;
  QStringList commands;   // commands that this trigger wants to execute

  /** Helper for recolorize() */
  QColor getColor (int c);
};

cTrigger::cTrigger (cList *list) : cListObject (list)
{
  d = new Private;

  // these defaults must match the default property values set in cTriggerList

  d->p.setMatching (cPattern::substring);
  d->global = false;
  d->dontsend = false;
  d->promptDetect = false;
  d->exp = nullptr;
  d->resolver = nullptr;
  d->ifMatch = cList::Stop;
  d->ifNotMatch = cList::Continue;
}

cTrigger::~cTrigger ()
{
  delete d->exp;
  delete d->resolver;
  delete d;
}

void cTrigger::updateVisibleName ()
{
  QString pattern = strVal ("pattern");
  if (pattern.isEmpty())
    cListObject::updateVisibleName();
  else
    setVisibleName (pattern);
}

void cTrigger::attribChanged (const QString &name)
{
  if (name == "pattern") {
    d->p.setPattern (strVal ("pattern"));
    updateVisibleName();
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
  if (name == "whole-words") {
    d->p.setWholeWords (boolVal ("whole-words"));
    return;
  }
  if (name == "global") {
    d->global = boolVal ("global");
    return;
  }
  if (name == "prompt") {
    d->promptDetect = boolVal ("prompt");
    return;
  }
  if ((name == "action-matched") || (name == "action-not-matched")) {
    int m = intVal (name);
    cList::TraverseAction act;
    switch (m) {
      case 0: act = cList::Continue; break;
      case 1: act = cList::Stop; break;
      case 2: act = cList::LeaveGroup; break;
      default: act = cList::Continue;
    }
    if (name == "action-matched")
      d->ifMatch = act;
    else
      d->ifNotMatch = act;
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
  // the other special trigger values would not be set/used so frequently, so
  // we will take them directly from the attributes when needed
}

cList::TraverseAction cTrigger::traverse (int traversalType)
{
  if (traversalType == TRIGGER_MATCH)
    return doMatch ();
  return cList::Stop;  // unknown action
}

cList::TraverseAction cTrigger::doMatch ()
{
  cTriggerList *tl = (cTriggerList *) list();

  // prompt-detect mode doesn't fit - do nothing
  bool detecting = tl->detectingPrompt();
  if (detecting != d->promptDetect)
    return cList::Continue;

  // fetch the string from the trigger list
  QString string = tl->stringToMatch();
  int mpos = 0;  // reset the index where matching will start
  //match against trigger
  bool everMatched = false;
  while (true) {
    if (!d->p.match (string, mpos)) break;  // match the string

    // matched
    bool cond = testCondition ();   // match the condition

    // also matched - execute this trigger !
    // but don't break the loop if the condition didn't match, as global matching
    // relies on that
    if (cond) {
      everMatched = true;
      executeTrigger ();
    }

    if (!d->global) break;  // only continue if it's global matching

    if (boolVal ("rewrite")) break;
    
    // global - update matching position, or terminate if no more matching should occur
    // if last length is 0, we must advance by 1 to avoid an endless loop ...
    int shift = (d->p.getLastLength() == 0) ? 1 : d->p.getLastLength();
    mpos = d->p.getLastPos() + shift;
    if (mpos >= string.length())
      break;
  }

  return everMatched ? d->ifMatch : d->ifNotMatch;
}

// TODO: this is duplicated for every place with conditions
// find out if we could create a common class for this
bool cTrigger::testCondition ()
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

void cTrigger::executeTrigger ()
{
  cActionManager *am = cActionManager::self();
  int sess = list()->session();
  cTextProcessor *textproc = dynamic_cast<cTextProcessor *>(am->object ("textproc", sess));
  cOutput *output = dynamic_cast<cOutput *>(am->object ("output", sess));
  cANSIParser *ansiparser = dynamic_cast<cANSIParser *>(am->object ("ansiparser", sess));
  
  d->commands.clear ();

  bool colorize = boolVal ("colorize");
  bool gag = boolVal ("gag");
  bool wantRewrite = boolVal ("rewrite");
  bool notifytrigger = boolVal ("notify");

  // colorize if needed
  if (colorize && ((!gag) || d->promptDetect))
    recolorize ();

  //rewrite the line if this is a rewrite trigger
  if (wantRewrite)
    rewrite ();

  // prompt detection - tell the processor that we have a prompt and end
  if (d->promptDetect)
  {
    textproc->setHavePrompt ();
    return;
  }

  // gag the line if needed
  if (gag)
    textproc->gagLine ();

  // notify the user if needed
  if (notifytrigger)
    am->invokeEvent ("notify-request", sess);

  // redirection
  if (boolVal ("output-window")) {
    textproc->setOutputWindow (strVal ("output-window-name"));
    if (boolVal ("output-gag-in-main"))
      textproc->gagLineInPrimary ();
  }

  //attempt to play the sound if needed
  if (boolVal ("sound") && (!(strVal ("sound-file").isEmpty())))
  {
    cSoundPlayer *player = dynamic_cast<cSoundPlayer *>(am->object ("soundplayer", 0));
    if (player)
    {
      player->setFileName (strVal ("sound-file"));
      player->setPriority (50);
      player->setRepeatsCount (1);
      player->setVolume (50);
      player->play ();
    }
  }

  int commands = strListCount ("newtext");
  // if we have only one empty command, we do nothing
  // prevents gag/notify/... triggers from sending empty lines to the server
  if ((commands > 1) || (!strListValue ("newtext", 1).isEmpty()))
  {

    // add commands to be sent
    for (int i = 1; i <= commands; ++i) {
      QString cmd = strListValue ("newtext", i);
      d->p.expandPseudoVariables (cmd);
      d->commands << cmd;
    }

    // we want to display, not send ?
    if (boolVal ("dont-send"))        
    {
      for (QStringList::iterator it = d->commands.begin(); it != d->commands.end(); ++it)
      {
        cTextChunk *chunk = cTextChunk::makeLine (*it, ansiparser->defaultTextColor(),
            ansiparser->defaultBkColor(), output->console());
        am->invokeEvent ("display-line", sess, chunk);
        delete chunk;
      }
      
      d->commands.clear ();    //don't send anything!
      return;
    }

    // we are done - send the commands
    cTriggerList *tl = (cTriggerList *) list();
    tl->processCommands (d->commands);
    d->commands.clear ();
  }

  // execute the script, if any
  QString script = strVal ("script");
  if (!script.isEmpty()) {
    cScriptEval *eval = dynamic_cast<cScriptEval *>(am->object ("scripteval", sess));
    if (eval) eval->eval (script, d->p.scriptVariables());
  }
}

// helper for colorize
QColor cTrigger::Private::getColor (int c)
{
  int r, g, b;
  b = c % 256;
  c /= 256;
  g = c % 256;
  c /= 256;
  r = c % 256;
  return QColor (r, g, b);
}

void cTrigger::recolorize ()
{
  cANSIParser *ansiparser = dynamic_cast<cANSIParser *>(cActionManager::self()->object ("ansiparser", list()->session()));
  cTextProcessor *textproc = dynamic_cast<cTextProcessor *>(cActionManager::self()->object ("textproc", list()->session()));
  
  std::list<colorChange> colors;
  
  // now go entry by entry
  int colorizationCount = intVal ("colorize-count");
  for (int clr = 1; clr <= colorizationCount; clr++)
  {
    QString sclr = QString::number (clr);
    QString varname = strVal ("colorize-variable-"+sclr).trimmed();
    //trim leading $ from varname
    if (varname[0] == '$') varname = varname.mid (1);    
    int fgc = intVal ("colorize-fg-"+sclr);
    int bgc = intVal ("colorize-bg-"+sclr);
    // sanity check
    if ((fgc < -16) || (fgc > 256*256*256)) fgc = 0;
    if ((bgc < -16) || (bgc > 256*256*256)) bgc = 0;

    if ((bgc == 0) && (fgc == 0)) continue;   // no change

    // get new colors
    QColor fg, bg;
    if (fgc > 0) {
      fg = d->getColor (fgc - 1);  // RGB value
    } else if (fgc < 0) {
      fg = ansiparser->color (fgc + 16);  // ANSI value
    }
    if (bgc > 0) {
      bg = d->getColor (bgc - 1);  // RGB value
    } else if (bgc < 0) {
      bg = ansiparser->color (bgc + 16);  // ANSI value
    }

    // positions
    int fromIndex, length;
    d->p.variablePosition (varname, &fromIndex, &length);
    if (fromIndex == -1)  //failed
      continue;
    
    //if we are here, colorization can occur!
    colorChange chg;
    chg.fg = fg;
    chg.bg = bg;
    chg.keepfg = (fgc == 0);
    chg.keepbg = (bgc == 0);
    chg.start = fromIndex;
    chg.len = length;
    colors.push_back (chg);
  }
  
  //okay, recolorize!
  textproc->recolorize (colors);
}

void cTrigger::rewrite ()
{
  QString varname = strVal ("rewrite-var");
  //trim leading $
  if (varname[0] == '$')
    varname = varname.mid (1);
  int fromIndex, length;
  d->p.variablePosition (varname, &fromIndex, &length);
  if (fromIndex == -1)
    return;
  
  QString rt = strVal ("rewrite-text");
  //expand pseudo-variables in the replacement text
  d->p.expandPseudoVariables (rt);
  //also expand real variables, if any
  cVariableList *vl = dynamic_cast<cVariableList *>(cActionManager::self()->object ("variables", list()->session()));
  rt = vl->expandVariables (rt, true);
  cTriggerList *tl = (cTriggerList *) list();
  tl->rewriteText (fromIndex, length, rt);
}


