//
// C++ Implementation: cgenericlist
//
// Description: A generic list that can only store data.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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



