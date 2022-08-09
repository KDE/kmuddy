/***************************************************************************
                          ctrigger.h  -  trigger
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne okt 13 2002
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

#ifndef CTRIGGER_H
#define CTRIGGER_H

#include "csaveablefield.h"

class cTriggerList;

#include <qcolor.h>

#include <map>

#define MAX_COLORIZATIONS 10

using namespace std;

/**
One trigger.
  *@author Tomas Mecir
  */

class cTrigger : public cSaveableField  {
public:
  cTrigger (int _sess);
  ~cTrigger () override;

  /** creates a new instance of the class */
  cSaveableField *newInstance () override;

  /** load data from a config file*/
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_TRIGGER; };

  bool dontSend () { return dontsend; };
  void setDontSend (bool what) { dontsend = what; };

  bool continueIfMatch () { return continueifmatch; };
  bool continueIfNoMatch () { return continueifnomatch; };

  bool isColorTrigger () { return colortrigger; };
  void setColorTrigger (bool what) { colortrigger = what; };
  bool isGagTrigger () { return gagtrigger; };
  void setGagTrigger (bool what) { gagtrigger = what; };
  bool isRewriteTrigger () { return rewritetrigger; };
  void setRewriteTrigger (bool what) { rewritetrigger = what; };
  bool isNotifyTrigger () { return notifytrigger; };
  void setNotifyTrigger (bool what) { notifytrigger = what; };
  bool isPromptDetectTrigger () { return prompttrigger; };
  void setPromptDetectTrigger (bool what) { prompttrigger = what; };
  bool isSoundTrigger () { return soundtrigger; };
  void setSoundTrigger (bool what) { soundtrigger = what; };

  void setOutputWindowTrigger (bool what) { outputwindowtrigger = what; };
  bool isOutputWindowTrigger() { return outputwindowtrigger; };
  
  void setGagOutputWindow (bool what) { gagoutputwindow = what; };
  bool isGagOutputWindow() { return gagoutputwindow; };

  void setOutputWindowName (QString text) { outputwindowname = text; };
  QString getOutputWindowName() { return outputwindowname; };
  
  void clearColorizations ();
  bool addColorization (QString pseudovar, int newcolors, QColor fgc, QColor bgc);
  int getColorizationsCount () { return colorizationCount; };
  QString getColorizationVariable (int num);
  int getColorizationColor (int num);
  QColor getColorizationFg (int num);
  QColor getColorizationBg (int num);

  QString rewriteVar () { return rewritevar; };
  QString rewriteText () { return rewritetext; };
  void setRewriteVar (const QString &val) { rewritevar = val; };
  void setRewriteText (const QString &val) { rewritetext = val; };

  const QString &soundFileName () { return soundFName; };
  void setSoundFileName (const QString & fName) { soundFName = fName; };
protected:
  
  bool continueifmatch, continueifnomatch;
  bool dontsend;
  
  bool colortrigger, gagtrigger, notifytrigger, prompttrigger, rewritetrigger;
  bool soundtrigger;
  bool outputwindowtrigger;
  bool gagoutputwindow;
  QString outputwindowname;
  
  int colorizationCount;
  QString colorizationVar[MAX_COLORIZATIONS];
  int colorization[MAX_COLORIZATIONS];
  QColor fgcol[MAX_COLORIZATIONS], bgcol[MAX_COLORIZATIONS];

  /** what to rewrite, and with what */
  QString rewritevar, rewritetext;
  
  QString soundFName;

  /** used by colorization and rewrite triggers (in computePosition()) */
  int fromIndex, length;

  int sess;
};

#endif
