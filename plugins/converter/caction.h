/***************************************************************************
                          caction.h  -  action toolbar item
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne nov 3 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#ifndef CACTION_H
#define CACTION_H

#include "csaveablefield.h"

#include <qstring.h>

/**
One action in the action toolbar.

  *@author Tomas Mecir
  */

class cAction : public cSaveableField  {
public: 
  cAction (int _sess);
  ~cAction () override;
  /** creates a new instance of the class; this is needed because I need to
  create instances of childclasses from within this class, but I don't know
  the exact type of that instance... */
  cSaveableField *newInstance () override;

  /** load data from a config file*/
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_ACTION; };

  void setCaption (const QString &s);
  QString getCaption ();
  void setCommand (const QString &s);
  QString getCommand ();
  void setCommand2 (const QString &s);
  QString getCommand2 ();
  void setPushDown (bool how);
  bool isPushDown ();
  void setIconName (const QString &s);
  QString getIconName ();

protected:
  int sess;
  QString caption, command, command2;
  QString iconname;
  bool pushdown;
};

#endif
