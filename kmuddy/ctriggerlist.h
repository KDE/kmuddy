//
// C++ Interface: ctriggerlist
//
// Description: list of triggers
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CTRIGGERLIST_H
#define CTRIGGERLIST_H

#include <clist.h>

#include <qdatetime.h>

class cTextChunk;

/**
List of triggers.
  *@author Tomas Mecir
  */

class cTriggerList : public cList {
public: 
  cTriggerList ();
  ~cTriggerList ();

  static cList *newList () { return new cTriggerList; };
  virtual cListObject *newObject () override;
  virtual QString objName () override { return "Trigger"; }
  virtual cListEditor *editor (QWidget *parent) override;

  /** Sends these commands via cCmdProcessor. */
  virtual void processCommands (const QStringList &commands);
  /** Perform matching of string against items of the list. Each match produces command(s),
  which are passed to processCommands() */
  virtual void matchString (cTextChunk *line);
  bool detectingPrompt ();
  void setDetectingPrompt (bool val);
  
  /** rewrite text - called by rewrite triggers */
  void rewriteText (int pos, int len, const QString &newtext);
protected:
  friend class cTrigger;
  struct Private;
  Private *d;

  cTextChunk *lineToMatch ();
  QString stringToMatch ();
};

#endif
