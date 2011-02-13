//
// C++ Interface: caliaslist
//
// Description: 
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
