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
#ifndef CMXPPARSER_H
#define CMXPPARSER_H

#include <list>
#include <string>

using namespace std;

/**
First-stage parser, it looks for newlines, elements and line tags.

@author Tomas Mecir
*/

enum chunkType {
  chunkNone = 0,
  chunkText,
  chunkTag,
  chunkError
};

enum parserState {
  pText = 0,
  pAnsiSeq,
  pTag,
  pComment,
  pQuotedParam
};

struct chunk {
  chunkType chk;
  string text;
};

class cMXPState;
class cElementManager;
class cResultHandler;

class cMXPParser {
public:
  /** constructor */
  cMXPParser (cMXPState *st = 0, cElementManager *elm = 0, cResultHandler *res = 0);
  /** destructor */
  ~cMXPParser ();

  void reset ();
    
  /** parse text and send chunks to the correct classes for further processing */
  void parse (const string &text);
  
  /** simple parser - only recognizes text and tags - used to parse !ELEMENT tag */
  void simpleParse (const string &text);
  /** do we have more things to report? used after simpleParse() */
  bool hasNext ();
  /** next chunk; used in conjunction with simpleParse() */
  chunk getNext ();
  
protected:
  cMXPState *state;
  cElementManager *elements;
  cResultHandler *results;
  
  string str;
  parserState pstate;
  list<chunk> chunks;
  char quoteChar;
  bool wasBackslashR;
};

#endif
