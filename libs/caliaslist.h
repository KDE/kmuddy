//
// C++ Interface: caliaslist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CALIASLIST_H
#define CALIASLIST_H

#include <clist.h>

#include <QStringList>

#include <kmuddy_export.h>

/**
A list of aliases.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cAliasList : public cList {
public: 
  cAliasList ();
  ~cAliasList ();

  static cList *newList () { return new cAliasList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Alias"; }
  virtual cListEditor *editor (QWidget *parent);

  bool matchString (const QString &string);
  QStringList commandsToExec ();

protected:
  friend class cAlias;
  struct Private;
  Private *d;

  /** String that's to be matched - used by cAlias. */
  QString stringToMatch ();
  /** Original command should also be sent - used by cAlias. */
  void wantOriginalCommand ();
  /** At least one alias has matched - used by cAlias.  */
  void setMatched ();
  /** Add a replacement command to execute - used by cAlias. */
  void addCommand (const QString &command);
  /** Check alias condition.  */
  void checkCondition (const QString &condition);
};

#endif
