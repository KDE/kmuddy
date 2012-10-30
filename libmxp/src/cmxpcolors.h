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
#ifndef CMXPCOLORS_H
#define CMXPCOLORS_H

#include <map>
#include <string>

#include "libmxp.h"

using namespace std;

/**
This class contains a list of MXP colors. It is based on the singleton pattern (single-instance).

@author Tomas Mecir
*/

class cMXPColors {
public:
  /** destructor */
  ~cMXPColors ();
  /** return an instance of this class */
  static cMXPColors *self ();
  void addColor (const string &color, RGB def);
  void removeColor (const string &color);
  RGB color (const string &color);
  static RGB noColor () { return nocolor; };
protected:
  /** constructor */
  cMXPColors ();

  map<string, RGB> colors;
  static RGB nocolor;

  static cMXPColors *_self;
};

#endif
