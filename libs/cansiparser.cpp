/***************************************************************************
                          cansiparser.cpp  -  ANSI parser
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pa Jun 21 2002
    copyright            : (C) 2002-2007 by Tomas Mecir
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

#define CANSIPARSER_CPP

#include "cansiparser.h"

#include <ctype.h>

#include "coutput.h"
#include "cactionmanager.h"
#include "cglobalsettings.h"
//needed for attribute #define-s
#include "ctextchunk.h"

cANSIParser::cANSIParser (int sess) : cActionBase ("ansiparser", sess)
{
  useansi = true;

  //default color scheme
  mycolor[CL_BLACK] = Qt::black;
  mycolor[CL_RED] = Qt::darkRed;
  mycolor[CL_GREEN] = Qt::darkGreen;
  mycolor[CL_YELLOW] = Qt::darkYellow;
  mycolor[CL_BLUE] = Qt::darkBlue;
  mycolor[CL_MAGENTA] = Qt::darkMagenta;
  mycolor[CL_CYAN] = Qt::darkCyan;
  mycolor[CL_WHITE] = Qt::lightGray;

  mycolor[CL_BRIGHT | CL_BLACK] = Qt::darkGray;
  mycolor[CL_BRIGHT | CL_RED] = Qt::red;
  mycolor[CL_BRIGHT | CL_GREEN] = Qt::green;
  mycolor[CL_BRIGHT | CL_YELLOW] = Qt::yellow;
  mycolor[CL_BRIGHT | CL_BLUE] = Qt::blue;
  mycolor[CL_BRIGHT | CL_MAGENTA] = Qt::magenta;
  mycolor[CL_BRIGHT | CL_CYAN] = Qt::cyan;
  mycolor[CL_BRIGHT | CL_WHITE] = Qt::white;

  defcolor = Qt::lightGray;
  defbkcolor = Qt::black;
  curcolor = defcolor;
  curbkcolor = defbkcolor;
  
  flush ();
  
  addEventHandler ("connected", 100, PT_NOTHING);
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cANSIParser::~cANSIParser()
{
  removeEventHandler ("connected");
  removeGlobalEventHandler ("global-settings-changed");
}

void cANSIParser::eventNothingHandler (QString event, int)
{
  if (event == "connected")
    flush ();
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    for (int i = 0; i < 16; i++)
      setColor (gs->getColor("color-" + QString::number (i)), i);
    setDefaultTextColor (color (gs->getInt ("fg-color")));
    setDefaultBkColor (color (gs->getInt ("bg-color")));
  }
}

QColor cANSIParser::color (int index)
{
  if ((index >= 0) && (index <= 15))
    return mycolor[index];
  else
    return Qt::white;    //returns white for invalid parameter values
}

void cANSIParser::setColor (QColor color, int index)
{
  if ((index >= 0) && (index <= 15))
    mycolor[index] = color;
}

void cANSIParser::parseText (const QString &data)
{
  //buffer may contain unfinished ANSI sequences, so we add new input to it...
  buffer += data;
  //this will contain ansi commands
  QString ansicmd = "";
  //this will contain plain text
  QString text = "";
  bool inANSI = false;
  int len = buffer.length ();
  int processed = -1;   //variable processed has the same role as one in cTelnet
  for (int i = 0; i < len; i++)
  {
    if (inANSI)
    {
      char ch = buffer[i].toLatin1();
      ansicmd += ch;
      //ANSI commands end with a letter...
      if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')))
      {
        inANSI = false;
        processed = i;
        if (ch == 'm')
           //ANSI command that changed color; other commands are
           //not interesting (for us at least ;-))...
        {
          //the text that preceeded this command should be displayed in old
          //colors - make it happen!
          emit plainText (text);
          text = "";  //old text is no longer needed

          //the ANSI command is now in ansicmd...
          //we parse it, getting all integers and signaling them (each
          //integer in this cmd is a code for ANSI color)
          int color = 0;
          int innumber = false;
          int alen = ansicmd.length ();
          int codeCount = 0;

          for (int j = 0; j < alen; j++)
          {
            char nn = ansicmd[j].toLatin1();
            if (isdigit (nn)) //a digit is here
            {
              if (!innumber)
              {
                innumber = true;
                color = 0;
              }
              color = color * 10 + (nn - 48); //ASCII code for '0' is 48, ...
            }
            else  //something else is here
            {
              if (innumber)
              {
                //only emit the signal if we use ANSI colors
                if (useansi)
                  changeColor (color);
                color = 0;
                codeCount++;
                innumber = false;
              }
            }
          }
          if (codeCount == 0)
            // ESC[m should be treated as ESC[0m
            if (useansi)
              changeColor (0);
          
          //the text ends in a 'm', so that we don't have to worry about numbers
          //ending at the end of the string...
        }
        ansicmd = "";
      }
    }
    else
    {
      if (buffer[i].toLatin1() == 27)   //new ANSI command begins...
      {
        inANSI = true;
        //start new ANSI command
        ansicmd = buffer[i];
      }
      else
      {
        text += buffer[i];    //this is normal text, no ANSI...
        processed = i;
      }
    }
  }
  emit plainText (text);
  text = "";  //old text is no longer needed
  buffer.remove (0, processed + 1);  //remove processed data from the buffer
}

void cANSIParser::flush ()
{
  buffer = "";

  brightactive = false;
  blinkactive = false;
  italics = false;
  underline = false;
  strikeout = false;
  negactive = false;
  invisible = false;

  curcolor = defcolor;
  curbkcolor = defbkcolor;
  emit fgColor (curcolor);
  emit bgColor (curbkcolor);
}

QColor cANSIParser::defaultTextColor ()
{
  return defcolor;
}

void cANSIParser::setDefaultTextColor (QColor color)
{
  if (curcolor == defcolor)
  {
    curcolor = color;
    emit fgColor (curcolor);
  }
  defcolor = color;
}

QColor cANSIParser::defaultBkColor ()
{
  return defbkcolor;
}

void cANSIParser::setDefaultBkColor (QColor color)
{
  if (curbkcolor == defbkcolor)
  {
    curbkcolor = color;
    emit bgColor (curbkcolor);
  }
  defbkcolor = color;
  cOutput *output = dynamic_cast<cOutput *>(object ("output"));
  output->setDefaultBkColor (color);
}

void cANSIParser::activateBright ()
{
  for (int i = 0; i < 8; i++)
    if (curcolor == mycolor[i])
    {
      curcolor = mycolor[i + 8];
      break;
    }
}

void cANSIParser::deactivateBright ()
{
  for (int i = 8; i < 16; i++)
    if (curcolor == mycolor[i])
    {
      curcolor = mycolor[i - 8];
      break;
    }
}

void cANSIParser::changeColor (int color)
{
  //text color
  if ((color >= 30) && (color <= 37))
  {
    int c = color - 30;
    curcolor = mycolor[c];
    if (brightactive)
      activateBright ();
    emit fgColor (curcolor);
  }
  else
  //text background
  if ((color >= 40) && (color <= 47))
  {
    curbkcolor = mycolor[color - 40];
    emit bgColor (curbkcolor);
  }
  else
  {
    //color code definitions from Ecma-048, page 61
    switch (color) {
      case 0:  //default color
        curcolor = defcolor;
        curbkcolor = defbkcolor;
        blinkactive = false;
        brightactive = false;
        italics = false;
        underline = false;
        strikeout = false;
        invisible = false;
        negactive = false;
        emit fgColor (curcolor);
        emit bgColor (curbkcolor);
        break;
      case 1:   //BRIGHT
        brightactive = true;
        activateBright ();
        emit fgColor (curcolor);
        break;
      case 2:   //FAINT (DECREASED INTENSITY)
        //this is handled as BOLD OFF, I think it's sufficient
        brightactive = false;
        deactivateBright ();
        emit fgColor (curcolor);
        break;
      case 3:   //ITALICS
        italics = true;
        break;
      case 4:   //UNDERLINE
        underline = true;
        break;
      case 5:   //BLINK
        blinkactive = true;
        break;
      case 6:   //RAPID BLINK
        //will blink, but NOT rapidly - rapid blinking is pure evil
        //both 5 and 6 are cancelled by 25, so we don't have to manage two
        //separate flags for this
        blinkactive = true;
        break;
      case 7:   //NEGATIVE IMAGE
        negactive = true;
        break;
      case 8:   //INVISIBLE
        invisible = true;
        break;
      case 9:   //STRIKE OUT
        strikeout = true;
        break;
      //10-19 are font changes. These are not supported.
      case 20:  //FRAKTUR (GOTHIC)
        //gothic is treated as italics
        //both italics and gothic are turned off with 23 so I don't need to
        //maintain two separate flags
        italics = true;
        break;
      case 21:  //DOUBLE UNDERLINE
        //this is handled as single underline
        //one flag is enough, as both options are disabled with 24
        underline = true;
        break;
      case 22:  //BOLD OFF
        brightactive = false;
        deactivateBright ();
        emit fgColor (curcolor);
        break;
      case 23:  //ITALICS OFF
        italics = false;
        break;
      case 24:  //UNDERLINE OFF
        underline = false;
        break;
      case 25:  //BLINK OFF
        blinkactive = false;
      //26 is proportional spacing - NOT SUPPORTED
      case 27:  //POSITIVE IMAGE
        negactive = false;
        break;
      case 28:  //VISIBLE
        invisible = false;
        break;
      case 29:  //NOT STRIKED OUT
        strikeout = false;
        break;
      //30-37 are colors, see above
      //38 is some special color, not supported
      case 39:  //default color, keep background
        curcolor = defcolor;
        if (brightactive) activateBright ();
        emit fgColor (curcolor);
        break;
      //40-47 are colors, see above
      //48 is some special color, not supported
      case 49:  //default background, keep text color
        curbkcolor = defbkcolor;
        emit bgColor (curbkcolor);
        break;
      //50 means proportional spacing off - not supported
      //codes >50 are not supported (these include special effects like
      // framed, encircled, overlined and so...
    };
    
    if ((color <= 29) && (color != 1) && (color != 2) && (color != 22))
    {
      //attributes may have changed - inform others...
      int a = 0;
      if (italics)
        a += ATTRIB_ITALIC;
      if (underline)
        a += ATTRIB_UNDERLINE;
      if (strikeout)
        a += ATTRIB_STRIKEOUT;
      if (blinkactive)
        a += ATTRIB_BLINK;
      if (negactive)
        a += ATTRIB_NEGATIVE;
      if (invisible)
        a += ATTRIB_INVISIBLE;
      emit attrib (a);
    }
  }
}

#include "cansiparser.moc"

