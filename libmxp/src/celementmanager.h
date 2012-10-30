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
#ifndef CELEMENTMANAGER_H
#define CELEMENTMANAGER_H

class cEntityManager;
class cMXPParser;
class cMXPState;
class cResultHandler;

#include <list>
#include <map>
#include <string>

using namespace std;


struct sElementPart {
  bool istag;
  string text;
};

/** one external element (defined by <!element>) */
struct sElement {
  /** is it an open element? */
  bool open;
  /** is it an element with no closing tag? */
  bool empty;
  /** tag associated with this element */
  int tag;
  /** flag associated with this element */
  string flag;
  /** list of all element contents */
  list<sElementPart *> element;
  /** list of element attributes in the right order */
  list<string> attlist;
  /** default values for attributes */
  map<string, string> attdefault;
  /** closing sequence */
  list<string> closingseq;
};

/** one internal element */
struct sInternalElement {
  /** is it an open element? */
  bool open;
  /** is it an element with no closing tag? */
  bool empty;
  /** list of element attributes in the right order */
  list<string> attlist;
  /** default values for attributes; if there's an empty string (but defined), then it's a flag */
  map<string, string> attdefault;
};

/** one parameter in one tag :-) */
struct sParam {
  bool flag;
  string name, value;
};

/**
Element manages handles a list of user-defined elements, it also supports second-level parsing of incoming elements.

@author Tomas Mecir
*/

class cElementManager {
 public:
  /** constructor */
  cElementManager (cMXPState *st, cResultHandler *res, cEntityManager *enm);
  /** destructor */
  ~cElementManager ();
  
  /** set pointer to cMXPState - needed due to circular dependencies */
  void assignMXPState (cMXPState *st);

  void reset ();

  /** is this element defined? */
  bool elementDefined (const string &name);
  /** is it an internal tag? */
  bool internalElement (const string &name);
  /** is it a custom element? (i.e. defined via <!element>) */
  bool customElement (const string &name);
  /** open element? */
  bool openElement (const string &name);
  /** empty element? i.e. does it need a closing tag? */
  bool emptyElement (const string &name);

  /** we've got a new tag, parameter contains the tag without <> chars */
  void gotTag (const string &tag);

  /** got a line tag - expand it to an associated tag, if any */
  void gotLineTag (int number);
  /** got newline - expand last line-tag to closing tag, if needed */
  void gotNewLine ();
protected:
  /** identify flags in an internal tag */
  void identifyFlags (const map<string, string> &attdefault, list<sParam> &args);
  /** create a list of parameters with their names */
  void handleParams (const string &tagname, list<sParam> &args,
      const list<string> &attlist, const map<string, string> &attdefault);

  void handleClosingTag (const string &name);

  /** process internal tag, the list consists of pairs name, value */
  void processInternalTag (const string &name, const list<sParam> &params,
      const list<string> &flags);
  /** process <support> tag, which has a special syntax */
  void processSupport (const list<sParam> &params);
  /** process external tag */
  void processCustomTag (const string &name, const list<sParam> &params);

  /** process a list of parameters for !ELEMENT and !ATTLIST */
  void processParamList (const string &params, list<string> &attlist,
      map<string, string> &attdefault);
  /** add a new element */
  void addElement (const string &name, list<sElementPart *> contents, list<string> attlist,
      map<string, string> attdefault, bool open, bool empty, int tag, string flag);
  /** set attribute list for an existing element */
  void setAttList (const string &name, list<string> attlist, map<string, string> attdefault);
  /** remove an EXTERNAL element */
  void removeElement (const string &name);
  /** remove all EXTERNAL elements */
  void removeAll ();

  void createInternalElements ();

  /** list of all custom elements */
  map<string, sElement *> elements;
  map<string, sInternalElement *> ielements;

  /** line tags associated with elements */
  map<int, string> lineTags;

  /** aliases for internal elements */
  map<string, string> aliases;

  /** last line tag */
  int lastLineTag;

  /** state class */
  cMXPState *state;
  /** result handler */
  cResultHandler *results;
  /** entity manager */
  cEntityManager *entities;
  /** expander of parameters in custom tags */
  cEntityManager *paramexpander;
  /** parser of custom element definitions */
  cMXPParser *parser;
  
};

#endif
