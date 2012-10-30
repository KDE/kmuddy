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
#ifndef CMXPSTATE_H
#define CMXPSTATE_H

#include <list>
#include <map>
#include <string>

#include "libmxp.h"

using namespace std;

struct closingTag {
  //tag name (lowercase)
  string name;
  //closing result, if there's exactly one
  mxpResult *closingresult;
  //usually only zero or one element, but sometimes more :-)
  list<mxpResult *> *closingresults;
};

enum mxpMode {
  openMode = 0,
  secureMode,
  lockedMode
};

class cResultHandler;
class cElementManager;
class cEntityManager;

/**
This class provides most of the "intelligence" of the library - others feed it with what they
got, and this class decides what to send to the client and all that :-)

In particular:
Stores state variables of the MXP parser - current font/color/..., as well as a list of
currently open tags. Also provides conversion from tag properties to mxpState.
Also handles tag closing and auto-closing.

It's up to the caller to verify that bad things don't happen - errors will be sent if
secure tags are sent in open mode and so...

@author Tomas Mecir
*/

class cMXPState {
public:
  /** constructor */
  cMXPState (cResultHandler *resh, cElementManager *elm, cEntityManager *enm);
  /** destructor */
  ~cMXPState ();

//some user-adjustable parameters
  void setDefaultText (const string &font, int size, bool _bold, bool _italic, bool _underline,
      bool _strikeout, RGB fg, RGB bg);
  void setHeaderParams (int which, const string &font, int size, bool _bold, bool _italic,
      bool _underline, bool _strikeout, RGB fg, RGB bg);
  void setDefaultGaugeColor (RGB color);
  void setNonProportFont (string font);
  void setClient (string name, string version);

  //SUPPORTS stuff
  void supportsLink (bool supports);
  void supportsGauge (bool supports);
  void supportsStatus (bool supports);
  void supportsSound (bool supports);
  void supportsFrame (bool supports);
  void supportsImage (bool supports);
  void supportsRelocate (bool supports);

  /** switch current mode and default mode to OPEN - call this before processing any text, if
  you want to better conform to MXP, at the risk of problems with non-MXP MUDs
  No reason to use this on MXP-enabled MUDs though - they will go to secure mode anyway... */
  void switchToOpen ();

  //reset everything to default values
  void reset ();

//modes, mode switching
  /** return current mode */
  mxpMode getMXPMode ();
  /** switch mode; IMPORTANT: WILL NOT CLOSE TAGS OR ANYTHING!!! */
  void setMXPMode (mxpMode m);
  /** line tag received - act accordingly */
  void gotLineTag (int number);
  /** we should close all tags - called by gotLineTag, */
  void closeAllTags ();

//regular text
  /** got regular text */
  void gotText (const string &text, bool expandentities = true);
  /** got new-line character */
  void gotNewLine ();

//flags
  /** called when a flagged tag was received, either at its beginning or at its ending; the tag
  may not necessarily follow, as it's possible to flag some text without giving it any more
  attributes */
  void gotFlag (bool begin, string flag);

//value retrieving (so that we can supply default parameters)
  bool isBold () { return bold; };
  bool isItalic () { return italic; };
  bool isUnderline () { return underline; };
  bool isStrikeout () { return strikeout; };
  RGB fgColor () { return fgcolor; };
  RGB bgColor () { return bgcolor; };
  string fontFace () { return curfont; };
  int fontSize () { return cursize; };

//tags:
//variables
  /** called upon !ENTITY tag (erase==true means erase the variable) */
  void gotVariable (const string &name, const string &value, bool erase = false);
  /** called upon the VAR tag */
  void gotVAR (const string &name);

//text formatting (OPEN tags)
  void gotBOLD ();
  void gotITALIC ();
  void gotUNDERLINE ();
  void gotSTRIKEOUT ();
  void gotCOLOR (RGB fg, RGB bg);
  void gotHIGH ();
  void gotFONT (const string &face, int size, RGB fg, RGB bg);
//line spacing
  void gotNOBR ();
  void gotP ();
  void gotBR ();
  void gotSBR ();
//links
  /** got A-link; must wait for closing tag before sending anything to the client */
  void gotA (const string &href, const string &hint, const string &expire);
  /** got SEND-link; must wait for </SEND>, then expand &text; if needed */
  void gotSEND (const string &command, const string &hint, bool prompt, const string &expire);
  void gotEXPIRE (const string &name);
//version control
  void gotVERSION ();
  void gotSUPPORT (list<string> params);
//optional tags go next
//other HTML tags
  /** got one of H1, ..., H6 (thus, which is in range 1..6) */
  void gotHtag (int which);
  void gotHR ();
  void gotSMALL ();
  void gotTT ();
//MSP compatibility
  void gotSOUND (const string &fname, int vol, int count, int priority, const string &type,
      const string &url);
  void gotMUSIC (const string &fname, int vol, int count, bool contifrereq, const string &type,
      const string &url);
//gauges / status bars
  void gotGAUGE (const string &entity, const string &maxentity, const string &caption, RGB color);
  void gotSTAT (const string &entity, const string &maxentity, const string &caption);
//frames and cursor control
  void gotFRAME (const string &name, const string &action, const string &title, bool internal,
      const string &align, int left, int top, int width, int height, bool scrolling,
      bool floating);
  void gotDEST (const string &name, int x = -1, int y = -1, bool eol = false, bool eof = false);
//crosslinking servers
  void gotRELOCATE (const string &hostname, int port);
  void gotUSER ();
  void gotPASSWORD ();
//images
  void gotIMAGE (const string &fname, const string &url, const string &type, int height,
      int width, int hspace, int vspace, const string &align, bool ismap);

//closing tags
  /** we got a closing-tag; caller must ensure that everything is okay (i.e. such closing tag
  can exist, tag is elementary (not user-defined), we're in correct mode, ...)
  also, the name should be a shorter variant (e.g. "b", not "bold") */
  void gotClosingTag (const string &name);

//mxpResult handling
  /** create a closing mxpResult; must be called BEFORE applyResult() !!! */
  mxpResult *createClosingResult (mxpResult *what);
  /** apply this result */
  void applyResult (mxpResult *what);

//screen, window and font size, used by FRAME and IMAGE tags
  void setScreenProps (int sx, int sy, int wx, int wy, int fx, int fy);
  /** compute coordinated, input can contain 'c' or '%' as the last parameter */
  int computeCoord (const string &coord, bool isX = true, bool inWindow = false);
protected:
  /** close this tag :-) and delete the argument */
  void closeTag (closingTag *tag);
  /** add a closing tag to the list of pending closing tags */
  void addClosingTag (const string &name, mxpResult *res = 0, list<mxpResult *> *res2 = 0);

  /** stuff common for all tags */
  void commonTagHandler ();
  /** things to do after a tag is processed */
  void commonAfterTagHandler ();

  /** redirect output to frame <name> */
  void redirectTo (const string &name);

  cResultHandler *results;
  cElementManager *elements;
  cEntityManager *entities;

  /** list of closing tags */
  list<closingTag *> closingTags;

  /** list of existing frames */
  map<string, bool> frames;

  /** current mode */
  mxpMode mode;
  /** current default mode */
  mxpMode defaultmode;
  /** temporary secure mode? */
  bool tempMode;
  /** did we just leave secure mode?*/
  bool wasSecureMode;

  //all the parameters:

  //text attributes
  bool bold, italic, underline, strikeout;
  RGB fgcolor, bgcolor;
  string curfont;
  int cursize;
  //variables
  bool inVar;
  string varName, varValue;
  //paragraphs
  bool inParagraph;  //in P tag; no method returns this
  bool ignoreNextNewLine;  //after NOBR; no method returns this
  //links
  bool inLink, isALink;
  string lastcmd;
  string linkText;
  bool gotmap;
  //current window
  string curWindow, prevWindow;

  //initial LOCKED state?
  bool initiallyLocked;
  
  //currently implemented MXP version
  string mxpVersion;

  //user-defined values
  RGB defaultfg, defaultbg;
  string defaultfont;
  int defaultsize;
  char defaultattribs;

  string Hfont[6];
  RGB Hfg[6], Hbg[6];
  int Hsize[6];
  char Hattribs[6];
  string ttFont;
  RGB gaugeColor;

  string clientName, clientVersion;

  //SUPPORTS stuff...
  bool suplink, supgauge, supstatus, supsound, supframe, supimage, suprelocate;
  
  //screen and font parameters
  int sX, sY;      // width/height of the screen
  int wX, wY;      // width/height of the output window
  int fX, fY;      // width/height of character X
};

#endif
