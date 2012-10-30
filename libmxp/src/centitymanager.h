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

#ifndef CENTITYMANAGER_H
#define CENTITYMANAGER_H

#include <map>
#include <string>

using namespace std;
/**
This class manages entities and provides a method for expanding them in a string.

@author Tomas Mecir
*/

class cEntityManager {
public:
  cEntityManager (bool noStdEntities = false);
  ~cEntityManager ();

  /** add or update entity */
  void addEntity (const string &name, const string &value);
  /** delete entity */
  void deleteEntity (const string &name);
  string entity (const string &name);
  bool exists (const string &name) { return (entities.count (name) != 0); };
  /** expand entities in a string */
  string expandEntities (const string &s, bool finished = true);
  bool needMoreText ();
  void reset (bool noStdEntities = false);
protected:
  /** empty string, to speed up some things a little bit */
  string empty_string;
  /** partial entity */
  string partent;
  /** are we in an entity? */
  bool inEntity;
  map <string, string> entities;
};

#endif
