/***************************************************************************
                          cscript.h  -  one script
                             -------------------
    begin                : So dec 7 2002
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

#ifndef CSCRIPT_H
#define CSCRIPT_H

#include "clistobject.h"

class cRunningScript;

/**
Information about one script.
  *@author Tomas Mecir
  */

class cScript : public cListObject {
public:
  virtual ~cScript ();

  virtual void updateVisibleName ();
protected:
  friend class cScriptList;
  cScript (cList *list);

#define SCRIPT_FIND 1
#define SCRIPT_EXECUTE 2

  virtual cList::TraverseAction traverse (int traversalType);

  /** this method creates a new instance of cRunningScript and prepares
  it for launching */
  cRunningScript *prepareToLaunch (const QString &params);
  /** This is called by cRunningScript when its script starts. */
  void scriptIsStarting ();
  /** This is called by cRunningScript when its script terminates. */
  void scriptIsTerminating ();
  
  /** some variables that are not stored */
  int runningCount;

  friend class cRunningScript;
};

#endif

