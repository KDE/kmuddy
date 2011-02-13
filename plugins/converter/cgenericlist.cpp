//
// C++ Implementation: cgenericlist
//
// Description: A generic list that can only store data.
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

#include "cgenericlist.h"

#include "cgenericitem.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <kdebug.h>

cGenericList::cGenericList () :
    cList ("generic")
{
}

cGenericList::~cGenericList ()
{
}


cListObject *cGenericList::newObject ()
{
  return new cGenericItem (this);
}

void cGenericList::init ()
{
  initRootGroup ();
}

bool cGenericList::saveList (const QString &file)
{
   // save the list contents
  QFile f (file);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    kDebug() << "Unable to open " << file << " for writing." << endl;
    return false;  // problem
  }

  QXmlStreamWriter *writer = new QXmlStreamWriter (&f);
  save (writer);

  f.close ();
  delete writer;
  return true;
}

void cGenericList::intProperty (const QString &name, int defaultValue)
{
  addIntProperty (name, QString(), defaultValue);
}

void cGenericList::stringProperty (const QString &name, QString defaultValue)
{
  addStringProperty (name, QString(), defaultValue);
}

void cGenericList::boolProperty (const QString &name, bool defaultValue)
{
  addBoolProperty (name, QString(), defaultValue);
}



