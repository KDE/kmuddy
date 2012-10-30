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

#include "celementmanager.h"

#include "centitymanager.h"
#include "cmxpcolors.h"
#include "cmxpparser.h"
#include "cmxpstate.h"
#include "cresulthandler.h"
#include "stringops.h"

#include <stdlib.h>

cElementManager::cElementManager (cMXPState *st, cResultHandler *res, cEntityManager *enm)
{
  state = st;
  results = res;
  entities = enm;

  paramexpander = new cEntityManager (true);
  parser = new cMXPParser;
  
  reset ();

  createInternalElements ();
}

cElementManager::~cElementManager ()
{
  delete paramexpander;
  paramexpander = 0;
  delete parser;
  parser = 0;
  
  removeAll ();

  //internal elements
  map<string, sInternalElement *>::iterator it;
  for (it = ielements.begin(); it != ielements.end(); ++it)
  {
    it->second->attlist.clear ();
    it->second->attdefault.clear ();
    delete it->second;
  }
  ielements.clear ();
  aliases.clear ();
}

void cElementManager::reset ()
{
  lastLineTag = 0;
  removeAll ();
}
  
void cElementManager::assignMXPState (cMXPState *st)
{
  state = st;
}

void cElementManager::createInternalElements ()
{
  //the list doesn't contain information on whether an argument is required or not
  //processor of the tag implements this functionality

  //create all internal elements
  sInternalElement *e;

  //!element
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("definition");  //this name is not in the spec!
  e->attlist.push_back ("att");
  e->attlist.push_back ("tag");
  e->attlist.push_back ("flag");
  e->attlist.push_back ("open");
  e->attlist.push_back ("delete");
  e->attlist.push_back ("empty");
  e->attdefault["open"] = "";  //flags
  e->attdefault["delete"] = "";
  e->attdefault["empty"] = "";
  ielements["!element"] = e;

  //!attlist
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("att");
  ielements["!attlist"] = e;

  //!entity
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("value");  //this name is not in the spec!
  e->attlist.push_back ("desc");
  e->attlist.push_back ("private");
  e->attlist.push_back ("publish");
  e->attlist.push_back ("add");
  e->attlist.push_back ("delete");
  e->attlist.push_back ("remove");
  e->attdefault["private"] = "";  //flags
  e->attdefault["publish"] = "";
  e->attdefault["delete"] = "";
  e->attdefault["add"] = "";
  e->attdefault["remove"] = "";
  ielements["!entity"] = e;

  //var
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("desc");
  e->attlist.push_back ("private");
  e->attlist.push_back ("publish");
  e->attlist.push_back ("add");
  e->attlist.push_back ("delete");
  e->attlist.push_back ("remove");
  e->attdefault["private"] = "";  //flags
  e->attdefault["publish"] = "";
  e->attdefault["delete"] = "";
  e->attdefault["add"] = "";
  e->attdefault["remove"] = "";
  ielements["var"] = e;

  //b
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  ielements["b"] = e;

  //i
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  ielements["i"] = e;

  //u
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  ielements["u"] = e;

  //s
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  ielements["s"] = e;

  //c
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  e->attlist.push_back ("fore");
  e->attlist.push_back ("back");
  ielements["c"] = e;

  //h
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  ielements["h"] = e;

  //font
  e = new sInternalElement;
  e->empty = false;
  e->open = true;
  e->attlist.push_back ("face");
  e->attlist.push_back ("size");
  e->attlist.push_back ("color");
  e->attlist.push_back ("back");
  ielements["font"] = e;

  //nobr
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["nobr"] = e;

  //p
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["p"] = e;

  //br
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["br"] = e;

  //sbr
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["sbr"] = e;

  //a
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  e->attlist.push_back ("href");
  e->attlist.push_back ("hint");
  e->attlist.push_back ("expire");
  ielements["a"] = e;

  //send
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  e->attlist.push_back ("href");
  e->attlist.push_back ("hint");
  e->attlist.push_back ("prompt");
  e->attlist.push_back ("expire");
  e->attdefault["prompt"] = "";  //flags
  ielements["send"] = e;

  //expire
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  ielements["expire"] = e;

  //version
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["version"] = e;

  //support
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["support"] = e;

  //h1
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h1"] = e;

  //h2
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h2"] = e;

  //h3
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h3"] = e;

  //h4
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h4"] = e;

  //h5
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h5"] = e;

  //h6
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["h6"] = e;

  //hr
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["hr"] = e;

  //small
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["small"] = e;

  //tt
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  ielements["tt"] = e;

  //sound
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("fname");
  e->attlist.push_back ("v");
  e->attlist.push_back ("l");
  e->attlist.push_back ("p");
  e->attlist.push_back ("t");
  e->attlist.push_back ("u");
  e->attdefault["v"] = "100";
  e->attdefault["l"] = "1";
  e->attdefault["p"] = "50";
  ielements["sound"] = e;

  //music
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("fname");
  e->attlist.push_back ("v");
  e->attlist.push_back ("l");
  e->attlist.push_back ("c");
  e->attlist.push_back ("t");
  e->attlist.push_back ("u");
  e->attdefault["v"] = "100";
  e->attdefault["l"] = "1";
  e->attdefault["c"] = "1";
  ielements["music"] = e;

  //gauge
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("entity");  //this name is not in the spec!
  e->attlist.push_back ("max");
  e->attlist.push_back ("caption");
  e->attlist.push_back ("color");
  ielements["gauge"] = e;

  //stat
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("entity");  //this name is not in the spec!
  e->attlist.push_back ("max");
  e->attlist.push_back ("caption");
  ielements["stat"] = e;

  //frame
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");
  e->attlist.push_back ("action");
  e->attlist.push_back ("title");
  e->attlist.push_back ("internal");
  e->attlist.push_back ("align");
  e->attlist.push_back ("left");
  e->attlist.push_back ("top");
  e->attlist.push_back ("width");
  e->attlist.push_back ("height");
  e->attlist.push_back ("scrolling");
  e->attlist.push_back ("floating");
  e->attdefault["action"] = "open";
  e->attdefault["align"] = "top";
  e->attdefault["left"] = "0";
  e->attdefault["top"] = "0";
  e->attdefault["internal"] = "";  //flags
  e->attdefault["scrolling"] = "";
  e->attdefault["floating"] = "";
  ielements["frame"] = e;

  //dest
  e = new sInternalElement;
  e->empty = false;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("x");
  e->attlist.push_back ("y");
  e->attlist.push_back ("eol");
  e->attlist.push_back ("eof");
  e->attdefault["eol"] = "";  //flags
  e->attdefault["eof"] = "";
  ielements["dest"] = e;

  //relocate
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("name");  //this name is not in the spec!
  e->attlist.push_back ("port");  //this name is not in the spec!
  ielements["relocate"] = e;

  //user
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["user"] = e;

  //password
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  ielements["password"] = e;

  //image
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("fname");
  e->attlist.push_back ("url");
  e->attlist.push_back ("t");
  e->attlist.push_back ("h");
  e->attlist.push_back ("w");
  e->attlist.push_back ("hspace");
  e->attlist.push_back ("vspace");
  e->attlist.push_back ("align");
  e->attlist.push_back ("ismap");
  e->attdefault["align"] = "top";
  e->attdefault["ismap"] = "";  //flags
  ielements["image"] = e;

  //filter
  e = new sInternalElement;
  e->empty = true;
  e->open = false;
  e->attlist.push_back ("src");
  e->attlist.push_back ("dest");
  e->attlist.push_back ("name");
  ielements["filter"] = e;

  //finally, define some aliases for internal elements
  aliases["!el"] = "!element";
  aliases["!at"] = "!attlist";
  aliases["!en"] = "!entity";
  aliases["v"] = "var";
  aliases["bold"] = "b";
  aliases["strong"] = "b";
  aliases["italic"] = "i";
  aliases["em"] = "i";
  aliases["underline"] = "u";
  aliases["strikeout"] = "s";
  aliases["high"] = "h";
  aliases["color"] = "c";
  aliases["destination"] = "dest";
}

bool cElementManager::elementDefined (const string &name)
{
  return ((elements.count (name) != 0) || (ielements.count (name) != 0) ||
      (aliases.count (name) != 0));
}

bool cElementManager::internalElement (const string &name)
{
  return ((ielements.count (name) != 0) || (aliases.count (name) != 0));
}

bool cElementManager::customElement (const string &name)
{
  return (elements.count (name) != 0);
}

bool cElementManager::openElement (const string &name)
{
  if (!elementDefined (name))
    return false;
  if (internalElement (name))
  {
    string n = name;
    if (aliases.count (name))
      n = aliases[name];
    return ielements[n]->open;
  }
  else
    return elements[name]->open;
}

bool cElementManager::emptyElement (const string &name)
{
  if (!elementDefined (name))
    return false;
  if (internalElement (name))
  {
    string n = name;
    if (aliases.count (name))
      n = aliases[name];
    return ielements[n]->empty;
  }
  else
    return elements[name]->empty;
}

enum tagParserState {
  tagBegin,
  tagName,
  tagParam,
  tagParamValue,
  tagQuotedParam,
  tagAfterQuotedParam,
  tagBetweenParams
};

void cElementManager::gotTag (const string &tag)
{
  string tagname;
  list<sParam> params;
  sParam param;
  param.flag = false;
  char quote;
  tagParserState pstate = tagBegin;
  string::const_iterator it;
  for (it = tag.begin(); it != tag.end(); ++it)
  {
    char ch = *it;

    //process character
    switch (pstate) {
      case tagBegin: {
        if (ch != ' ')
        {
          pstate = tagName;
          tagname += ch;
        }
        break;
      }
      case tagName: {
        if (ch == ' ')
          pstate = tagBetweenParams;
        else
          tagname += ch;
        break;
      }
      case tagParam: {
        if (ch == '=')
          pstate = tagParamValue;
        else if (ch == ' ')
        {
          //one parameter, value only (it could also be a flag, we'll check that later)
          param.value = param.name;
          param.name = "";
          //add a new parameter :-)
          params.push_back (param);
          param.value = "";
          pstate = tagBetweenParams;
        }
        else
          param.name += ch;
        break;
      }
      case tagParamValue: {
        if (ch == ' ')
        {
          //add a new parameter :-)
          params.push_back (param);
          param.name = "";
          param.value = "";
          pstate = tagBetweenParams;
        }
        else if (param.value.empty() && ((ch == '\'') || (ch == '"')))
        {
          pstate = tagQuotedParam;
          quote = ch;
        }
        else
          param.value += ch;
        break;
      }
      case tagQuotedParam: {
        if (ch == quote)
        {
          //add a new parameter :-)
          params.push_back (param);
          param.name = "";
          param.value = "";
          pstate = tagAfterQuotedParam;
        }
        else
          param.value += ch;
        break;
      }
      case tagAfterQuotedParam: {
        if (ch == ' ')    //ignore everything up to some space...
          pstate = tagBetweenParams;
        break;
      }
      case tagBetweenParams: {
        if (ch != ' ')
        {
          if ((ch == '\'') || (ch == '"'))
          {
            pstate = tagQuotedParam;
            param.name = "";
            quote = ch;
          }
          else
          {
            pstate = tagParam;
            param.name += ch;
          }
        }
        break;
      }
    };
  }

  //last parameter...
  switch (pstate) {
    case tagBegin:
      results->addToList (results->createError ("Received a tag with no body!"));
      break;
    case tagParam: {
      param.value = param.name;
      param.name = "";
      params.push_back (param);
      }
      break;
    case tagParamValue:
      params.push_back (param);
      break;
    case tagQuotedParam:
      results->addToList (results->createError ("Received tag " + tagname +
          " with unfinished quoted parameter!"));
      break;
  };

  //nothing more to do if the tag has no contents...
  if (pstate == tagBegin) return;
  
  //convert tag name to lowercase
  tagname = lcase (tagname);
  
  //handle closing tag...
  if (tagname[0] == '/')
  {
    if (!params.empty())
      results->addToList (results->createError ("Received closing tag " + tagname +
          " with parametrs!"));
    //remove that '/'
    tagname.erase (tagname.begin());
    //and call closing tag processing
    handleClosingTag (tagname);
    return;
  }
  
  //convert all parameter names to lower-case
  list<sParam>::iterator parit;
  for (parit = params.begin(); parit != params.end(); ++parit)
    (*parit).name = lcase ((*parit).name);
  
  //now we check the type of the tag and act accordingly
  if (!elementDefined (tagname))
  {
    params.clear();
    results->addToList (results->createError ("Received undefined tag " + tagname + "!"));
    return;
  }

  mxpMode m = state->getMXPMode ();
  //mode can be open or secure; locked mode is not possible here (or we're in a bug)
  if (m == openMode)
    //open mode - only open tags allowed
    if (!openElement (tagname))
    {
    params.clear();
      results->addToList (results->createError ("Received secure tag " + tagname +
          " in open mode!"));
      return;
    }

  if (internalElement (tagname))
  {
    //if the name is an alias for another tag, change the name
    if (aliases.count (tagname))
      tagname = aliases[tagname];
    //the <support> tag has to be handled separately :(
    if (tagname == "support")
    {
      processSupport (params);
      return;
    }
    //identify all flags in the tag
    identifyFlags (ielements[tagname]->attdefault, params);
    //correctly identify all parameters (assign names where necessary)
    handleParams (tagname, params, ielements[tagname]->attlist, ielements[tagname]->attdefault);
    //separate out all the flags (flags are only valid for internal tags)
    list<string> flags;
    parit = params.begin();
    while (parit != params.end())
    {
      if ((*parit).flag)
      {
        flags.push_back ((*parit).name);
        parit = params.erase (parit);
      }
      else
        ++parit;
    }
    //okay, parsing done - send the tag for further processing
    processInternalTag (tagname, params, flags);
  }
  else
  {
    handleParams (tagname, params, elements[tagname]->attlist, elements[tagname]->attdefault);
    processCustomTag (tagname, params);
  }
  
  params.clear ();
}

void cElementManager::identifyFlags (const map<string, string> &attdefault,
    list<sParam> &args)
{
  list<sParam>::iterator it;
  for (it = args.begin(); it != args.end(); ++it)
    if ((*it).name.empty())
    {
      string s = lcase ((*it).value);
      if ((attdefault.count (s) != 0) && (attdefault.find(s)->second == ""))
      {
        //this one is a flag
        (*it).name = s;
        (*it).value = "";
        (*it).flag = true;
      }
    }
}

void cElementManager::handleParams (const string &tagname, list<sParam> &args,
    const list<string> &attlist, const map<string, string> &attdefault)
{
  list<string>::const_iterator cur = attlist.begin();
  list<sParam>::iterator it;
  for (it = args.begin(); it != args.end(); ++it)
  {
    //flag?
    if ((*it).flag)
    {
      //only advance parameter iterator
      cur++;
    }
    //not a flag
    else
    {
      //empty name?
      if ((*it).name.empty())
      {
        //set the parameter name:
        
        //find the correct attribute name, skipping all flags
        while (cur != attlist.end())
        {
          if ((attdefault.count (*cur) != 0) && (attdefault.find(*cur)->second == ""))  //flag
            cur++;
          else
            break;  //okay, found the correct parameter
        }
        if (cur == attlist.end())    //ARGH! Parameter not found :(
        {
          results->addToList (results->createError ("Received too many parameters for tag " +
              tagname + "!"));
          continue;  //continue with the next parameter...
        }
      }
      //non-empty name?
      else
      {
        //set "cur" to the parameter following the given one
        
        //to speed things up a bit, first look if the iterator is pointing at the right parameter
        // (we won't need to traverse the whole list, if it does)
        if ((cur == attlist.end()) || ((*it).name != *cur))
        {
          list<string>::const_iterator cur2 = cur;  //remember old "cur" value
          for (cur = attlist.begin(); cur != attlist.end(); ++cur)
            if ((*it).name == *cur)
              break;
          if (cur == attlist.end())    //parameter name not found
          {
            //restore old iterator value
            cur = cur2;
            results->addToList (results->createError ("Received unknown parameter " +
                (*it).name + " in tag " + tagname + "!"));
            //clear name/value to avoid confusion in later stages
            (*it).name = "";
            (*it).value = "";
            //proceed with next parameter
            continue;
          }
          //if cur isn't attlist.end(), it's now set to the correct value...
        }
      }
      
      //things common to all non-flag parameters...
      
      //set parameter name
      (*it).name = *cur;
      //if parameter value is empty, set it to default value (if any)
      if ((*it).value.empty() && (attdefault.count (*cur) != 0))
        (*it).value = attdefault.find(*cur)->second;
      //advance parameter iterator
      cur++;
    }
  }
  
  //finally, we add default parameter values to the beginning of the list... these shall get
  //overridden by given values, if any (those shall be later in the list)
  sParam s;
  map<string, string>::const_iterator mit;
  for (mit = attdefault.begin(); mit != attdefault.end(); ++mit)
    if (mit->second != "")  //not a flag
    {
      s.flag = false;
      s.name = mit->first;
      s.value = mit->second;
      args.push_front (s);
    }
}

void cElementManager::gotLineTag (int number)
{
  if ((number < 20) || (number > 99))
  {
    lastLineTag = 0;
    return;
  }
  if (lineTags.count (number) == 0)
  {
    lastLineTag = 0;
    return;
  }
  string tag = lineTags[number];
  lastLineTag = number;
  //behave as if we've just gotten the appropriate tag
  gotTag (tag);
}

void cElementManager::gotNewLine ()
{
  if ((lastLineTag < 20) || (lastLineTag > 99))
  {
    lastLineTag = 0;
    return;
  }
  if (lineTags.count (lastLineTag) == 0)
  {
    lastLineTag = 0;
    return;
  }
  string tag = lineTags[lastLineTag];
  lastLineTag = 0;
  if (emptyElement (tag))
    //no closing tag needed
    return;
  //okay, send out the appropriate closing tag
  handleClosingTag (tag);
}

void cElementManager::handleClosingTag (const string &name)
{
  string n = lcase (name);
  if (!elementDefined (n))
  {
    results->addToList (results->createError ("Received unknown closing tag </" + n + ">!"));
    return;
  }
  if (emptyElement (n))
  {
    results->addToList (results->createError ("Received closing tag for tag " + n +
        ", which doesn't need a closing tag!"));
    return;
  }
  if (internalElement (n))
  {
    //if the name is an alias for another tag, change the name
    if (aliases.count (n))
      n = aliases[n];
    state->gotClosingTag (n);
  }
  else
  {
    //send closing flag, if needed
    if (!elements[n]->flag.empty())
      state->gotFlag (false, elements[n]->flag);
    
    //expand the closing tag...
    list<string>::iterator cit;
    for (cit = elements[n]->closingseq.begin(); cit != elements[n]->closingseq.end(); ++cit)
      handleClosingTag (*cit);
  }
}

void cElementManager::processInternalTag (const string &name, const list<sParam> &params,
    const list<string> &flags)
{
  list<sParam>::const_iterator it;
  list<string>::const_iterator it2;
  if (name == "!element")
  {
    string name, definition, att, flag;
    int tag;
    bool fopen = false, fdelete = false, fempty = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = lcase ((*it).value);
      if (s == "definition") definition = (*it).value;
      if (s == "att") att = (*it).value;
      if (s == "flag") flag = (*it).value;
      if (s == "tag") tag = atoi ((*it).value.c_str());
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
    {  
      if (*it2 == "open") fopen = true;
      if (*it2 == "delete") fdelete = true;
      if (*it2 == "empty") fempty = true;
    }
    
    if (name.empty())
    {
      results->addToList (results->createError (
          "Received an <!element> tag with no element name!"));
      return;
    }
    //definition can be empty, that's no problem...

    //if we want to delete the tag...
    if (fdelete)
    {
      //sanity check
      if (!elements.count (name))
      {
        results->addToList (results->createWarning (
            "Received request to remove an undefined tag " + name + "!"));
        return;
      }
      removeElement (name);
      return;
    }
    
    //parse tag definition
    parser->simpleParse (definition);
    list<sElementPart *> tagcontents;
    while (parser->hasNext ())
    {
      chunk ch = parser->getNext ();
      if (ch.chk == chunkError)
        results->addToList (results->createError (ch.text));
      else
      {
        //create a new element part
        sElementPart *part = new sElementPart;
        part->text = ch.text;
        part->istag = (ch.chk == chunkTag) ? true : false;
        tagcontents.push_back (part);
      }
    }
    
    //parse attribute list
    list<string> attlist;
    map<string, string> attdefault;
    processParamList (att, attlist, attdefault);
    
    //and do the real work
    addElement (name, tagcontents, attlist, attdefault, fopen, fempty, tag, flag);
  }
  else if (name == "!attlist")
  {
    string name, att;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "att") att = (*it).value;
    }
    
    if (name.empty())
    {
      results->addToList (results->createError (
          "Received an <!attlist> tag with no element name!"));
      return;
    }

    //parse attribute list
    list<string> attlist;
    map<string, string> attdefault;
    processParamList (att, attlist, attdefault);
    
    //and do the real work
    setAttList (name, attlist, attdefault);
  }
  else if (name == "!entity")
  {
    string name, value, desc;
    bool fpriv = false, fpub = false, fadd = false, fdel = false, frem = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "value") value = (*it).value;
      if (s == "desc") desc = (*it).value;
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
    {  
      if (*it2 == "private") fpriv = true;
      if (*it2 == "publish") fpub = true;
      if (*it2 == "delete") fdel = true;
      if (*it2 == "add") fadd = true;
      if (*it2 == "remove") frem = true;
    }
    
    if (name.empty())
    {
      results->addToList (results->createError (
          "Received an <!entity> tag with no variable name!"));
      return;
    }
    
    //fpub is IGNORED...
    //fadd and frem is IGNORED...
    if (!(fadd) && !(frem))
    {
      if (fdel)
      {
        entities->deleteEntity (name);
        if (!fpriv) //do not announce PRIVATE entities
          state->gotVariable (name, "", true);
      }
      else
      {
        //we now have a new variable...
        entities->addEntity (name, value);
        if (!fpriv) //do not announce PRIVATE entities
          state->gotVariable (name, value);
      }
    }
    else
      results->addToList (results->createWarning (
          "Ignored <!ENTITY> tag with ADD or REMOVE flag."));
  }
  else if (name == "var")
  {
    //this is very similar to the !entity handler above...
    
    string name, desc;
    bool fpriv = false, fpub = false, fadd = false, fdel = false, frem = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "desc") desc = (*it).value;
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
    {  
      if (*it2 == "private") fpriv = true;
      if (*it2 == "publish") fpub = true;
      if (*it2 == "add") fadd = true;
      if (*it2 == "delete") fdel = true;
      if (*it2 == "remove") frem = true;
    }
    
    if (name.empty())
    {
      results->addToList (results->createError (
          "Received an <var> tag with no variable name!"));
      return;
    }
    
    //fpriv and fpub is IGNORED...
    //fadd and fdel is IGNORED...
    if (!(fadd) && !(fdel))
      state->gotVAR (name);
    else
      results->addToList (results->createWarning ("Ignored <VAR> tag with ADD or REMOVE flag."));
  }
  else if (name == "b")
    state->gotBOLD();
  else if (name == "i")
    state->gotITALIC();
  else if (name == "u")
    state->gotUNDERLINE();
  else if (name == "s")
    state->gotSTRIKEOUT();
  else if (name == "c")
  {
    string fore, back;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "fore") fore = (*it).value;
      if (s == "back") back = (*it).value;
    }
    RGB fg = state->fgColor();
    RGB bg = state->bgColor();
    if (!fore.empty())
      fg = cMXPColors::self()->color (fore);
    if (!back.empty())
      bg = cMXPColors::self()->color (back);
    state->gotCOLOR (fg, bg);
  }
  else if (name == "h")
    state->gotHIGH();
  else if (name == "font")
  {
    string face, fore, back;
    int size = 0;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "face") face = (*it).value;
      if (s == "size") size = atoi ((*it).value.c_str());
      if (s == "color") fore = (*it).value;
      if (s == "back") back = (*it).value;
    }
    if (face.empty())
      face = state->fontFace();
    if (size == 0)
      size = state->fontSize();
    RGB fg = state->fgColor();
    RGB bg = state->bgColor();
    if (!fore.empty())
      fg = cMXPColors::self()->color (fore);
    if (!back.empty())
      bg = cMXPColors::self()->color (back);
    state->gotFONT (face, size, fg, bg);
  }
  else if (name == "p")
    state->gotP();
  else if (name == "br")
    state->gotBR();
  else if (name == "nobr")
    state->gotNOBR();
  else if (name == "sbr")
    state->gotSBR();
  else if (name == "a")
  {
    string href, hint, expire;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "href") href = (*it).value;
      if (s == "hint") hint = (*it).value;
      if (s == "expire") expire = (*it).value;
    }
    state->gotA (href, hint, expire);
  }
  else if (name == "send")
  {
    string href, hint, expire;
    bool prompt = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "href") href = (*it).value;
      if (s == "hint") hint = (*it).value;
      if (s == "expire") expire = (*it).value;
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
      if (*it2 == "prompt") prompt = true;
    state->gotSEND (href, hint, prompt, expire);
  }
  else if (name == "expire")
  {
    string name;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
    }
    //name can be empty - all named links shall then expire
    state->gotEXPIRE (name);
  }
  else if (name == "version")
    state->gotVERSION();
  else if (name == "h1")
    state->gotHtag (1);
  else if (name == "h2")
    state->gotHtag (2);
  else if (name == "h3")
    state->gotHtag (3);
  else if (name == "h4")
    state->gotHtag (4);
  else if (name == "h5")
    state->gotHtag (5);
  else if (name == "h6")
    state->gotHtag (6);
  else if (name == "hr")
    state->gotHR();
  else if (name == "small")
    state->gotSMALL();
  else if (name == "tt")
    state->gotTT();
  else if (name == "sound")
  {
    string fname, t, u;
    int v = 0, l = 0, p = 0;  //shall be overridden by defaults...
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "fname") fname = (*it).value;
      if (s == "t") t = (*it).value;
      if (s == "u") u = (*it).value;
      if (s == "v") v = atoi ((*it).value.c_str());
      if (s == "l") l = atoi ((*it).value.c_str());
      if (s == "p") p = atoi ((*it).value.c_str());
    }
    if (fname.empty())
    {
      results->addToList (results->createError ("Received SOUND tag with no file name!"));
      return;
    }
    if ((v < 0) ||  (v > 100))
    {
      results->addToList (results->createWarning ("Ignoring incorrect V param for SOUND tag."));
      v = 100;  //set default value
    }
    if ((l < -1) || (l > 100) || (l == 0))
    {
      results->addToList (results->createWarning ("Ignoring incorrect L param for SOUND tag."));
      l = 1;  //set default value
    }
    if ((p < 0) ||  (p > 100))
    {
      results->addToList (results->createWarning ("Ignoring incorrect P param for SOUND tag."));
      p = 50;  //set default value
    }
    state->gotSOUND (fname, v, l, p, t, u);
  }
  else if (name == "music")
  {
    string fname, t, u;
    int v = 0, l = 0, c = 0;  //shall be overridden by defaults...
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "fname") fname = (*it).value;
      if (s == "t") t = (*it).value;
      if (s == "u") u = (*it).value;
      if (s == "v") v = atoi ((*it).value.c_str());
      if (s == "l") l = atoi ((*it).value.c_str());
      if (s == "c") c = atoi ((*it).value.c_str());
    }
    if (fname.empty())
    {
      results->addToList (results->createError ("Received MUSIC tag with no file name!"));
      return;
    }
    if ((v < 0) ||  (v > 100))
    {
      results->addToList (results->createWarning ("Ignoring incorrect V param for MUSIC tag."));
      v = 100;  //set default value
    }
    if ((l < -1) || (l > 100) || (l == 0))
    {
      results->addToList (results->createWarning ("Ignoring incorrect L param for MUSIC tag."));
      l = 1;  //set default value
    }
    if ((c != 0) && (c != 1))
    {
      results->addToList (results->createWarning ("Ignoring incorrect C param for MUSIC tag."));
      c = 1;  //set default value
    }
    state->gotMUSIC (fname, v, l, (c!=0), t, u);
  }
  else if (name == "gauge")
  {
    string entity, max, caption, color;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "entity") entity = (*it).value;
      if (s == "max") max = (*it).value;
      if (s == "caption") caption = (*it).value;
      if (s == "color") color = (*it).value;
    }
    if (entity.empty())
    {
      results->addToList (results->createError ("Received GAUGE with no entity name!"));
      return;
    }
    RGB c;
    if (color.empty()) color = "white";
    c = cMXPColors::self()->color (color);
    state->gotGAUGE (entity, max, caption, c);
  }
  else if (name == "stat")
  {
    string entity, max, caption;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "entity") entity = (*it).value;
      if (s == "max") max = (*it).value;
      if (s == "caption") caption = (*it).value;
    }
    if (entity.empty())
    {
      results->addToList (results->createError ("Received STAT with no entity name!"));
      return;
    }
    state->gotSTAT (entity, max, caption);
  }
  else if (name == "frame")
  {
    string name, action, title, align;
    int left = 0, top = 0, width = 0, height = 0;
    bool finternal = false, fscroll = false, ffloat = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "action") action = (*it).value;
      if (s == "title") title = (*it).value;
      if (s == "align") align = (*it).value;
      if (s == "left") left = state->computeCoord ((*it).value.c_str(), true);
      if (s == "top") top = state->computeCoord ((*it).value.c_str(), false);
      if (s == "width") width = state->computeCoord ((*it).value.c_str(), true);
      if (s == "height") height = state->computeCoord ((*it).value.c_str(), false);
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
    {
      if (*it2 == "internal") finternal = true;
      if (*it2 == "scrolling") fscroll = true;
      if (*it2 == "floating") ffloat = true;
    }
    if (name.empty())
    {
      results->addToList (results->createError ("Received FRAME tag with no frame name!"));
      return;
    }
    state->gotFRAME (name, action, title, finternal, align, left, top, width, height,
        fscroll, ffloat);
  }
  else if (name == "dest")
  {
    string name;
    int x = 0, y = 0;
    bool feol = false, feof = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "x") x = atoi ((*it).value.c_str());
      if (s == "y") y = atoi ((*it).value.c_str());
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
    {
      if (*it2 == "eol") feol = true;
      if (*it2 == "eof") feof = true;
    }
    if (name.empty())
    {
      results->addToList (results->createError ("Received DEST tag with no frame name!"));
      return;
    }
    state->gotDEST (name, x, y, feol, feof);
  }
  else if (name == "relocate")
  {
    string name;
    int port = 0;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "name") name = (*it).value;
      if (s == "port") port = atoi ((*it).value.c_str());
    }
    if (name.empty())
    {
      results->addToList (results->createError ("Received RELOCATE tag with no server name!"));
      return;
    }
    if (port == 0)
    {
      results->addToList (results->createError ("Received RELOCATE tag with no server port!"));
      return;
    }
    state->gotRELOCATE (name, port);
  }
  else if (name == "user")
    state->gotUSER();
  else if (name == "password")
    state->gotPASSWORD();
  else if (name == "image")
  {
    string name, url, t, align;
    int h = 0, w = 0, hspace = 0, vspace = 0;
    bool fismap = false;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "fname") name = (*it).value;
      if (s == "url") url = (*it).value;
      if (s == "t") t = (*it).value;
      if (s == "align") align = (*it).value;
      if (s == "h") h = state->computeCoord ((*it).value.c_str(), true, true);
      if (s == "w") w = state->computeCoord ((*it).value.c_str(), false, true);
      if (s == "hspace") hspace = atoi ((*it).value.c_str());
      if (s == "vspace") vspace = atoi ((*it).value.c_str());
    }
    for (it2 = flags.begin(); it2 != flags.end(); ++it2)
      if (*it2 == "ismap") fismap = true;
    if (name.empty())
    {
      results->addToList (results->createError ("Received IMAGE tag with no image name!"));
      return;
    }
    state->gotIMAGE (name, url, t, h, w, hspace, vspace, align, fismap);
  }
  else if (name == "filter")
  {
/*
    string src, dest, name;
    for (it = params.begin(); it != params.end(); ++it)
    {
      string s = (*it).name;
      if (s == "src") src = (*it).value;
      if (s == "dest") dest = (*it).value;
      if (s == "name") name = (*it).value;
    }
    state->gotFILTER (src, dest, name);
*/
    results->addToList (results->createWarning ("Ignoring unsupported FILTER tag."));
  }
}

void cElementManager::processSupport (const list<sParam> &params)
{
  list<string> pars;
  list<sParam>::const_iterator it;
  for (it = params.begin(); it != params.end(); ++it)
    pars.push_back ((*it).value);
  state->gotSUPPORT (pars);
}

void cElementManager::processCustomTag (const string &name, const list<sParam> &params)
{
  //generate a mapping with all parameter values
  paramexpander->reset();
  list<sParam>::const_iterator itp;
  for (itp = params.begin(); itp != params.end(); ++itp)
    //assign parameter value... default values and stuff were already expanded
    paramexpander->addEntity ((*itp).name, "'" + (*itp).value + "'");
  
  //process tag contents one by one
  list<sElementPart *>::iterator it;
  for (it = elements[name]->element.begin(); it != elements[name]->element.end(); ++it)
  {
    sElementPart *sep = *it;
    string part = sep->text;
    
    //expand tag parameters first
    part = paramexpander->expandEntities (part, true);
    
    //parameters are expanded, process this part
    if (sep->istag)
      //this part is another tag - expand it recursively
      gotTag (part);
    else
      //this part is regular text
      state->gotText (part);
  }

  //tag processed, send flag information, is any
  if (!elements[name]->flag.empty())
    state->gotFlag (true, elements[name]->flag);
}

enum paramParserState {
  parNone,
  parName,
  parValue,
  parQuotedValue
};


void cElementManager::processParamList (const string &params, list<string> &attlist,
    map<string, string> &attdefault)
{
  //this is similar to the parser in gotTag(), but it's a bit simpler...
  
  string name, value;
  char quote;
  paramParserState state = parNone;
  string::const_iterator it;
  for (it = params.begin(); it != params.end(); ++it)
  {
    char ch = *it;

    //process character
    switch (state) {
      case parNone: {
        if (ch != ' ')
        {
          state = parName;
          name += ch;
        }
        break;
      }
      case parName: {
        if (ch == '=')
          state = parValue;
        else if (ch == ' ')
        {
          //new parameter, no default value
          attlist.push_back (lcase (name));
          name = "";
          state = parNone;
        }
        else
          name += ch;
        break;
      }
      case parValue: {
        if (ch == ' ')
        {
          //new parameter, with default value
          attlist.push_back (lcase (name));
          attdefault[name] = value;
          name = "";
          value = "";
          state = parNone;
        }
        else if (value.empty() && ((ch == '\'') || (ch == '"')))
        {
          state = parQuotedValue;
          quote = ch;
        }
        else
          value += ch;
        break;
      }
      case parQuotedValue: {
        if (ch == quote)
        {
          //new parameter, with default value
          attlist.push_back (lcase (name));
          attdefault[name] = value;
          name = "";
          value = "";
          state = parNone;
        }
        else
          value += ch;
        break;
      }
    };
  }

  //last parameter...
  switch (state) {
    case parName: {
      //new parameter, no default value
      attlist.push_back (lcase (name));
    }
    break;
    case parValue: {
      //new parameter, with default value
      attlist.push_back (lcase (name));
      attdefault[name] = value;
      break;
    }
    case parQuotedValue:
      results->addToList (results->createWarning (
          "Received tag definition with unfinished quoted default parameter value!"));
      //new parameter, with default value (unfinished, but hey...)
      attlist.push_back (lcase (name));
      attdefault[name] = value;
    break;
  };
  
  //everything done...
}

void cElementManager::addElement (const string &name, list<sElementPart *> contents,
    list<string> attlist, map<string, string> attdefault, bool open, bool empty,
    int tag, string flag)
{
  //sanity checks
  if (elementDefined (name))
  {
    results->addToList (results->createError ("Multiple definition of element " + name + "!"));
    return;
  }

  sElement *e = new sElement;
  e->open = open;
  e->empty = empty;
  if ((tag >= 20) && (tag <= 99))
  {
    e->tag = tag;
    if (lineTags.count (tag))
      results->addToList (results->createError ("Element " + name +
          " uses an already assigned line tag!"));
    lineTags[tag] = name;
  }
  else
    e->tag = 0;
  e->flag = flag;

  //assign element contents, generating the list of closing tags
  e->element.clear();
  list<sElementPart *>::iterator it;
  for (it = contents.begin(); it != contents.end(); ++it)
  {
    sElementPart *ep = *it;
    if (ep->istag)
    {
      string tag = lcase (firstword (ep->text));
      if (elementDefined (tag))
      {
        if (open && !(openElement (tag)))
        {
          delete ep;
          results->addToList (results->createError ("Definition of open " + name +
              " tag contains secure tag " + tag + "!"));
        }
        else if (empty && !(emptyElement (tag)))
        {
          delete ep;
          results->addToList (results->createError ("Definition of empty " + name +
              " tag contains non-empty tag " + tag + "!"));
        }
        else
        {
          e->element.push_back (ep);
          if (!emptyElement(tag)) e->closingseq.push_front (tag);
        }
      }
      else
      {
        //element doesn't exist yet - we must believe that it's correct
        e->element.push_back (ep);
        if (!empty) e->closingseq.push_front (tag);
        results->addToList (results->createWarning ("Definition of element " + name +
            " contains undefined element " + tag + "!"));
      }
    }
    else
      e->element.push_back (ep);
  }

  //assign the element definition
  elements[name] = e;

  //set attribute list
  setAttList (name, attlist, attdefault);
}

void cElementManager::setAttList (const string &name, list<string> attlist,
    map<string, string> attdefault)
{
  //sanity check
  if (!elements.count (name))
  {
    results->addToList (results->createWarning ("Received attribute list for undefined tag " +
        name + "!"));
    return;
  }

  sElement *e = elements[name];
  e->attlist.clear();
  e->attdefault.clear();
  e->attlist = attlist;
  e->attdefault = attdefault;
}

void cElementManager::removeElement (const string &name)
{
  //external elements only
  if (elements.count (name))
  {
    sElement *e = elements[name];
    list<sElementPart *>::iterator it;
    for (it = e->element.begin(); it != e->element.end(); ++it)
      delete *it;
    e->element.clear();
    e->attlist.clear();
    e->attdefault.clear();
    e->closingseq.clear();
    if (e->tag)
      lineTags.erase (e->tag);
    delete e;
    elements.erase (name);
  }
}

void cElementManager::removeAll ()
{
  //external elements only
  
  list<string> names;
  //we cannot delete definitions directly, because that would invalidate our iterator
  map<string, sElement *>::iterator it;
  for (it = elements.begin(); it != elements.end(); ++it)
    names.push_back (it->first);
  list<string>::iterator it2;
  for (it2 = names.begin(); it2 != names.end(); ++it2)
    removeElement (*it2);
  names.clear ();
}
