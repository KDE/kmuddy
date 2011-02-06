//
// C++ Interface: ctriggerlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
  virtual cListObject *newObject ();
  virtual QString objName () { return "Trigger"; }
  virtual cListEditor *editor (QWidget *parent);

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
