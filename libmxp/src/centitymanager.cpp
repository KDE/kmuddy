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

#include "centitymanager.h"

#include "entitylist.h"

cEntityManager::cEntityManager (bool noStdEntities)
{
  reset (noStdEntities);
}

cEntityManager::~cEntityManager ()
{
  entities.clear ();
}

void cEntityManager::reset (bool noStdEntities)
{
  partent = "";
  entities.clear ();
  inEntity = false;
  
  if (noStdEntities)
    return;
  
  char s[2];
  s[1] = 0;
  //restore standard HTML entities
  for (int i = 0; i < NUM_MXP_ENTITIES; i++)
  {
    s[0] = ENTITY_DEF[i];
    entities[ENTITY_NAMES[i]] = s;
  }
}

//can this be the first letter of an entity?
inline bool correct1 (char l)
{
  return (((l >= 'a') && (l <= 'z')) || ((l >= 'A') && (l <= 'Z')) || (l == '#'));
}

//can this be a letter of entity?
inline bool correctN (char l)
{
  return (((l >= 'a') && (l <= 'z')) || ((l >= 'A') && (l <= 'Z')) || (l == '_') ||
      ((l >= '0') && (l <= '9')));
}

void cEntityManager::addEntity (const string &name, const string &value)
{
  if (name.empty()) return;

  //add or modify the entity
  entities[name] = value;
}

void cEntityManager::deleteEntity (const string &name)
{
  entities.erase (name);
}

string cEntityManager::entity (const string &name)
{
  if (exists (name))
    return entities[name];
  //return empty string otherwise
  return empty_string;
}

string cEntityManager::expandEntities (const string &s, bool finished)
{
  string s1;

  if (!partent.empty ())  //some unfinished entity is waiting...
    inEntity = true;

  string::const_iterator it;
  for (it = s.begin(); it != s.end(); ++it)
    if (inEntity)
    {
      char ch = *it;
      if (ch == ';')  //end of entity
      {
        inEntity = false;
        if (partent.empty()) //received &;
        {
          s1 += "&;";
        }
        else
        if (partent[0] == '_')  //invalid entity name - IGNORED
        {
          partent = "";
        }
        else
        if (partent[0] == '#')  //&#nnn; entity
        {
          //compute number
          int n = 0;
          string::iterator it2 = partent.begin();
          it2++;  //starting from second character
          for (; it2 != partent.end(); ++it2)
          {
            int x = *it2 - 48;
            if ((x < 0) || (x > 9)) //WRONG
            {
              n = 0;
              break;
            }
            n = n * 10 + x;
            if (n > 255)  //number too big!
            {
              n = 0;
              break;
            }
          }
          //verify number, IGNORE entity if it's wrong
          if ((n >= 32) && (n <= 255))
            s1 += (unsigned char) n;
          partent = "";
        }
        else
        {
          //now we have correct entity name, let's expand it, if possible :)
          if (entities.count (partent))
            s1 += entities[partent];
          else
            //keep the same string if the entity doesn't exist...
            s1 += "&" + partent + ";";
          partent = "";
        }
      }
      else if (ch == '&')
      //unterminated entity, new entity may start here
      {
        s1 += "&" + partent;
        partent = "";
        //isEntity remains set
      }
      else if ((partent.empty() && correct1(ch)) || ((!partent.empty()) && correctN(ch)))
      {
        partent += ch;
      }
      //this wasn't an entity after all
      else
      {
        inEntity = false;
        s1 += "&" + partent + ch;
        partent = "";
      }
    }
    else
    {
      if (*it == '&')
        inEntity = true;
      else
        //copy without change
        s1 += *it;
    }
  //string ends in an unterminated entity, but only if the string is finished
  if (inEntity && finished)
  {
    s1 += "&" + partent;
    partent = "";
    inEntity = false;
  }

  //return the resulting string
  return s1;
}

bool cEntityManager::needMoreText()
{
  return partent.empty() ? false : true;
}
