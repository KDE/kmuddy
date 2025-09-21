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
#include "cmxpcolors.h"

#include "colorlist.h"
#include "rgbops.h"

#include <ctype.h>

cMXPColors *cMXPColors::_self = 0;
RGB cMXPColors::nocolor = {0, 0, 0};

cMXPColors::cMXPColors ()
{
  //fill in the colors mapping...
  for (int i = 0; i < NUM_MXP_COLORS; i++)
    addColor (COLOR_NAMES[i], COLOR_DEF[i]);
}

cMXPColors::~cMXPColors ()
{
  //clear the colors mapping
  colors.clear ();
  //instance doesn't exist any more
  _self = 0;
}

cMXPColors * cMXPColors::self ()
{
  //returns an instance, creating it if it doesn't exist
  if (!_self)
    _self = new cMXPColors;
  return _self;
}

void cMXPColors::addColor (const string &color, RGB def)
{
  //will overwrite existing color, if any
  colors[color] = def;
}

void cMXPColors::removeColor (const string &color)
{
  colors.erase (color);
}

RGB cMXPColors::color (const string &color)
{
  string clr = color;
  //conversion to lowercase
  for (int i = 0; i < clr.length(); i++)
    clr[i] = tolower (clr[i]);

  //color in style #rrggbb
  if ((clr.length() == 7) && (clr[0] == '#'))
  {
    //check if all digits are correct
    bool okay = true;
    for (int i = 1; i <= 6; i++)
      if (!isxdigit (clr[i]))
        okay = false;
    //okay - parse and return the color
    if (okay)
    {
      char r1 = tolower (clr[1]);
      char r2 = tolower (clr[2]);
      char g1 = tolower (clr[3]);
      char g2 = tolower (clr[4]);
      char b1 = tolower (clr[5]);
      char b2 = tolower (clr[6]);
      r1 = (r1 <= '9') ? (r1 - '0') : (10 + (r1 - 'a'));
      r2 = (r2 <= '9') ? (r2 - '0') : (10 + (r2 - 'a'));
      g1 = (g1 <= '9') ? (g1 - '0') : (10 + (g1 - 'a'));
      g2 = (g2 <= '9') ? (g2 - '0') : (10 + (g2 - 'a'));
      b1 = (b1 <= '9') ? (b1 - '0') : (10 + (b1 - 'a'));
      b2 = (b2 <= '9') ? (b2 - '0') : (10 + (b2 - 'a'));
      RGB col;
      col.r = r1 * 16 + r2;
      col.g = g1 * 16 + g2;
      col.b = b1 * 16 + b2;
      
      return col;
    }
  }
  //one of pre-defined colors
  if (colors.count (clr))
    return colors[clr];
  return nocolor;
}
