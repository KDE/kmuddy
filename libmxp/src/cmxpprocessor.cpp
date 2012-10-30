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
#include "cmxpprocessor.h"

#include "cresulthandler.h"
#include "cmxpstate.h"
#include "cmxpparser.h"
#include "centitymanager.h"
#include "celementmanager.h"

cMXPProcessor::cMXPProcessor ()
{
  //create all the objects...
  results = new cResultHandler;
  entities = new cEntityManager;
  elements = new cElementManager (0, results, entities);
  state = new cMXPState (results, elements, entities);
  elements->assignMXPState (state);
  parser = new cMXPParser (state, elements, results);
}

cMXPProcessor::~cMXPProcessor ()
{
  delete parser;
  parser = 0;
  delete state;
  state = 0;
  delete elements;
  elements = 0;
  delete entities;
  entities = 0;
  delete results;
  results = 0;
}

void cMXPProcessor::processText (const char *text)
{
  if (!text)
    return;
  parser->parse (text);
}

mxpResult *cMXPProcessor::nextResult ()
{
  return results->nextResult ();
}

bool cMXPProcessor::hasResults ()
{
  return results->haveResults ();
}

void cMXPProcessor::switchToOpen ()
{
  state->switchToOpen ();
}

void cMXPProcessor::setDefaultText (const char *font, int size, bool _bold, bool _italic,
    bool _underline, bool _strikeout, RGB fg, RGB bg)
{
  state->setDefaultText (font, size, _bold, _italic, _underline, _strikeout, fg, bg);
}

void cMXPProcessor::setHeaderParams (int which, const char *font, int size, bool _bold,
    bool _italic, bool _underline, bool _strikeout, RGB fg, RGB bg)
{
  state->setHeaderParams (which, font, size, _bold, _italic, _underline, _strikeout, fg, bg);
}

void cMXPProcessor::setDefaultGaugeColor (RGB color)
{
  state->setDefaultGaugeColor (color);
}

void cMXPProcessor::setNonProportFont (const char *font)
{
  state->setNonProportFont (font);
}

void cMXPProcessor::setClient (const char *name, const char *version)
{
  state->setClient (name, version);
}

void cMXPProcessor::setScreenProps (int sx, int sy, int wx, int wy, int fx, int fy)
{
  state->setScreenProps (sx, sy, wx, wy, fx, fy);
}

void cMXPProcessor::supportsLink (bool supports)
{
  state->supportsLink (supports);
}

void cMXPProcessor::supportsGauge (bool supports)
{
  state->supportsGauge (supports);
}

void cMXPProcessor::supportsSound (bool supports)
{
  state->supportsSound (supports);
}

void cMXPProcessor::supportsStatus (bool supports)
{
  state->supportsStatus (supports);
}

void cMXPProcessor::supportsFrame (bool supports)
{
  state->supportsFrame (supports);
}

void cMXPProcessor::supportsImage (bool supports)
{
  state->supportsImage (supports);
}

void cMXPProcessor::supportsRelocate (bool supports)
{
  state->supportsRelocate (supports);
}
