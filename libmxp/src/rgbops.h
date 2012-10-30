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

#ifndef RGBOPS_H
#define RGBOPS_H

//some struct-operators that weren't defined by default (probably due to RGB being defined
// in extern "C" ...

inline bool operator== (RGB a, RGB b)
{
  return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b)) ? true : false;
}

#endif
