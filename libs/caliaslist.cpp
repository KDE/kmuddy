//
// C++ Implementation: caliaslist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "caliaslist.h"

#include "calias.h"
#include "caliaseditor.h"
#include "cpattern.h"

#include <QStringList>

struct cAliasList::Private {
  QString stringToMatch;
  bool sendOriginal, matched;
  QStringList commandsToExec;
};

cAliasList::cAliasList () :
    cList ("aliases")
{
  // create properties
  addStringProperty ("pattern", "Matching pattern");
  addIntProperty ("newtext-count", "Replacement line count", 0);
  // then we have a "newtext-"+i string for each
  addIntProperty ("matching", "Comparison type", int(cPattern::begin));
  addBoolProperty ("cs", "Case sensitive", true);
  addBoolProperty ("prefix-suffix", "Include prefix/suffix", true);
  addBoolProperty ("orig", "Send original", false);
  addBoolProperty ("whole-words", "Whole words", true);
  addBoolProperty ("global", "Global matching", false);
  addStringProperty ("condition", "Matching condition");

  // script
  addStringProperty ("script", "Script to execute");

  d = new Private;
}

cAliasList::~cAliasList ()
{
  delete d;
}

cListObject *cAliasList::newObject ()
{
  return new cAlias (this);
}

cListEditor *cAliasList::editor (QWidget *parent)
{
  return new cAliasEditor (parent);
}

bool cAliasList::matchString (const QString &string)
{
  d->sendOriginal = false;
  d->matched = false;
  d->stringToMatch = string;
  d->commandsToExec.clear ();

  // traverse the structure
  traverse (ALIAS_MATCH);

  if (d->sendOriginal)
    d->commandsToExec.prepend (string);

  return d->matched;
}

QStringList cAliasList::commandsToExec ()
{
  return d->commandsToExec;
}

QString cAliasList::stringToMatch ()
{
  return d->stringToMatch;
}
  
void cAliasList::wantOriginalCommand ()
{
  d->sendOriginal = true;
}

void cAliasList::setMatched ()
{
  d->matched = true;
}

void cAliasList::addCommand (const QString &command)
{
  d->commandsToExec << command;
}

