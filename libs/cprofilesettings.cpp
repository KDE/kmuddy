//
// C++ Implementation: cProfileSettings
//
// Description: profile settings
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

#include "cprofilesettings.h"

#include "cprofilemanager.h"

#include <kdebug.h>

#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <map>

using namespace std;

struct cProfileSettings::Private {
  QString profileId;
  map<QString, bool> boolVals;
  map<QString, int> intVals;
  map<QString, QString> strVals;
};

cProfileSettings::Private *cProfileSettings::defd = 0;

cProfileSettings::cProfileSettings (QString profileId)
{
  d = new Private;
  d->profileId = profileId;
  initDefaultStorage ();
  load ();
}

cProfileSettings::~cProfileSettings ()
{
  // we must NOT save here
  delete d;
}

void cProfileSettings::setBool (const QString &name, bool value)
{
  d->boolVals[name] = value;
}

void cProfileSettings::setInt (const QString &name, int value)
{
  d->intVals[name] = value;
}

void cProfileSettings::setString (const QString &name, const QString &value)
{
  d->strVals[name] = value;
}

bool cProfileSettings::getBool (const QString &name) const
{
  if (d->boolVals.count (name))
    return d->boolVals[name];
  if (defd->boolVals.count (name))
    return defd->boolVals[name];
  return false;
}

int cProfileSettings::getInt (const QString &name) const
{
  if (d->intVals.count (name))
    return d->intVals[name];
  if (defd->intVals.count (name))
    return defd->intVals[name];
  return 0;
}

QString cProfileSettings::getString (const QString &name) const
{
  if (d->strVals.count (name))
    return d->strVals[name];
  if (defd->strVals.count (name))
    return defd->strVals[name];
  return QString();
}

void cProfileSettings::setDefaultBool (const QString &name, bool value)
{
  initDefaultStorage ();
  defd->boolVals[name] = value;
}

void cProfileSettings::setDefaultInt (const QString &name, int value)
{
  initDefaultStorage ();
  defd->intVals[name] = value;
}

void cProfileSettings::setDefaultString (const QString &name, const QString &value)
{
  initDefaultStorage ();
  defd->strVals[name] = value;
}

void cProfileSettings::initDefaultStorage ()
{
  if (defd) return;
  defd = new Private;
  fillDefaultValues ();
}

void cProfileSettings::fillDefaultValues ()
{
  setDefaultBool ("use-ansi", true);
  setDefaultBool ("limit-repeater", true);
  setDefaultString ("encoding", "ISO 8859-1");
  setDefaultBool ("startup-negotiate", true);
  setDefaultBool ("lpmud-style", false);
  setDefaultBool ("prompt-label", false);
  setDefaultBool ("prompt-status", true);
  setDefaultBool ("prompt-console", true);
  setDefaultBool ("auto-adv-transcript", false);

  QString movecmds[10];
  movecmds[0] = "n"; movecmds[1] = "ne";
  movecmds[2] = "e"; movecmds[3] = "se";
  movecmds[4] = "s"; movecmds[5] = "sw";
  movecmds[6] = "w"; movecmds[7] = "nw";
  movecmds[8] = "u"; movecmds[9] = "d";
  for (int i = 0; i < 10; i++)
    setDefaultString ("movement-command-"+QString::number(i), movecmds[i]);
  setDefaultString ("transcript-directory", QDir::homePath());

  setDefaultInt ("sound-dir-count", 0);
  setDefaultBool ("use-msp", true);
  setDefaultBool ("always-msp", false);
  setDefaultBool ("midline-msp", false);

  setDefaultInt ("use-mxp", 3);
  setDefaultString ("mxp-variable-prefix", QString());
}


void cProfileSettings::save ()
{
  cProfileManager *pm = cProfileManager::self();

  QString path = pm->profilePath (d->profileId);
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  dir.remove ("settings.backup");
  
  if (!QFile(path + "/settings.xml").copy (path + "/settings.backup")) {
    kDebug() << "Unable to backup settings.xml." << endl;  // not fatal, may simply not exist
  }
  
  QFile f (path + "/settings.xml");
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    kDebug() << "Unable to open settings.xml for writing." << endl;
    return;  // problem
  }
  // save the profile file
  QXmlStreamWriter *writer = new QXmlStreamWriter (&f);

  writer->setAutoFormatting (true);  // make the generated XML more readable
  writer->writeStartDocument ();

  writer->writeStartElement ("profile");
  writer->writeAttribute ("version", "1.0");

  // boolean values
  map<QString, bool>::iterator itb;
  for (itb = d->boolVals.begin(); itb != d->boolVals.end(); ++itb) {
    writer->writeStartElement ("setting");
    writer->writeAttribute ("type", "bool");
    writer->writeAttribute ("name", itb->first);
    writer->writeAttribute ("value", itb->second ? "true" : "false");
    writer->writeEndElement ();
  }

  // integer values
  map<QString, int>::iterator it;
  for (it = d->intVals.begin(); it != d->intVals.end(); ++it) {
    writer->writeStartElement ("setting");
    writer->writeAttribute ("type", "integer");
    writer->writeAttribute ("name", it->first);
    writer->writeAttribute ("value", QString::number (it->second));
    writer->writeEndElement ();
  }

  // string values
  map<QString, QString>::iterator it2;
  for (it2 = d->strVals.begin(); it2 != d->strVals.end(); ++it2) {
    writer->writeStartElement ("setting");
    writer->writeAttribute ("type", "string");
    writer->writeAttribute ("name", it2->first);
    writer->writeAttribute ("value", it2->second);
    writer->writeEndElement ();
  }

  writer->writeEndElement ();
  writer->writeEndDocument ();

  f.close ();
  delete writer;
}

void cProfileSettings::load ()
{
  cProfileManager *pm = cProfileManager::self();
  
  QString path = pm->profilePath (d->profileId);
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  QFile f (path + "/settings.xml");
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    kWarning() << "No settings file - nothing to do." << endl;
    return;  // no profiles - nothing to do
  }
  QXmlStreamReader *reader = new QXmlStreamReader (&f);
  
  reader->readNext ();  // read the document start
  reader->readNext ();
  if (reader->isStartElement ())
    if (reader->name() == "profile")
      if (reader->attributes().value ("version") == "1.0") {
        // okay, read the list
        while (!reader->atEnd()) {
          reader->readNext ();
          if (reader->isStartElement () && (reader->name() == "setting")) {
            QString type = reader->attributes().value ("type").toString();
            QString name = reader->attributes().value ("name").toString();
            QString value = reader->attributes().value ("value").toString();
            if (type == "integer")
              setInt (name, value.toInt());
            else if (type == "bool")
              setBool (name, (value == "true"));
            else if (type == "string")
              setString (name, value);
            else
              kDebug() << "Unrecognized setting type " << type << endl;
          }
        }
      } else reader->raiseError ("Unknown profile file version.");
    else reader->raiseError ("This is not a valid profile list file.");
  else reader->raiseError ("This file is corrupted.");

  if (reader->hasError()) {
    kWarning() << ("Error in settings.xml at line " + QString::number (reader->lineNumber()) + ", column " + QString::number (reader->columnNumber()) + QString (": ") + reader->errorString()) << endl;
  }

  // close the file
  f.close ();
  delete reader;
}



