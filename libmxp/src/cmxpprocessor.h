/***************************************************************************
 *   Copyright (C) 2004 by Tomas Mecir                                     *
 *   kmuddy@kmuddy.org                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 ***************************************************************************/
#ifndef CMXPPROCESSOR_H
#define CMXPPROCESSOR_H

#include "libmxp.h"

class cResultHandler;
class cMXPState;
class cMXPParser;
class cEntityManager;
class cElementManager;

/**
This class wraps the public API of the library.

@author Tomas Mecir
*/
class cMXPProcessor {
public:
  cMXPProcessor ();
  ~cMXPProcessor ();

  /** text processing */
  void processText (const char *text);
  /** fetch next result of the processing */
  mxpResult *nextResult ();
  /** are there any more results to fetch? */
  bool hasResults ();
  
  /** switch to OPEN mode - use this immediately after creating this object, in case that
  you don't want to be in LOCKED mode by default (where MXP is only used if some line tag
  is received) */
  void switchToOpen ();
  
  /** set default text attributes */
  void setDefaultText (const char *font, int size, bool _bold, bool _italic, bool _underline,
      bool _strikeout, RGB fg, RGB bg);
  /** set attributes of header 1-6 (for tags <h1>-<h6> */
  void setHeaderParams (int which, const char *font, int size, bool _bold, bool _italic,
      bool _underline, bool _strikeout, RGB fg, RGB bg);
  /** set default color for gauges */
  void setDefaultGaugeColor (RGB color);
  /** set used non-proportional font */
  void setNonProportFont (const char *font);
  /** set client name and version reported to the MUD */
  void setClient (const char *name, const char *version);
  /* screen, window and font size, used by FRAME and IMAGE tags */
  void setScreenProps (int sx, int sy, int wx, int wy, int fx, int fy);
  
  //SUPPORTS stuff
  void supportsLink (bool supports);
  void supportsGauge (bool supports);
  void supportsStatus (bool supports);
  void supportsSound (bool supports);
  void supportsFrame (bool supports);
  void supportsImage (bool supports);
  void supportsRelocate (bool supports);
  
protected:
  cResultHandler *results;
  cMXPState *state;
  cMXPParser *parser;
  cEntityManager *entities;
  cElementManager *elements;
};

#endif
