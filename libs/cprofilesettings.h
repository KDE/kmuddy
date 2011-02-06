//
// C++ Interface: cProfileSettings
//
// Description: profile settings
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CPROFILESETTINGS_H
#define CPROFILESETTINGS_H

#include <QString>
#include <kmuddy_export.h>

/** cProfileSettings - manages the profile settings of one profile */

class KMUDDY_EXPORT cProfileSettings {
 public:
  cProfileSettings (QString profileId);
  ~cProfileSettings ();

  // only bool, int and string for now - more may get added later
  void setBool (const QString &name, bool value);
  void setInt (const QString &name, int value);
  void setString (const QString &name, const QString &value);

  bool getBool (const QString &name) const;
  int getInt (const QString &name) const;
  QString getString (const QString &name) const;

  static void setDefaultBool (const QString &name, bool value);
  static void setDefaultInt (const QString &name, int value);
  static void setDefaultString (const QString &name, const QString &value);

  /** save the settings */
  void save ();
 private:
  /** load the settings */
  void load ();
  /** Fill in the default values for main preferences. Plugins can add more in the profile-init or profile-load handler. */
  static void fillDefaultValues ();
  static void initDefaultStorage ();

  struct Private;
  Private *d;
  static Private *defd;
};

#endif   // CPROFILESETTINGS_H

