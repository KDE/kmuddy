//
// C++ Interface: cmudlist
//
// Description: A list of MUDs from the MUD Connector database.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CMUDLIST_H
#define CMUDLIST_H

#include <QString>
#include <QStringList>

class QAbstractTableModel;

struct cMUDEntry {
  QString name, codebase, www;
  QString host, ip;
  int port;
  QStringList desc;
};

/** This class manages the list of MUDs from the MUD Connector database. */

class cMUDList {
 public:
  /** constructor */
  cMUDList ();
  /** destructor */
  ~cMUDList ();

  /** Returns the model containing all the data. */
  QAbstractTableModel *model ();

  /** Returns the version of the input file. */
  int version ();
  /** Returns item count. */
  int count ();
  /** Returns a single entry. */
  const cMUDEntry *entry (int id);
 private:
  /** Load the list. */
  void load ();
  /** Load the list from the specified file. */
  void load (const QString &file);

  struct Private;
  Private *d;
};

#endif  // CMUDLIST_H

