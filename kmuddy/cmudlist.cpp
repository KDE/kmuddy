//
// C++ Implementation: cmudlist
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

#include "cmudlist.h"

#include <cglobalsettings.h>

#include <klocale.h>

#include <QAbstractTableModel>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QXmlStreamReader>

#include <vector>

using namespace std;

class cMUDListModel : public QAbstractTableModel {
 public:
  cMUDListModel (cMUDList *l) :
    QAbstractTableModel (nullptr), lst (l)
  {
  }
  ~cMUDListModel () override {}

  int columnCount (const QModelIndex &parent = QModelIndex()) const override
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return 6;  // we have 6 columns
  }

  int rowCount (const QModelIndex &parent = QModelIndex()) const override
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return lst->count();
  }

  QVariant headerData ( int section, Qt::Orientation /*orientation*/,
      int role = Qt::DisplayRole ) const override
  {
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
      case 0: return i18n ("Name");
      case 1: return i18n ("Server");
      case 2: return i18n ("Port");
      case 3: return i18n ("WWW");
      case 4: return i18n ("IP");
      case 5: return i18n ("Codebase");
      default: return QVariant();
    }
  }

  QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole) const override
  {
    // display and user role only
    if ((role != Qt::DisplayRole) && (role != Qt::UserRole)) return QVariant();

    if (index.parent().isValid()) return QVariant();
    int row = index.row();
    int col = index.column();
    if ((col < 0) || (col > 5)) return QVariant();
    if ((row < 0) || (row > lst->count())) return QVariant();

    const cMUDEntry *entry = lst->entry (row);

    // the user role gives us a pointer to the entry
    if (role == Qt::UserRole)
      return entry->desc;

    // we want the displayed text - so fetch it and return it
    switch (col) {
      case 0: return entry->name;
      case 1: return entry->host;
      case 2: return entry->port;
      case 3: return entry->www;
      case 4: return entry->ip;
      case 5: return entry->codebase;
      default: return QVariant();
    };
  }

 private:
  cMUDList *lst;
};

struct cMUDList::Private {
  vector<cMUDEntry *> lst;
  int version;
  cMUDListModel *model;
};

cMUDList::cMUDList ()
{
  d = new Private;
  d->version = 0;
  load ();
  d->model = new cMUDListModel (this);
}

cMUDList::~cMUDList ()
{
  // delete all entries
  vector<cMUDEntry *>::iterator it;
  for (it = d->lst.begin(); it != d->lst.end(); ++it)
    delete *it;

  delete d;
}

QAbstractTableModel *cMUDList::model ()
{
  return d->model;
}

int cMUDList::version ()
{
  return d->version;
}

int cMUDList::count ()
{
  return d->lst.size();
}

const cMUDEntry *cMUDList::entry (int id)
{
  if (id < 0) return nullptr;
  if (id >= count()) return nullptr;
  return d->lst[id];
}

void cMUDList::load ()
{
  // first try a local copy of the file in the profile directory
  QString path = cGlobalSettings::self()->profilePath();
  if (QFile::exists (path + "/muds.xml")) {
    load (path + "/muds.xml");
    return;
  }

  // no local copy exists, try the global file (or a local file in .kde)
  QString file = QStandardPaths::locate(QStandardPaths::AppDataLocation, "muds.xml");
  if (!file.isEmpty()) {
    load (file);
    return;
  }
}

void cMUDList::load (const QString &file)
{
  QFile f (file);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "No MUD list file - nothing to do.";
    return;  // nothing to do
  }
  QXmlStreamReader *reader = new QXmlStreamReader (&f);

  // load the list
  reader->readNext ();  // read the document start
  reader->readNext ();
  if (reader->isStartElement ())
    if (reader->name() == "mudlist") {
      d->version = reader->attributes().value ("version").toString().toInt();
      // okay, read the list
      while (!reader->atEnd()) {
        reader->readNext ();
        if (reader->isStartElement () && (reader->name() == "mud")) {
          // found another entry
          cMUDEntry *entry = new cMUDEntry;
          entry->port = 0;

          // now read the entry
          while (!reader->atEnd()) {
            reader->readNext ();
            // Error ? Break out.
            if (reader->hasError()) break;
            // Done here ?
            if (reader->isEndElement() && (reader->name() == "mud")) break;
            // Anything other than a start element is ignored here.
            if (!reader->isStartElement()) continue;
            QString tag = reader->name().toString();
            // read the contents of the tag - be sure to read everything including entities
            QString value;
            while (!reader->isEndElement()) {
              reader->readNext ();
              if (reader->hasError()) break;
              if (reader->isCharacters()) value += reader->text().toString();
            }
            // now place it to where it belongs
            if (tag == "name") entry->name = value;
            if (tag == "codebase") entry->codebase = value;
            if (tag == "host") entry->host = value;
            if (tag == "port") entry->port = value.toInt();
            if (tag == "ip") entry->ip = value;
            if (tag == "www") entry->www = value;
            if (tag == "desc") entry->desc << value;  // desc is split across paragraphs
          }
          // ensure validity of the entry
          if (entry->host.isEmpty() || (entry->port == 0)) {
            // we need host and port, if we don't have it, ignore this entry
            delete entry;
          } else {
            // all is well - add it to our list
            d->lst.insert (d->lst.end(), entry);
          }
        }
      }
    } else reader->raiseError ("This is not a valid profile list file.");
  else reader->raiseError ("This file is corrupted.");

  if (reader->hasError()) {
    qDebug() << ("Error in profiles.xml at line " + QString::number (reader->lineNumber()) + ", column " + QString::number (reader->columnNumber()) + QString (": ") + reader->errorString());
  }

  // close the file
  f.close ();
  delete reader;
}




