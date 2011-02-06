//
// C++ Implementation: cProfileManager
//
// Description: profile manager
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cprofilemanager.h"

#include <QAbstractTableModel>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <kdebug.h>
#include <klocale.h>

#include <map>

#include "cprofilesettings.h"

#include "cglobalsettings.h"

using namespace std;

// cProfileModel - a model showing all the profiles.
class cProfileModel : public QAbstractTableModel {
 public:
  cProfileModel (cProfileManager *manager)
      : QAbstractTableModel (0), mgr (manager)
  {
  }

  virtual ~cProfileModel ()
  {
  }

  virtual int columnCount (const QModelIndex &parent = QModelIndex()) const
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return 4;  // we have 4 columns
  }

  virtual int rowCount (const QModelIndex &parent = QModelIndex()) const
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return mgr->profileList().size();
  }

  virtual QVariant headerData ( int section, Qt::Orientation /*orientation*/,
      int role = Qt::DisplayRole ) const
  {
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
      case 0: return i18n ("Profile name");
      case 1: return i18n ("Server");
      case 2: return i18n ("Port");
      case 3: return i18n ("Login name");
      default: return QVariant();
    }
  }

  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
  {
    // display role only
    if (role != Qt::DisplayRole) return QVariant();

    if (index.parent().isValid()) return QVariant();
    int row = index.row();
    int col = index.column();
    if ((col < 0) || (col > 3)) return QVariant();

    if ((row < 0) || (row > mgr->profileList().size())) return QVariant();
    QString profile = mgr->profileList()[row];
    // okay, we have the profile
    cProfileSettings *sett = mgr->settings (profile);
    // now fetch the requested information and return it
    switch (col) {
      case 0: return mgr->visibleProfileName (profile);
      case 1: return sett->getString ("server");
      case 2: return sett->getInt ("port");
      case 3: return sett->getString ("login");
      default: return QVariant();
    };
  }

  void rowChanged (int row) {
    emit dataChanged (index (row, 0), index(row, 3));
  }

  void addRow (int row) {
    beginInsertRows (QModelIndex(), row, row);
  }

  void rowAdded () {
    endInsertRows ();
  }

  void removeRow (int row) {
    beginRemoveRows (QModelIndex(), row, row);
  }

  void rowRemoved () {
    endRemoveRows ();
  }

 private:
   cProfileManager *mgr;
};

struct cProfileManager::Private {
  QStringList profiles;
  map<QString, QString> profileNames;
  map<int, QString> sessions;
  map<QString, cProfileSettings *> profileSettings;

  cProfileModel *model;
};

cProfileManager *cProfileManager::_self = 0;

cProfileManager *cProfileManager::self ()
{
  if (!_self) {
    _self = new cProfileManager;
    // we can't load in the constructor, because we need the self() pointer there
    _self->load ();
    /** Initialize the model. */
    _self->init ();
  }
  return _self;
}

cProfileManager::cProfileManager ()
{
  d = new Private;
}

cProfileManager::~cProfileManager ()
{
  map<QString, cProfileSettings *>::iterator it;
  for (it = d->profileSettings.begin(); it != d->profileSettings.end(); ++it)
    delete it->second;
  delete d->model;
  delete d;
}

QAbstractTableModel *cProfileManager::model () const
{
  return d->model;
}

QStringList cProfileManager::profileList () const
{
  return d->profiles;
}

bool cProfileManager::profileExists (const QString &name)
{
  return (d->profileNames.count (name) != 0);
}

QString cProfileManager::profilePath (const QString &profileName) const
{
  QString path = cGlobalSettings::self()->profilePath();
  return path + "/" + profileName + "/";
}

void cProfileManager::assignSession (int sess, const QString &profileName)
{
  if (d->profileNames.count (profileName))
    d->sessions[sess] = profileName;
}

bool cProfileManager::sessionAssigned (int sess)
{
  return (d->sessions.count (sess));
}

QString cProfileManager::profileName (int sess)
{
  if (!sessionAssigned (sess)) return QString();
  return d->sessions[sess];
}

bool cProfileManager::hasSessionAssigned (const QString &profile)
{
  map<int, QString>::iterator it;
  for (it = d->sessions.begin(); it != d->sessions.end(); ++it)
    if (it->second == profile)
      return true;
  return false;
}

void cProfileManager::unassignSession (int sess)
{
  // save session settings
  if (sessionAssigned (sess)) {
    cProfileSettings *s = settings (sess);
    if (s) s->save();
  }
  // and remove it
  d->sessions.erase (sess);
}

QString cProfileManager::profilePath (int sess)
{
  if (d->sessions.count (sess))
    return profilePath (d->sessions[sess]);
//  return QString();   // not found
  return cGlobalSettings::self()->profilePath() + "/";
}

QString cProfileManager::visibleProfileName (QString name)
{
  if (d->profileNames.count (name))
    return d->profileNames[name];
  return QString();
}

cProfileSettings *cProfileManager::settings (const QString &profileName)
{
  if (d->profileSettings.count (profileName))
    return d->profileSettings[profileName];
  return 0;
}

cProfileSettings *cProfileManager::settings (int sess)
{
  QString name;
  if (!d->sessions.count (sess)) return 0;
  name = d->sessions[sess];
  if (d->profileSettings.count (name))
    return d->profileSettings[name];
  return 0;
}

QString cProfileManager::newProfile (const QString &name)
{
  // find first available directory name

  QDir mainDir (cGlobalSettings::self()->profilePath());
  int num = 0;
  QString dirname;
  bool found = false;
  while (num < 1000000) {  // nobody will have more than a million profiles :)
    ++num;
    dirname = "profile" + QString::number (num);
    // check if the profile and/or directory name exists
    if (d->profileNames.count (dirname)) continue;
    if (mainDir.exists (dirname)) continue;
    // they do not - we're good to go
    found = true;
    break;
  }
  if (!found) return QString();

  // notify the model
  d->model->addRow (d->profiles.size());
  
  // create the directory
  mainDir.mkdir (dirname);

  // adjust the structures
  d->profiles.push_back (dirname);
  d->profileNames[dirname] = name;
  cProfileSettings *s = new cProfileSettings (dirname);
  d->profileSettings[dirname] = s;

  // we are done - notify the model
  d->model->rowAdded ();

  save ();
  return dirname;
}

bool cProfileManager::renameProfile (const QString &name, const QString &newName)
{
  d->profileNames[name] = newName;
  profileInfoChanged (name);
  save ();
  return true;
}

bool cProfileManager::deleteProfile (const QString &name, bool deleteFiles)
{
  // check if any open connection uses the profile
  if (hasSessionAssigned (name)) return false;
  if (!profileExists (name)) return false;

  int idx = d->profiles.indexOf (name);
  d->model->removeRow (idx);

  // remove the profile
  d->profiles.removeAll (name);
  d->profileNames.erase (name);
  if (d->profileSettings.count (name))
    delete d->profileSettings[name];
  d->profileSettings.erase (name);

  // delete files, if requested
  if (deleteFiles) {
    QString path = profilePath (name);
    QDir dir (path);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      dir.remove (fileInfo.fileName());
    }
    QDir mainDir (cGlobalSettings::self()->profilePath());
    mainDir.rmdir (name);
  }

  d->model->rowRemoved ();
  save ();
  return true;
}

bool cProfileManager::duplicateProfile (const QString &name, const QString &newName)
{
  // create a new profile
  QString profile = newProfile (newName);
  if (profile.isEmpty()) return false;

  // and copy over everything to it
  QString path = profilePath (name);
  QString path2 = profilePath (profile);
  QDir dir (path);
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    QFile(path + "/" + fileInfo.fileName()).copy (path2 + "/" + fileInfo.fileName());
  }

  // and reload the settings
  cProfileSettings *sett = new cProfileSettings (profile);
  if (d->profileSettings.count (profile))
    delete d->profileSettings[profile];
  d->profileSettings[profile] = sett;
  sett->save ();  // just in case the old settings have overwritten the XML

  profileInfoChanged (profile);
  save ();
  return true;
}

void cProfileManager::profileInfoChanged (const QString &name)
{
  int idx = d->profiles.indexOf (name);
  if (idx >= 0)
    d->model->rowChanged (idx);
}

// TODO: error reporting !!!
void cProfileManager::load ()
{
  // create the profile directory if it does not exist
  QString path = cGlobalSettings::self()->profilePath();
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  // create the XML loader
  QFile f (path + "/profiles.xml");
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    kDebug() << "No profiles file - nothing to do." << endl;
    return;  // no profiles - nothing to do
  }
  QXmlStreamReader *reader = new QXmlStreamReader (&f);

  // load the profiles
  reader->readNext ();  // read the document start
  reader->readNext ();
  if (reader->isStartElement ())
    if (reader->name() == "profiles")
      if (reader->attributes().value ("version") == "1.0") {
        // okay, read the list
        while (!reader->atEnd()) {
          reader->readNext ();
          if (reader->isStartElement () && (reader->name() == "profile")) {
            // found another profile
            QString id = reader->attributes().value ("id").toString();
            QString name = reader->attributes().value ("name").toString();
            if (!id.isEmpty() && (!d->profileNames.count (id))) {
              // add the profile
              d->profiles.push_back (id);
              d->profileNames[id] = name;
              cProfileSettings *sett = new cProfileSettings (id);
              d->profileSettings[id] = sett;
            }
          }
        }
      } else reader->raiseError ("Unknown profile file version.");
    else reader->raiseError ("This is not a valid profile list file.");
  else reader->raiseError ("This file is corrupted.");

  if (reader->hasError()) {
    kDebug() << ("Error in profiles.xml at line " + QString::number (reader->lineNumber()) + ", column " + QString::number (reader->columnNumber()) + QString (": ") + reader->errorString()) << endl;
  }

  // close the file
  f.close ();
  delete reader;
}

void cProfileManager::init ()
{
  d->model = new cProfileModel (this);
}

void cProfileManager::save ()
{
  // create the profile directory if it does not exist
  QString path = cGlobalSettings::self()->profilePath();
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  // backup the old profile file, if any
  dir.remove ("profiles.backup");
  if (!QFile(path + "/profiles.xml").copy (path + "/profiles.backup")) {
    kDebug() << "Unable to backup profiles.xml." << endl;  // not fatal, may simply not exist
  }

  QFile f (path + "/profiles.xml");
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    kDebug() << "Unable to open profile.xml for writing." << endl;
    return;
  }
  // save the profile file
  QXmlStreamWriter *writer = new QXmlStreamWriter (&f);

  writer->setAutoFormatting (true);  // make the generated XML more readable
  writer->writeStartDocument ();

  writer->writeStartElement ("profiles");
  writer->writeAttribute ("version", "1.0");
 
  map<QString, QString>::iterator it;
  for (it = d->profileNames.begin(); it != d->profileNames.end(); ++it) {
    writer->writeStartElement ("profile");
    writer->writeAttribute ("id", it->first);
    writer->writeAttribute ("name", it->second);
    writer->writeEndElement ();
  }
  
  writer->writeEndElement ();
  writer->writeEndDocument ();

  f.close ();
  delete writer;
}

