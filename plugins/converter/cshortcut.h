/***************************************************************************
                          cshortcut.h  -  macro key/shortcut
                             -------------------
    begin                : St máj 28 2003
    copyright            : (C) 2003 by Tomas Mecir
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

#ifndef CSHORTCUT_H
#define CSHORTCUT_H

#include "csaveablefield.h"

/**
Represents one macro key/shortcut.
  *@author Tomas Mecir
  */

class cShortcut : public cSaveableField  {
public: 
  cShortcut (int _sess);
  ~cShortcut () override;

  /** creates a new instance of the class */
  cSaveableField *newInstance () override;

  /** load data from a config file */
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_SHORTCUT; };

  int key () { return _key; };
  void setKey (int what) { _key = what; };
  int state () { return _state; };
  void setState (int what) { _state = what; };
  bool sendIt () { return sendit; };
  void setSendIt (bool what) { sendit = what; };
  bool overwriteInput () { return overwriteinput; };
  void setOverwriteInput (bool what) { overwriteinput = what; };
  
protected:
  int sess;
  
  int _key, _state;
  /** true = send, false = put to inputline */
  bool sendit;
  /**should existing text in inputline be overwritten?; only valid if
  sendit==false */
  bool overwriteinput;
  //command stored in getText/setText from cSaveableField
};

#endif
