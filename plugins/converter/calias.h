/***************************************************************************
                          calias.h  -  One alias
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Út sep 10 2002
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

#ifndef CALIAS_H
#define CALIAS_H

#include "csaveablefield.h"

/**
This class represents one alias. It is derived from cSaveableField, and thus
it knows how to load/save itself and how to perform alias matching (all sorts).
  *@author Tomas Mecir
  */

class cAlias : public cSaveableField  {
public:
  cAlias (int _sess);
  virtual ~cAlias ();

  /** creates a new instance of the class */
  virtual cSaveableField *newInstance ();

  /** load data from a config file*/
  virtual void load (KConfig *config, const QString &group);

  /** returns type of item (light-weight RTTI) */
  virtual int itemType () { return TYPE_ALIAS; };

  bool sendOriginal () { return sendoriginal; };
  void setSendOriginal (bool what) { sendoriginal = what; };
  bool wholeWords () { return wholewords; };
  void setWholeWords (bool what) { wholewords = what; };
  bool includePrefixSuffix () { return includeprefixsuffix; };
  void setIncludePrefixSuffix (bool what) { includeprefixsuffix = what; };
protected:
  bool sendoriginal;
  bool wholewords;
  bool includeprefixsuffix;
    
  int sess;
};

#endif
