/***************************************************************************
                          calias.cpp  -  One alias
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : sep 10 2002
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

#include "calias.h"

#include <kconfig.h>
#include <kconfiggroup.h>

cAlias::cAlias (int _sess) : sess(_sess)
{
  //default compare type is "begins with"
  setType (begin);
  //do not send original command by default
  sendoriginal = false;
  //whole words expansion
  wholewords = true;
  //include prefix/suffix
  includeprefixsuffix = true;
}

cAlias::~cAlias ()
{
  //nothing here
}

cSaveableField *cAlias::newInstance ()
{
  return new cAlias (sess);
}

void cAlias::load (KConfig *config, const QString &group)
{
  KConfigGroup g = config->group (group);
  setText (g.readEntry ("Text", ""));

  //ntxt: for compatibility with KMuddy <= 0.6pre1
  QString ntxt = g.readEntry ("Replacement text", QString());

  newtext.clear();
  int repcount = g.readEntry ("Replacement count", -1);
  if (repcount == -1)   //use old-style replacement
    newtext.push_back (ntxt);
  else    //new-style replacement
    for (int i = 1; i <= repcount; i++)
    {
      QString repline = g.readEntry ("Replacement line " +
            QString::number (i), QString());
      newtext.push_back (repline); 
    }

  setType (g.readEntry ("Type", int(substring)));
  setCaseSensitive (g.readEntry ("Case sensitive", true));
  includeprefixsuffix = g.readEntry ("Include prefix suffix", true);
  sendoriginal = g.readEntry ("Send original", false);
  wholewords = g.readEntry ("Whole words", true);
  globalmatch = g.readEntry ("Global matching", false);
  setCond (g.readEntry ("Condition", QString()));
}


