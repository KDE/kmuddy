//
// C++ Interface: cProfileSettings
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

