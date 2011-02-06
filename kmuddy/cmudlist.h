//
// C++ Interface: cmudlist
//
// Description: A list of MUDs from the MUD Connector database.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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

