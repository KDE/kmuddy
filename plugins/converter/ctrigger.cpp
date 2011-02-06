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

#include <kconfig.h>
#include <kconfiggroup.h>

cTrigger::cTrigger (int _sess) : sess(_sess)
{
  //default compare type is substring
  setType (substring);

  continueifmatch = false;
  continueifnomatch = true;
  dontsend = false;

  colortrigger = false;
  clearColorizations ();

  gagtrigger = false;
  notifytrigger = false;
  prompttrigger = false;
  rewritetrigger = false;
  soundtrigger = false;

  outputwindowtrigger = false;
  gagoutputwindow = false;
}

cTrigger::~cTrigger ()
{
  //nothing here
}

cSaveableField *cTrigger::newInstance ()
{
  return new cTrigger (sess);
}

void cTrigger::load (KConfig *config, const QString &group)
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
            QString::number (i), "");
      newtext.push_back (repline);
    }

  setType (g.readEntry ("Type", (int)substring));
  continueifmatch = g.readEntry ("Continue if match", false);
  continueifnomatch = g.readEntry ("Continue if no match", true);
  dontsend = g.readEntry ("Show dont send", false);
  setCaseSensitive (g.readEntry ("Case sensitive", true));
  globalmatch = g.readEntry ("Global matching", false);
  setCond (g.readEntry ("Condition", QString()));

  clearColorizations ();
  colortrigger = g.readEntry ("Color trigger", false);
  if (colortrigger)
  {
    int cnum = g.readEntry ("Colorizations", 0);
    //ensure that cnum is correct
    if (cnum > MAX_COLORIZATIONS)
      cnum = MAX_COLORIZATIONS;
    if (cnum < 0)
      cnum = 0;
    for (int i = 0; i < cnum; i++)
    {
      QString cname = g.readEntry ("Colorization variable " +
            QString::number(i + 1), "");
      int colors = g.readEntry ("Colorization colors " +
            QString::number(i + 1), 255 * 256 + 255);
      QColor fgc = g.readEntry ("Colorization fg " +
            QString::number(i + 1), (QColor) Qt::black);
      QColor bgc = g.readEntry ("Colorization bg " +
            QString::number(i + 1), (QColor) Qt::black);
      addColorization (cname, colors, fgc, bgc);
    }
  }

  rewritetrigger = g.readEntry ("Rewrite trigger", false);
  rewritevar = g.readEntry ("Rewrite variable", QString());
  rewritetext = g.readEntry ("Rewrite new text", QString());
  
  gagtrigger = g.readEntry ("Gag trigger", false);
  notifytrigger = g.readEntry ("Notify trigger", false);
  prompttrigger = g.readEntry ("Prompt trigger", false);
  soundtrigger = g.readEntry ("Sound trigger", false);
  soundFName = g.readEntry ("Sound file name", QString());

  outputwindowtrigger = g.readEntry ("Send output to separate window", false);
  gagoutputwindow = g.readEntry ("Gag main window in favour of output", false);
  outputwindowname = g.readEntry ("Output window name", QString());
}

void cTrigger::clearColorizations ()
{
  colorizationCount = 0;
  for (int i = 0; i < MAX_COLORIZATIONS; i++)
  {
    colorizationVar[i] = QString();
    colorization[i] = 255 * 256 + 255;
    fgcol[i] = Qt::black;
    bgcol[i] = Qt::black;
  }
}

bool cTrigger::addColorization (QString pseudovar, int newcolors, QColor fgc, QColor bgc)
{
  if (colorizationCount >= MAX_COLORIZATIONS)
    return false;   //NO MORE FREE SPACE!!!
  colorizationVar[colorizationCount] = pseudovar;
  colorization[colorizationCount] = newcolors;
  fgcol[colorizationCount] = fgc;
  bgcol[colorizationCount] = bgc;
  
  colorizationCount++;
  return true;
}

QString cTrigger::getColorizationVariable (int num)
{
  if ((num < 0) || (num >= MAX_COLORIZATIONS))
    return QString();
  return colorizationVar[num];
}

int cTrigger::getColorizationColor (int num)
{
  if ((num < 0) || (num >= MAX_COLORIZATIONS))
    return 255 * 256 + 255;
  return colorization[num];
}

QColor cTrigger::getColorizationFg (int num)
{
  if ((num < 0) || (num >= MAX_COLORIZATIONS))
    return Qt::black;
  return fgcol[num];
}

QColor cTrigger::getColorizationBg (int num)
{
  if ((num < 0) || (num >= MAX_COLORIZATIONS))
    return Qt::black;
  return bgcol[num];
}


