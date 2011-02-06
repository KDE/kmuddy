//
// C++ Interface: cProfileManager
//
// Description: profile manager
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CPROFILEMANAGER_H
#define CPROFILEMANAGER_H

#include <QStringList>

#include <kmuddy_export.h>

class cProfileSettings;
class QAbstractTableModel;

/** cProfileManager - manages the profiles */

class KMUDDY_EXPORT cProfileManager {
 public:
  static cProfileManager *self ();
  ~cProfileManager ();

  /** Returns the profile list. */
  QStringList profileList () const;

  // a data model for the connect dialog
  QAbstractTableModel *model () const;

  /** Path to profile files - takes internal profile name as param. */
  QString profilePath (const QString &profileName) const;

  bool profileExists (const QString &name);

  /** Assign a session to a particular profile. */
  void assignSession (int sess, const QString &profileName);

  /** Remove the session assignation */
  void unassignSession (int sess);

  /** Is the session assigned to a profile ? If the session exists and isn't
   assigned, it s quick connection */
  bool sessionAssigned (int sess);

  /** Does this profile have any session assigned ? */
  bool hasSessionAssigned (const QString &profile);

  /** Path to profile files belonging to a session. */
  QString profilePath (int sess);

  /** Profile name associated to this session. */
  QString profileName (int sess);

  /** Return visible profile name. */
  QString visibleProfileName (QString name);

  /** Return profile settings. */
  cProfileSettings *settings (const QString &profileName);

  /** Return profile settings for a session. */
  cProfileSettings *settings (int sess);

  /** Create new profile. */
  QString newProfile (const QString &name);

  /** Rename profile's visible name. The internal name stays the same. */
  bool renameProfile (const QString &name, const QString &newName);

  /** Delete profile. */
  bool deleteProfile (const QString &name, bool deleteFiles = false);

  /** Duplicate profile. */
  bool duplicateProfile (const QString &name, const QString &newName);

  /** Base profile info has been changed, react accordingly. */
  void profileInfoChanged (const QString &name);

 private:
  cProfileManager ();
  static cProfileManager *_self;

  /** Load the profile list. */
  void load ();
  /** Save the profile list. */
  void save ();
  /** Initialize the object after loading. */
  void init ();

  struct Private;
  Private *d;
};

#endif   // CPROFILEMANAGER_H

