/***************************************************************************
                          cglobalsettings.h  -  global application settings
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : St Aug 14 2002
    copyright            : (C) 2002-2007 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CGLOBALSETTINGS_H
#define CGLOBALSETTINGS_H

#include <qcolor.h>
#include <qfont.h>
#include <qobject.h>
#include <kmuddy_export.h>

class dlgAppSettings;
class KActionCollection;
/**
Class that stores and manages global application configuration.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cGlobalSettings {
public:
  static cGlobalSettings *self();
  ~cGlobalSettings();

  void setBool (const QString &name, bool value);
  void setInt (const QString &name, int value);
  void setString (const QString &name, const QString &value);
  void setColor (const QString &name, QColor value);
  void setFont (const QString &name, QFont value);

  bool getBool (const QString &name);
  int getInt (const QString &name);
  QString getString (const QString &name);
  QColor getColor (const QString &name);
  QFont getFont (const QString &name);

  void setDefaultBool (const QString &name, bool val);
  void setDefaultInt (const QString &name, int val);
  void setDefaultString (const QString &name, const QString &val);
  void setDefaultColor (const QString &name, const QColor &val);
  void setDefaultFont (const QString &name, const QFont &val);

  /** Returns profile path. Convenience wrapper around getString. */
  QString profilePath ();
  /** Set profile path. Convenience wrapper around setString.  */
  void setProfilePath (const QString &path);

  /** Notify everyone that settings have changed. It should rarely be necessary to call this. */
  void notifyChange ();

  /** disable events upon changing settings */
  void disableEvents ();
  /** enable events upon changing settings */
  void enableEvents ();

private:
  cGlobalSettings();
  static cGlobalSettings *_self;

  /** Sets default global configuration options. */
  void setDefaultOptions ();

  /** load settings from kmuddyrc */
  void load ();
  /** load old-style settings from kmuddyrc */
  void loadOldConfig ();
  /** save settings there */
  void save ();

  struct Private;
  Private *d;

  /** the dialog itself */
  dlgAppSettings *sdlg;
};


#endif
