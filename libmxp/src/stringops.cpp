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

#include "stringops.h"

#include <ctype.h>

using namespace std;

std::string lcase (const std::string &str)
{
  string s;
  int len = str.length();
  for (int i = 0; i < len; i++)
    s.push_back (tolower (str[i]));
  return s;
}

std::string ucase (const std::string &str)
{
  string s;
  int len = str.length();
  for (int i = 0; i < len; i++)
    s.push_back (toupper (str[i]));
  return s;
}

std::string firstword (const std::string &str)
{
  int pos = str.find (" ");
  return str.substr (0, pos);
}
