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
#include "cmxpstate.h"

#include "centitymanager.h"
#include "cmxpcolors.h"
#include "cresulthandler.h"

#include "rgbops.h"
#include "stringops.h"

#include <config.h>

#include <algorithm>
#include <stdlib.h>
#include <string.h>

cMXPState::cMXPState (cResultHandler *resh, cElementManager *elm, cEntityManager *enm)
{
  results = resh;
  elements = elm;
  entities = enm;

  //currently implemented MXP version
  mxpVersion = "1.0";

  //starting MXP mode is LOCKED, to prevent problems with non-MXP MUDs).
  //This goes against the MXP protocol, therefore there's a setting that will keep the OPEN
  //mode, if desired - see public API.
  mode = lockedMode;
  defaultmode = lockedMode;
  initiallyLocked = true;
  tempMode = false;
  wasSecureMode = false;
  
  //some default values...
  cMXPColors *colors = cMXPColors::self();
  defaultfg = colors->color ("gray");
  defaultbg = colors->color ("black");
  defaultfont = "Courier";
  defaultsize = 12;
  defaultattribs = 0;
  //by default, all headers are written in the same font (Courier), they are bold and they
  //differ in sizes...
  for (int i = 0; i < 6; i++)
  {
    Hfont[i] = "Courier";
    Hfg[i] = defaultfg;
    Hbg[i] = defaultbg;
    Hattribs[i] = Bold;
  }
  Hsize[0] = 32;
  Hsize[1] = 24;
  Hsize[2] = 20;
  Hsize[3] = 16;
  Hsize[4] = 14;
  Hsize[5] = 12;
  ttFont = "Courier";
  setDefaultGaugeColor (colors->color ("white"));
  //PACKAGE and VERSION are defined in config.h
  clientName = PACKAGE;
  clientVersion = VERSION;
  //some default screen and font attributes...
  fX = 16;
  fY = 8;
  sX = 800;
  sY = 600;
  
  suplink = supgauge = supstatus = supframe = supimage = suprelocate = false;
  
  //params
  reset ();
}


cMXPState::~cMXPState ()
{
  //delete mxpResult structures in closing tags list
  list<closingTag *>::iterator it;
  for (it = closingTags.begin(); it != closingTags.end(); ++it)
  {
    if ((*it)->closingresult)
      delete (*it)->closingresult;
    list<mxpResult *> *rlist = (*it)->closingresults;
    if (rlist)
    {
      list<mxpResult *>::iterator it2;
      for (it2 = rlist->begin(); it2 != rlist->end(); ++it2)
        delete *it2;
      delete rlist;
    }
  }
  closingTags.clear ();
}

//some user-adjustable parameters

void cMXPState::setDefaultText (const string &font, int size, bool _bold, bool _italic,
    bool _underline, bool _strikeout, RGB fg, RGB bg)
{
  if (curfont == defaultfont) curfont = font;
  defaultfont = font;

  if (cursize == defaultsize) cursize = size;
  defaultsize = size;

  char curattrib = (bold?1:0) * Bold + (italic?1:0) * Italic +
      (underline?1:0) * Underline + (strikeout?1:0) * Strikeout;
  char newattribs = (_bold?1:0) * Bold + (_italic?1:0) * Italic +
      (_underline?1:0) * Underline + (_strikeout?1:0) * Strikeout;
  if (curattrib == defaultattribs)
  {
    bold = _bold;
    italic = _italic;
    underline = _underline;
    strikeout = _strikeout;
  }
  defaultattribs = newattribs;

  if (fgcolor == defaultfg) fgcolor = fg;
  defaultfg = fg;
  if (bgcolor == defaultbg) bgcolor = bg;
  defaultbg = bg;
}

void cMXPState::setHeaderParams (int which, const string &font, int size, bool _bold, bool _italic,
    bool _underline, bool _strikeout, RGB fg, RGB bg)
{
  //invalid H-num?
  if ((which < 1) || (which > 6))
    return;

  Hfont[which - 1] = font;

  Hsize[which - 1] = size;

  char newattribs = (_bold?1:0) * Bold + (_italic?1:0) * Italic +
      (_underline?1:0) * Underline + (_strikeout?1:0) * Strikeout;
  Hattribs[which - 1] = newattribs;

  Hfg[which - 1] = fg;
  Hbg[which - 1] = bg;
}

void cMXPState::setDefaultGaugeColor (RGB color)
{
  gaugeColor = color;
}

void cMXPState::setNonProportFont (string font)
{
  ttFont = font;
}

void cMXPState::setClient (string name, string version)
{
  clientName = name;
  clientVersion = version;
}

void cMXPState::supportsLink (bool supports)
{
  suplink = supports;
}

void cMXPState::supportsGauge (bool supports)
{
  supgauge = supports;
}

void cMXPState::supportsStatus (bool supports)
{
  supstatus = supports;
}

void cMXPState::supportsSound (bool supports)
{
  supsound = supports;
}

void cMXPState::supportsFrame (bool supports)
{
  supframe = supports;
}

void cMXPState::supportsImage (bool supports)
{
  supimage = supports;
}

void cMXPState::supportsRelocate (bool supports)
{
  suprelocate = supports;
}

void cMXPState::switchToOpen ()
{
  mode = openMode;
  defaultmode = openMode;
  initiallyLocked = false;
  //not we conform to MXP spec... use with care - only affects non-MXP MUDs, where it allows
  //open tags - MUDs supporting MXP are NOT affected
}

void cMXPState::reset ()
{
  bold = defaultattribs & Bold;
  italic = defaultattribs & Italic;
  underline = defaultattribs & Underline;
  strikeout = defaultattribs & Strikeout;
  fgcolor = defaultfg;
  bgcolor = defaultbg;
  curfont = defaultfont;
  cursize = defaultsize;
  inVar = false;
  varValue = "";
  inParagraph = false;
  ignoreNextNewLine = false;
  inLink = false;
  isALink = false;
  linkText = "";
  gotmap = false;
  curWindow = "";
  prevWindow = "";
  
}

//modes, mode switching

mxpMode cMXPState::getMXPMode ()
{
  return mode;
}

void cMXPState::setMXPMode (mxpMode m)
{
  mode = m;
  tempMode = false;
  wasSecureMode = false;
  
  //if we start in LOCKED mode and mode change occurs, we set default mode
  //to OPEN, so that we are compatible with the spec...
  if (initiallyLocked)
  {
    initiallyLocked = false;
    defaultmode = openMode;
  }
}

void cMXPState::gotLineTag (int number)
{
  //got a line tag - close outstanding entities, if any (unless we're in LOCKED mode)
  if (mode != lockedMode)
  {
    string t = entities->expandEntities ("", true);
    if (!t.empty())
      gotText (t, false);
  }

  //leaving secure mode
  if (wasSecureMode && (number != 1))
    closeAllTags ();
  wasSecureMode = false;

  if (number < 0) return;
  if (number > 99) return;
  if (number >= 10)
    results->addToList (results->createLineTag (number));
  else
  {
    switch (number) {
      case 0:
        setMXPMode (openMode);
        break;
      case 1:
        setMXPMode (secureMode);
        break;
      case 2:
        setMXPMode (lockedMode);
        break;
      case 3:
        closeAllTags ();
        //default mode remains the same...
        setMXPMode (openMode);
        reset ();
        break;
      case 4:
        setMXPMode (secureMode);
        tempMode = true;
        break;
      case 5:
        setMXPMode (openMode);
        defaultmode = openMode;
        break;
      case 6:
        setMXPMode (secureMode);
        defaultmode = secureMode;
        break;
      case 7:
        setMXPMode (lockedMode);
        defaultmode = lockedMode;
        break;
      default:
        results->addToList (results->createWarning ("Received unrecognized line tag."));
        break;
    };
  }
}

void cMXPState::closeAllTags ()
{
  if (closingTags.empty())
    return;

  //process open tags one by one...
  while (!closingTags.empty())
  {
    //closingTags is a FIFO queue, tho technically it's a list
    closingTag *tag = closingTags.back ();
    closingTags.pop_back ();

    results->addToList (results->createWarning ("Had to auto-close tag " + tag->name + "."));
    
    closeTag (tag);
  }
}

void cMXPState::commonTagHandler ()
{
  //got a new tag - close outstanding entities, if any (unless we're in LOCKED mode)
  if (mode != lockedMode)
  {
    string t = entities->expandEntities ("", true);
    if (!(t.empty()))
      gotText (t, false);
  }

  //outstanding tags are closed, if we're going out of secure mode, unless a change back to secure
  //mode occurs
  if (wasSecureMode)
  {
    closeAllTags ();
    wasSecureMode = false;
  }

  //error is reported, if we're inside VAR...
  if (inVar)
    results->addToList (results->createError ("Got a tag inside a variable!"));
}

void cMXPState::commonAfterTagHandler ()
{
  //secure mode for one tag?
  if (tempMode)
  {
    tempMode = false;
    //set mode back to default mode
    mode = defaultmode;
  }
}

//regular text

void cMXPState::gotText (const string &text, bool expandentities)
{
  if (text.length() == 0)
    return;
  //temp-secure mode -> ERROR!
  if (tempMode)
  {
    tempMode = false;
    mode = defaultmode;
    results->addToList (results->createError ("Temp-secure line tag not followed by a tag!"));
  }

  //outstanding tags are closed, if we're going out of secure mode, unless a change back to secure
  //mode occurs
  if (wasSecureMode)
  {
    closeAllTags ();
    wasSecureMode = false;
  }

  //expand entities, if needed
  string t;
  if (expandentities && (mode != lockedMode))
    t = entities->expandEntities (text, false);
  else
    t = text;

  //special handling if we're in a variable or a link
  if (inVar)
    varValue.append (t);
  if (inLink)
    linkText.append (t);

  //text can be sent is it's not a part of a link or of a variable
  if (!(inVar || inLink))
    //add text to the list of things to send
    results->addToList (results->createText (t));
}

void cMXPState::gotNewLine ()
{
  //got a newline char - close outstanding entities, if any (unless we're in LOCKED mode)
  if (mode != lockedMode)
  {
    string t = entities->expandEntities ("", true);
    if (!t.empty())
      gotText (t, false);
  }

  //was temp-secure mode?
  if (tempMode)
  {
    tempMode = false;
    mode = defaultmode;
    results->addToList (results->createError ("Temp-secure line tag followed by a newline!"));
  }

  //leaving secure mode?
  wasSecureMode = false;
  if ((mode == secureMode) && (defaultmode != secureMode))
    wasSecureMode = true;

  //ending line in OPEN mode - close all tags!
  if (mode == openMode)
    closeAllTags ();

  //is we're in SECURE mode, some tags may need to be closed...

  //line ended inside a link
  if (inLink)
  {
    inLink = false;
    isALink = false;
    linkText = "";
    results->addToList (results->createError ("Received an unterminated link!"));
  }

  if (inVar)
  {
    inVar = false;
    results->addToList (results->createError ("Received an unterminated VAR tag!"));
    varValue = "";
  }

  //should next newline be ignored?
  if (ignoreNextNewLine)
  {
    ignoreNextNewLine = false;
    return;
  }

  //if we're in a paragraph, don't report the new-line either
  if (inParagraph)
    return;

  //set mode back to default mode
  mode = defaultmode;
  
  //neither NOBR nor P - report newline
  results->addToList (results->createText ("\r\n"));
}

//flags

//we treat flag as another tag - this is needed to allow correct flag closing even if the //appropriate closing tag wasn't sent by the MUD (auto-closing of flag)
void cMXPState::gotFlag (bool begin, string flag)
{
  bool setFlag = false;  //is this a set-variable flag?
  string f = lcase (flag);
  if ((f[0] == 's') && (f[1] == 'e') && (f[2] == 't') && (f[3] == ' '))
    setFlag = true;

  //disable inVar and remember old value, if this is a set-flag
  //this is needed to prevent error report in commonTagHandler()
  bool oldInVar = inVar;
  if (setFlag) inVar = false;

  commonTagHandler();
  
  //restore inVar value
  inVar = oldInVar;
  
  //no -> inform about the flag
  if (begin)
  {
    mxpResult *res = results->createFlag (true, flag);
    mxpResult *res2 = createClosingResult (res);
    results->addToList (res);
    addClosingTag ("flag", res2);
    
    //"set xxx" type of flag?
    if (setFlag)
    {
      if (inVar)  //in variable already
      {
        results->addToList (results->createError
            ("Got a set-flag, but I'm already in a variable definition!"));
        return;
      }
      //we are now in a variable
      inVar = true;
      varName = f.substr (f.rfind (' ') + 1);  //last word
      varValue = "";
    }
  }
  else
  {
    //closing set-flag...
    if (inVar && setFlag)
    {
      results->addToList (results->createVariable (varName, varValue));
      //send variable value, but no varname as in </var>
      results->addToList (results->createText (varValue));
      entities->addEntity (varName, varValue);
      inVar = false;
      varName = "";
      varValue = "";
    }
    gotClosingTag ("flag");
  }
  
  //no commonAfterTagHandler() here - this ain't no real tag :D
}


//tags:

//variables

void cMXPState::gotVariable (const string &name, const string &value, bool erase)
{
  commonTagHandler();

  //send the variable value
  results->addToList (results->createVariable (name, value, erase));

  commonAfterTagHandler();
}

void cMXPState::gotVAR (const string &name)
{
  commonTagHandler();

  if (inVar)
  {
    results->addToList (results->createError ("Nested VAR tags are not allowed!"));
    commonAfterTagHandler();
    return;
  }

  //we are now in a variable
  inVar = true;
  varName = name;
  varValue = "";

  //create a closing result; the variable name shall be updated when the tag will be closed
  addClosingTag ("var");

  commonAfterTagHandler();
}


//text formatting (OPEN tags)

void cMXPState::gotBOLD ()
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_BOLD, Bold, cMXPColors::noColor(),
      cMXPColors::noColor(), "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("b", res2);

  commonAfterTagHandler();
}

void cMXPState::gotITALIC ()
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_ITALICS, Italic, cMXPColors::noColor(),
      cMXPColors::noColor(), "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("i", res2);

  commonAfterTagHandler();
}

void cMXPState::gotUNDERLINE ()
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_UNDERLINE, Underline, cMXPColors::noColor(), cMXPColors::noColor(), "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("u", res2);

  commonAfterTagHandler();
}

void cMXPState::gotSTRIKEOUT ()
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_STRIKEOUT, Strikeout, cMXPColors::noColor(),
      cMXPColors::noColor(), "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("s", res2);

  commonAfterTagHandler();
}

void cMXPState::gotCOLOR (RGB fg, RGB bg)
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_FG | USE_BG, 0, fg, bg, "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("c", res2);

  commonAfterTagHandler();
}

void cMXPState::gotHIGH ()
{
  commonTagHandler();

  RGB color = fgcolor;
  //High color is computed by adding 128 to each attribute...
  //This is a very primitive way of doing it, and it's probably insufficient. We'll see.
  color.r = (color.r < 128) ? (color.r + 128) : 255;
  color.g = (color.g < 128) ? (color.g + 128) : 255;
  color.b = (color.b < 128) ? (color.b + 128) : 255;

  mxpResult *res = results->createFormatting (USE_FG, 0, color, cMXPColors::noColor(), "", 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("h", res2);

  commonAfterTagHandler();
}

void cMXPState::gotFONT (const string &face, int size, RGB fg, RGB bg)
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_FG | USE_BG | USE_FONT | USE_SIZE, 0, fg, bg,
      face, size);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("font", res2);

  commonAfterTagHandler();
}

//line spacing

void cMXPState::gotNOBR ()
{
  commonTagHandler();

  //next new-line is to be ignored
  ignoreNextNewLine = true;

  //no reporting to client

  commonAfterTagHandler();
}

void cMXPState::gotP ()
{
  commonTagHandler();

  //we're now in a paragraph
  inParagraph = true;

  addClosingTag ("p");
  
  //no reporting to the client

  commonAfterTagHandler();
}

void cMXPState::gotBR ()
{
  commonTagHandler();

  //inform the client that we got a newline (but no mode changes shall occur)
  results->addToList (results->createText ("\r\n"));

  commonAfterTagHandler();
}

void cMXPState::gotSBR ()
{
  commonTagHandler();

  //soft-break is represented as 0x1F
  results->addToList (results->createText ("\x1f"));

  commonAfterTagHandler();
}

//links

void cMXPState::gotA (const string &href, const string &hint, const string &expire)
{
  commonTagHandler();

  inLink = true;
  isALink = true;
  linkText = "";
  mxpResult *res = results->createLink (expire, href, "", hint);

  addClosingTag ("a", res);

  commonAfterTagHandler();
}

string stripANSI (const string &s)
{
  // first of all, find out whether there are any ANSI sequences
  bool ansi = false;
  for (int i = 0; i < s.length(); ++i)
    if (s[i] == 27) ansi = true;
  if (!ansi) return s;

  // there are ANSI sequences - have to get rid of them
  string res;
  ansi = false;
  for (int i = 0; i < s.length(); ++i) {
    if (!ansi) {
      if (s[i] == 27)
        ansi = true;
      else
        res += s[i];
    } else {
      // ANSI seq is ended by a-z,A-Z
      if (isalpha(s[i]))
        ansi = false;
    }
  }
  return res;
}

void cMXPState::gotSEND (const string &command, const string &hint, bool prompt, const string &expire)
{
  commonTagHandler();

  inLink = true;
  isALink = false;
  linkText = "";
  gotmap = false;
  string cmd = stripANSI (command);
  lastcmd = cmd;
  mxpResult *res = results->createSendLink (expire, cmd, "", hint, prompt,
      (command.find ("|") == string::npos) ? false : true);

  addClosingTag ("send", res);

  commonAfterTagHandler();
}

void cMXPState::gotEXPIRE (const string &name)
{
  commonTagHandler();

  results->addToList (results->createExpire (name));

  commonAfterTagHandler();
}

//version control

void cMXPState::gotVERSION ()
{
  commonTagHandler();

  //this is to be sent...
  results->addToList (results->createSendThis ("\x1b[1z<VERSION MXP=" + mxpVersion + " CLIENT=" +
      clientName + " VERSION=" + clientVersion + ">\r\n"));

  commonAfterTagHandler();
}

void cMXPState::gotSUPPORT (list<string> params)
{
  commonTagHandler();

  if (!params.empty())  //some parameters - this is not supported at the moment
    results->addToList (results->createWarning (
        "Received <support> with parameters, but this isn't supported yet..."));
    
  string res;
  res = "\x1b[1z<SUPPORTS +!element +!attlist +!entity +var +b +i +u +s +c +h +font";
  res += " +nobr +p +br +sbr +version +support +h1 +h2 +h3 +h4 +h5 +h6 +hr +small +tt";
  if (suplink)
    res += " +a +send +expire";
  if (supgauge)
    res += " +gauge";
  if (supstatus)
    res += " +status";
  if (supsound)
    res += " +sound +music";
  if (supframe)
    res += " +frame +dest";
  if (supimage)
    res += " +image";
  if (suprelocate)
    res += " +relocate +user +password";
  res += ">\r\n";
  results->addToList (results->createSendThis (res));
  
  commonAfterTagHandler();
}

//optional tags go next

//other HTML tags

void cMXPState::gotHtag (int which)
{
  if ((which < 1) || (which > 6)) //BUG!!!
  {
    commonAfterTagHandler();
    return;
  }

  commonTagHandler();

  int idx = which - 1;
  mxpResult *res = results->createFormatting (USE_ALL, Hattribs[idx], Hfg[idx], Hbg[idx],
      Hfont[idx], Hsize[idx]);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  char ct[3];
  ct[0] = 'h';
  ct[1] = '1' + idx;
  ct[2] = '\0';
  addClosingTag (ct, res2);

  commonAfterTagHandler();
}

void cMXPState::gotHR ()
{
  commonTagHandler();

  results->addToList (results->createHorizLine ());

  commonAfterTagHandler();
}

void cMXPState::gotSMALL ()
{
  commonTagHandler();

  //SMALL means 3/4 of standard size :)
  mxpResult *res = results->createFormatting (USE_SIZE, 0, cMXPColors::noColor(),
    cMXPColors::noColor(), "", defaultsize * 3/4);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("small", res2);

  commonAfterTagHandler();
}

void cMXPState::gotTT ()
{
  commonTagHandler();

  mxpResult *res = results->createFormatting (USE_FONT, 0, cMXPColors::noColor(), cMXPColors::noColor(), ttFont, 0);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);
  addClosingTag ("tt", res2);

  commonAfterTagHandler();
}

//MSP compatibility

void cMXPState::gotSOUND (const string &fname, int vol, int count, int priority,
    const string &type, const string &url)
{
  commonTagHandler();

  results->addToList (results->createSound (true, fname, vol, count, priority, false, type, url));

  commonAfterTagHandler();
}

void cMXPState::gotMUSIC (const string &fname, int vol, int count, bool contifrereq,
    const string &type, const string &url)
{
  commonTagHandler();

  results->addToList (results->createSound (false, fname, vol, count, 0, contifrereq, type, url));

  commonAfterTagHandler();
}

//gauges / status bars

void cMXPState::gotGAUGE (const string &entity, const string &maxentity, const string &caption,
    RGB color)
{
  commonTagHandler();

  results->addToList (results->createGauge (entity, maxentity, caption, color));

  commonAfterTagHandler();
}

void cMXPState::gotSTAT (const string &entity, const string &maxentity, const string &caption)
{
  commonTagHandler();

  results->addToList (results->createStat (entity, maxentity, caption));

  commonAfterTagHandler();
}

//frames and cursor control

void cMXPState::gotFRAME (const string &name, const string &action, const string &title,
    bool internal, const string &align, int left, int top, int width, int height,
    bool scrolling, bool floating)
{
  commonTagHandler();

  if (name.empty())
  {
    results->addToList (results->createError ("Got FRAME tag without frame name!"));
    commonAfterTagHandler();
    return;
  }

  string nm = lcase (name);
  string act = lcase (action);
  string alg = lcase (align);

  string tt = title;
  //name is the default title
  if (tt.empty())
    tt = name;
   
  //align
  alignType at = Top;
  if (!align.empty())
  {
    bool alignok = false;
    if (align == "left") { at = Left; alignok = true; }
    if (align == "right") { at = Right; alignok = true; }
    if (align == "top") { at = Top; alignok = true; }
    if (align == "bottom") { at = Bottom; alignok = true; }
    if (!alignok)
      results->addToList (results->createError ("Received FRAME tag with unknown ALIGN option!"));
  }

  //does the list of frames contain frame with name nm?
  bool nmExists = (frames.count (nm) != 0);
  
  if (act == "open")
  {
    if (nmExists)
    {
      results->addToList (results->createError ("Received request to create an existing frame!"));
      commonAfterTagHandler();
      return;
    }
    //cannot create _top or _previous
    if ((nm == "_top") || (nm == "_previous"))
    {
      results->addToList (results->createError ("Received request to create a frame with name " +
          nm + ", which is invalid!"));
      commonAfterTagHandler();
      return;
    }
    if (internal)
    {
      //false for internal windows... value not used as of now, but it may be used later...
      frames[nm] = false;
      results->addToList (results->createInternalWindow (nm, tt, at, scrolling));
    }
    else
    {
      //true for normal windows... value not used as of now, but it may be used later...
      frames[nm] = true;
      results->addToList (results->createWindow (nm, tt, left, top, width, height,
          scrolling, floating));
    }
  }
  if (act == "close")
  {
    if (nmExists)
    {
      frames.erase (nm);
      results->addToList (results->createCloseWindow (nm));
    }
    else
      results->addToList (results->createError
          ("Received request to close a non-existing frame!"));
  }
  if (act == "redirect")
  {
    //if the frame exists, or if the name is either _top or _previous, we redirect to that window
    if ((nm == "_top") || (nm == "_previous") || nmExists)
      redirectTo (nm);

    else
    {
      //create that window
      if (internal)
      {
        //false for internal windows... value not used as of now, but it may be used later...
        frames[nm] = false;
        results->addToList (results->createInternalWindow (nm, tt, at, scrolling));
      }
      else
      {
        //true for normal windows... value not used as of now, but it may be used later...
        frames[nm] = true;
        results->addToList (results->createWindow (nm, tt, left, top, width, height,
            scrolling, floating));
      }
      //then redirect to it
      redirectTo (nm);
    }
  }

  commonAfterTagHandler();
}

void cMXPState::redirectTo (const string &name)
{
  string nm = lcase (name);
  
  string emptystring;
  mxpResult *res = 0;
  if (nm == "_top")
    res = results->createSetWindow (emptystring);
  else
  if (nm == "_previous")
    res = results->createSetWindow (prevWindow);
  else
    if (frames.count (nm))
      res = results->createSetWindow (nm);
    else
      res = results->createError ("Received request to redirect to non-existing window " + nm);
  //apply result - will update info about previous window and so...
  applyResult (res);
  results->addToList (res);
}

void cMXPState::gotDEST (const string &name, int x, int y, bool eol, bool eof)
{
  commonTagHandler();

  string nm = lcase (name);
  bool nmExists = (frames.count (nm) != 0);
  
  if (!nmExists)
  {
    results->addToList (results->createError ("Received a request to redirect to non-existing window " + nm));
    return;
  }
  
  mxpResult *res = results->createSetWindow (name);
  mxpResult *res2 = createClosingResult (res);
  applyResult (res);
  results->addToList (res);

  int _x = x;
  int _y = y;
  if ((y >= 0) && (x < 0)) _x = 0;
  if ((_x >= 0) && (_y >= 0))
    results->addToList (results->createMoveCursor (_x, _y));

  list<mxpResult *> *ls = 0;
  //erase AFTER displaying text
  if (eol || eof)
  {
    ls = new list<mxpResult *>;
    ls->push_back (res2);
    res2 = results->createEraseText (eof);
  }

  //closing tag...
  addClosingTag ("dest", res2, ls);

  commonAfterTagHandler();
}

//crosslinking servers

void cMXPState::gotRELOCATE (const string &hostname, int port)
{
  commonTagHandler();

  results->addToList (results->createRelocate (hostname, port));

  commonAfterTagHandler();
}

void cMXPState::gotUSER ()
{
  commonTagHandler();

  results->addToList (results->createSendLogin (true));

  commonAfterTagHandler();
}

void cMXPState::gotPASSWORD ()
{
  commonTagHandler();

  results->addToList (results->createSendLogin (false));

  commonAfterTagHandler();
}

//images

void cMXPState::gotIMAGE (const string &fname, const string &url, const string &type, int height,
    int width, int hspace, int vspace, const string &align, bool ismap)
{
  commonTagHandler();

  //align
  string alg = lcase (align);
  alignType at = Top;
  if (!align.empty())
  {
    bool alignok = false;
    if (align == "left") { at = Left; alignok = true; }
    if (align == "right") { at = Right; alignok = true; }
    if (align == "top") { at = Top; alignok = true; }
    if (align == "bottom") { at = Bottom; alignok = true; }
    if (align == "middle") { at = Middle; alignok = true; }
    if (!alignok)
      results->addToList (results->createError ("Received IMAGE tag with unknown ALIGN option!"));
  }

  if (gotmap)
    results->addToList (results->createError ("Received multiple image maps in one SEND tag!"));

  if (ismap)
  {
    if (inLink && (!isALink))
    {
      results->addToList (results->createImageMap (lastcmd));
      lastcmd = "";
      gotmap = true;
    }
    else
      results->addToList (results->createError ("Received an image map with no SEND tag!"));
  }
  results->addToList (results->createImage (fname, url, type, height, width, hspace, vspace, at));

  commonAfterTagHandler();
}


//closing tags

void cMXPState::gotClosingTag (const string &name)
{
  string nm = lcase (name);
  //hack, to prevent an error from being reported when </var> or end-of-flag comes
  //we cannot simply test for </var> and friends and disable it then, because
  //we could have the var tag inside some element
  bool oldInVar = inVar;
  inVar = false;
  
  commonTagHandler();
  
  //restore the inVar variable...
  inVar = oldInVar;

  bool okay = false;
  while (!okay)
  {
    if (closingTags.empty())
      break;  //last one closed...
    //closingTags is a FIFO queue, tho technically it's a list
    closingTag *tag = closingTags.back ();
    closingTags.pop_back ();

    if (tag->name == nm)
      okay = true;  //good
    else
      results->addToList (results->createWarning ("Had to auto-close tag " + tag->name +
          ", because closing tag </" + name + "> was received."));
    
    closeTag (tag);
  }

  if (!okay)
    results->addToList (results->createError ("Received unpaired closing tag </" + name + ">."));

  commonAfterTagHandler();
}

void cMXPState::closeTag (closingTag *tag)
{
  //some tags need special handling...
  if (tag->name == "p")
  {
    inParagraph = false;
    ignoreNextNewLine = false;
    //also send a newline after end of paragraph... MXP docs say nothing about this :(
    results->addToList (results->createText ("\r\n"));
  }
  if (tag->name == "var")
  {
    tag->closingresult = 0;
    tag->closingresults = 0;
    results->addToList (results->createVariable (varName, varValue));
    results->addToList (results->createText (varName + ": " + varValue));
    entities->addEntity (varName, varValue);
    inVar = false;
    varName = "";
    varValue = "";
  }
  if (tag->name == "a")
  {
    if (inLink && isALink)
    {
      // !!! SOME LOW-LEVEL MANIPULATIONS HERE !!!
      
      linkStruct *ls = (linkStruct *) tag->closingresult->data;
      //assign text, using URL if no text given
      string lt = linkText.empty() ? (ls->url ? ls->url : "") : linkText;
      lt = stripANSI (lt);
      ls->text = new char[lt.length() + 1];
      ls->text[0] = '\0';
      if (lt.length())
        strcpy (ls->text, lt.c_str());
    }
    else
      //this should never happen
      results->addToList (results->createError ("Received </A> tag, but I'm not in a link!"));
    linkText = "";
    inLink = false;
    isALink = false;
  }
  if (tag->name == "send")
  {
    if (gotmap)
    {
      //don't send this closing result
      results->deleteResult (tag->closingresult);
      tag->closingresult = 0;
      
      if (!linkText.empty())
        results->addToList (results->createError
            ("Received image map and a command in one SEND tag!"));
    }
    else if (inLink && (!isALink))
    {
      // !!! SOME LOW-LEVEL MANIPULATIONS HERE !!!
      
      sendStruct *ss = (sendStruct *) tag->closingresult->data;
      //assign text, also assign to command if none given
      
      //assign linkText to ss->text
      linkText = stripANSI (linkText);
      delete[] ss->text;
      ss->text = new char[linkText.length() + 1];
      strcpy (ss->text, linkText.c_str());
      
      if (ss->hint)
      {
        //expand &text; in hint
        string hint = ss->hint;

        bool found = true, havematch = false;
        while (found)
        {
          int p = hint.find ("&text;");
          if (p < hint.length())   //found it
          {
            //replace it...
            hint.replace (p, 6, linkText);
            havematch = true;
          }
          else
            found = false;  //no more matches
        }
        if (havematch)  //apply changes if needed
        {
          //assign hint to ss->hint
          delete[] ss->hint;
          ss->hint = new char[hint.length() + 1];
          strcpy (ss->hint, hint.c_str());
        }
      }
      if (ss->command)
      {
        string cmd = ss->command;
        //also expand &text; in href
        
        bool found = true, havematch = false;
        while (found)
        {
          int p = cmd.find ("&text;");
          if (p < cmd.length())   //found it
          {
            //replace it...
            cmd.replace (p, 6, linkText);
            havematch = true;
          }
          else
            found = false;  //no more matches
        }
        if (havematch)  //apply changes if needed
        {
          //assign cmd to ss->command
          delete[] ss->command;
          ss->command = new char[cmd.length() + 1];
          strcpy (ss->command, cmd.c_str());
        }
      }
      else if (!linkText.empty())
      {
        //assign linkText to ss->command
        ss->command = new char[linkText.length() + 1];
        strcpy (ss->command, linkText.c_str());
      }
    }
    else
      //this should never happen
      results->addToList (results->createError ("Received </SEND> tag, but I'm not in a link!"));

    linkText = "";
    inLink = false;
    isALink = false;
    gotmap = false;
  }

  //handle applying/sending of closing results, is any
  if (tag->closingresult)
  {
    //apply result, reverting changes made by opening tag
    applyResult (tag->closingresult);
    //and send the changes to the client app
    results->addToList (tag->closingresult);
  }
  if (tag->closingresults)
  {
    //the same for remaining closing tags...
    list<mxpResult *>::iterator it;
    for (it = tag->closingresults->begin(); it != tag->closingresults->end(); ++it)
    {
      applyResult (*it);
      results->addToList (*it);
    }
  }
  //finally, the closing tag gets deleted
  //note that this won't delete the results themselves - they will be deleted after
  //they are processed by the client app
  delete tag->closingresults;
  tag->closingresults = 0;
  delete tag;
}


//mxpResult handling

mxpResult *cMXPState::createClosingResult (mxpResult *what)
{
  mxpResult *res = 0;
  switch (what->type) {
    case 3: {
      flagStruct *fs = (flagStruct *) what->data;
      res = results->createFlag (false, fs->name);
      break;
    }
    case 5: {
      formatStruct *fs = (formatStruct *) what->data;
      //usemask is the most relevant thing here - things not enabled there won't be applied,
      //so we can place anything there
      int usemask = fs->usemask;
      char curattrib = (bold?1:0) * Bold + (italic?1:0) * Italic +
          (underline?1:0) * Underline + (strikeout?1:0) * Strikeout;
      string font;
      if (usemask & USE_FONT)
        font = curfont;
      res = results->createFormatting (usemask, curattrib, fgcolor, bgcolor, font, cursize);
      break;
    }
    case 15: {
      res = results->createSetWindow (curWindow);
      break;
    }
  };
  return res;
}

void cMXPState::applyResult (mxpResult *what)
{
  switch (what->type) {
    case 5: {
      formatStruct *fs = (formatStruct *) what->data;
      int usemask = fs->usemask;
      if (usemask & USE_BOLD)
        bold = fs->attributes & Bold;
      if (usemask & USE_ITALICS)
        italic = fs->attributes & Italic;
      if (usemask & USE_UNDERLINE)
        underline = fs->attributes & Underline;
      if (usemask & USE_STRIKEOUT)
        strikeout = fs->attributes & Strikeout;
      if (usemask & USE_FG)
        fgcolor = fs->fg;
      if (usemask & USE_BG)
        bgcolor = fs->bg;
      if (usemask & USE_FONT)
        curfont = fs->font;
      if (usemask & USE_SIZE)
        cursize = fs->size;
      break;
    }
    case 15: {
      prevWindow = curWindow;
      if (what->data)
        curWindow = (char *) what->data;
      else
        curWindow = "";
      break;
    };

  };
}

void cMXPState::addClosingTag (const string &name, mxpResult *res, list<mxpResult *> *res2)
{
  closingTag *ctag = new closingTag;
  ctag->name = name;
  ctag->closingresult = res;
  ctag->closingresults = res2;
  closingTags.push_back (ctag);
}

void cMXPState::setScreenProps (int sx, int sy, int wx, int wy, int fx, int fy)
{
  sX = sx;
  sY = sy;
  wX = wx;
  wY = wy;
  fX = fx;
  fY = fy;
}

int cMXPState::computeCoord (const string &coord, bool isX, bool inWindow)
{
  int retval = atoi (coord.c_str());
  int len = coord.length();
  char ch = coord[len - 1];
  if (ch == 'c') retval *= (isX ? fX : fY);
  if (ch == '%') retval = retval * (inWindow ? (isX ? wX : wY) : (isX ? sX : sY)) / 100;
  return retval;
}

