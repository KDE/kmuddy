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

#include "libmxp.h"
#include "cmxpprocessor.h"

#ifdef __cplusplus
extern "C" {
#endif 

MXPHANDLER mxpCreateHandler ()
{
  cMXPProcessor *proc = new cMXPProcessor;
  return (MXPHANDLER) proc;
}

void mxpDestroyHandler (MXPHANDLER handler)
{
  delete (cMXPProcessor *) handler;
}

void mxpProcessText (MXPHANDLER handler, const char *text)
{
  ((cMXPProcessor *) handler)->processText (text);
}

mxpResult *mxpNextResult (MXPHANDLER handler)
{
  return ((cMXPProcessor *) handler)->nextResult ();
}

char mxpHasResults (MXPHANDLER handler)
{
  return ((cMXPProcessor *) handler)->hasResults ();
}

void mxpSwitchToOpen (MXPHANDLER handler)
{
  ((cMXPProcessor *) handler)->switchToOpen ();
}

void mxpSetDefaultText (MXPHANDLER handler, const char *font, int size, char _bold,
    char _italic, char _underline, char _strikeout, RGB fg, RGB bg)
{
  ((cMXPProcessor *) handler)->setDefaultText (font, size, _bold, _italic, _underline,
      _strikeout, fg, bg);
}

void mxpSetHeaderParams (MXPHANDLER handler, int which, const char *font, int size, char _bold,
    char _italic, char _underline, char _strikeout, RGB fg, RGB bg)
{
  ((cMXPProcessor *) handler)->setHeaderParams (which, font, size, _bold, _italic, _underline,
      _strikeout, fg, bg);
}

void mxpSetDefaultGaugeColor (MXPHANDLER handler, RGB color)
{
  ((cMXPProcessor *) handler)->setDefaultGaugeColor (color);
}

void mxpSetNonProportFont (MXPHANDLER handler, const char *font)
{
  ((cMXPProcessor *) handler)->setNonProportFont (font);
}

void mxpSetClient (MXPHANDLER handler, const char *name, const char *version)
{
  ((cMXPProcessor *) handler)->setClient (name, version);
}

void mxpSetScreenProps (MXPHANDLER handler, int sx, int sy, int wx, int wy, int fx, int fy)
{
  ((cMXPProcessor *) handler)->setScreenProps (sx, sy, wx, wy, fx, fy);
}

//things that the client supports...

void mxpSupportsLink (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsLink (supports);
}

void mxpSupportsGauge (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsGauge (supports);
}

void mxpSupportsStatus (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsStatus (supports);
}

void mxpSupportsSound (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsStatus (supports);
}

void mxpSupportsFrame (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsFrame (supports);
}

void mxpSupportsImage (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsImage (supports);
}

void mxpSupportsRelocate (MXPHANDLER handler, char supports)
{
  ((cMXPProcessor *) handler)->supportsRelocate (supports);
}

#ifdef __cplusplus
};
#endif 
