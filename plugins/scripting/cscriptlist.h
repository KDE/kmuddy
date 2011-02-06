/***************************************************************************
                          cscriptlist.h  -  list of scripts
                             -------------------
    begin                : Pi dec 13 2002
    copyright            : (C) 2002-2009 by Tomas Mecir
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

#ifndef CSCRIPTLIST_H
#define CSCRIPTLIST_H

#include "clist.h"

class cScript;

/**
List of scripts.
  *@author Tomas Mecir
  */

class cScriptList : public cList  {
public: 
  cScriptList ();
  ~cScriptList ();

  static cList *newList () { return new cScriptList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Script"; }
  virtual cListEditor *editor (QWidget *parent);

  /** run script with name */
  bool runScript (QString name, const QString &paramlist = QString());
  /** does script with such name exist? */
  bool nameExists (const QString &name);
protected:
  struct Private;
  Private *d;
  friend class cScript;

  QString nameToFind ();
  void setNameFound ();

  /** run this script! */
  bool runScript (cScript *script);
};

#endif
