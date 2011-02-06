/***************************************************************************
                          csaveablefield.h  -  alias/trigger baseclass
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So sep 7 2002
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

#ifndef CSAVEABLEFIELD_H
#define CSAVEABLEFIELD_H

#include <QString>
#include <QStringList>

#include "cpattern.h"

class KConfig;

/**
Abstract base-class for alias / trigger objects (and more).
Knows how to save itself.

This class is arranged in a double-linked list.

  *@author Tomas Mecir
  */

//object types
#define TYPE_ALIAS 1
#define TYPE_TRIGGER 2
#define TYPE_ACTION 3
#define TYPE_GROUP 4
#define TYPE_SCRIPT 5
#define TYPE_TIMER 6
#define TYPE_SHORTCUT 7
#define TYPE_VARIABLE 8
#define TYPE_VARTRIG 9
#define TYPE_TIMERTICK 10
#define TYPE_GAUGE 11
#define TYPE_STATUSVAR 12

class cSaveableField {
public:
  friend class cSaveableList;

  /** constructor */
  cSaveableField ();
  /** destructor */
  virtual ~cSaveableField ();

  /** creates a new instance of the class; this is needed because I need to
  create instances of childclasses from within this class, but I don't know
  the exact type of that instance... */
  virtual cSaveableField *newInstance () = 0;

  /** abstract; load data from a config file*/
  virtual void load (KConfig *config, const QString &group) = 0;

  /** abstract; returns type of item (light-weight RTTI) */
  virtual int itemType () = 0;

  /** set alias/trigger text */
  void setText (const QString &_text) { p.setPattern (_text); };
  /** returns current alias/trigger text */
  QString getText () { return p.pattern(); };

  /** set alias/trigger replacement(s)/command(s) */
  void setNewText (const QStringList &_text) { newtext = _text; } ;
  /** returns current alias/trigger replacement/command */
  QStringList getNewText () { return newtext; };

  void setGlobal (bool g) { globalmatch = g; };
  bool global () { return globalmatch; };

  /** set a new condition for matching */
  void setCond (const QString &c);
  /** return the condition */
  QString cond () { return condition; };

  void setCaseSensitive (bool cs) { p.setCaseSensitive (cs); };
  bool caseSensitive () { return p.caseSensitive(); };

  /** set comparison type */
  void setType (int t) { p.setMatching ((cPattern::PatternType) t); };
  /** returns comparison type */
  int getType () { return (int) p.matching(); };
  /** possible types of comparison */
  enum compType { exact, substring, begin, end, regexp };
  #define TYPES 5
protected:
  cPattern p;

  /** fields form a double-linked list. These are some other entries... */
  cSaveableField *prev, *next;

  /** alias replacement(s) or triggered command(s) */
  QStringList newtext;
  /** condition to match */
  QString condition;

  /** global matching */
  bool globalmatch;
};

#endif
