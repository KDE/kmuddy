/***************************************************************************
                          cglobalsettings.cpp  -  global application settings
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

#include "cglobalsettings.h"

#include "cactionmanager.h"
// cansiparser.h needed for color constants
#include "cansiparser.h"
#include "cpluginmanager.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kglobalsettings.h>

#include <QDir>

#include <map>

using namespace std;

struct cGlobalSettings::Private {
  map<QString, bool> boolValues;
  map<QString, int> intValues;
  map<QString, QString> stringValues;
  map<QString, QColor> colorValues;
  map<QString, QFont> fontValues;
  map<QString, bool> defaultBoolValues;
  map<QString, int> defaultIntValues;
  map<QString, QString> defaultStringValues;
  map<QString, QColor> defaultColorValues;
  map<QString, QFont> defaultFontValues;
  bool allowEvents, pendingNotify;
};

cGlobalSettings *cGlobalSettings::_self = 0;

cGlobalSettings *cGlobalSettings::self()
{
  if (_self == 0)
    _self = new cGlobalSettings;
  return _self;
}

cGlobalSettings::cGlobalSettings()
{
  _self = this;
  d = new Private;
  d->allowEvents = true;
  d->pendingNotify = false;
  load ();
}

cGlobalSettings::~cGlobalSettings()
{
  save ();
  delete d;
  _self = 0;
}

void cGlobalSettings::setBool (const QString &name, bool value)
{
  d->boolValues[name] = value;
  notifyChange ();
}

void cGlobalSettings::setInt (const QString &name, int value)
{
  d->intValues[name] = value;
  notifyChange ();
}

void cGlobalSettings::setString (const QString &name, const QString &value)
{
  d->stringValues[name] = value;
  notifyChange ();
}

void cGlobalSettings::setColor (const QString &name, QColor value)
{
  d->colorValues[name] = value;
  notifyChange ();
}

void cGlobalSettings::setFont (const QString &name, QFont value)
{
  d->fontValues[name] = value;
  notifyChange ();
}

bool cGlobalSettings::getBool (const QString &name)
{
  if (d->boolValues.count (name))
    return d->boolValues[name];
  if (d->defaultBoolValues.count (name))
    return d->defaultBoolValues[name];
  return false;
}

int cGlobalSettings::getInt (const QString &name)
{
  if (d->intValues.count (name))
    return d->intValues[name];
  if (d->defaultIntValues.count (name))
    return d->defaultIntValues[name];
  return 0;
}

QString cGlobalSettings::getString (const QString &name)
{
  if (d->stringValues.count (name))
    return d->stringValues[name];
  if (d->defaultStringValues.count (name))
    return d->defaultStringValues[name];
  return QString();
}

QColor cGlobalSettings::getColor (const QString &name)
{
  if (d->colorValues.count (name))
    return d->colorValues[name];
  if (d->defaultColorValues.count (name))
    return d->defaultColorValues[name];
  return Qt::black;
}

QFont cGlobalSettings::getFont (const QString &name)
{
  if (d->fontValues.count (name))
    return d->fontValues[name];
  if (d->defaultFontValues.count (name))
    return d->defaultFontValues[name];
  return KGlobalSettings::fixedFont ();
}

void cGlobalSettings::setDefaultBool (const QString &name, bool val)
{
  d->defaultBoolValues[name] = val;
}

void cGlobalSettings::setDefaultInt (const QString &name, int val)
{
  d->defaultIntValues[name] = val;
}

void cGlobalSettings::setDefaultString (const QString &name, const QString &val)
{
  d->defaultStringValues[name] = val;
}

void cGlobalSettings::setDefaultColor (const QString &name, const QColor &val)
{
  d->defaultColorValues[name] = val;
}

void cGlobalSettings::setDefaultFont (const QString &name, const QFont &val)
{
  d->defaultFontValues[name] = val;
}

// this contains default options for all global settings
void cGlobalSettings::setDefaultOptions ()
{
  // Default profile path
  setDefaultString ("profile-path", QDir::homePath() + "/.kmuddy");

  // Window
  setDefaultBool ("always-tab-bar", true);
  setDefaultBool ("aux-input", false);
  setDefaultBool ("always-notify", true);
  setDefaultBool ("local-notify", true);
  setDefaultBool ("global-notify", true);
  setDefaultBool ("systray-enabled", false);
  setDefaultBool ("passive-popup", false);
  setDefaultString ("auto-connect", QString());

  //Output area
  setDefaultInt ("fg-color", CL_WHITE);
  setDefaultInt ("bg-color", CL_BLACK);
  setDefaultInt ("echo-color", CL_YELLOW | CL_BRIGHT);
  setDefaultInt ("system-color", CL_CYAN | CL_BRIGHT);
  setDefaultBool ("allow-blink", true);
  setDefaultBool ("command-echo", true);
  setDefaultBool ("show-messages", true);
  setDefaultBool ("word-wrap", true);
  setDefaultInt ("indent", 0);
  setDefaultInt ("wrap-pos", 0);
  setDefaultInt ("history-size", 1000);
  setDefaultInt ("force-redraw", 10);

  //Fonts
  QFont f = KGlobalSettings::fixedFont ();
  setDefaultFont ("console-font", f);
  setDefaultFont ("input-font", f);
  setDefaultFont ("multi-line-font", f);

  //Input line
  setDefaultBool ("keep-text", true);
  setDefaultBool ("select-kept", true);
  setDefaultBool ("cursors-browse", true);
  setDefaultBool ("auto-completion", false);
  setDefaultInt ("auto-completion-type", 0);
  setDefaultBool ("telnet-style-paste", false);
  setDefaultBool ("trim-spaces", false);
  setDefaultInt ("input-bg-color", CL_BLACK);
  setDefaultInt ("input-fg-color", CL_YELLOW | CL_BRIGHT);
  setDefaultBool ("swap-enters", false);

  //Colors
  setDefaultColor ("color-0", Qt::black);
  setDefaultColor ("color-1", Qt::darkRed);
  setDefaultColor ("color-2", Qt::darkGreen);
  setDefaultColor ("color-3", Qt::darkYellow);
  setDefaultColor ("color-4", Qt::darkBlue);
  setDefaultColor ("color-5", Qt::darkMagenta);
  setDefaultColor ("color-6", Qt::darkCyan);
  setDefaultColor ("color-7", Qt::lightGray);
  setDefaultColor ("color-8", Qt::darkGray);
  setDefaultColor ("color-9", Qt::red);
  setDefaultColor ("color-10", Qt::green);
  setDefaultColor ("color-11", Qt::yellow);
  setDefaultColor ("color-12", Qt::blue);
  setDefaultColor ("color-13", Qt::magenta);
  setDefaultColor ("color-14", Qt::cyan);
  setDefaultColor ("color-15", Qt::white);

  //Characters
  setDefaultString ("str-separator", ";");
  setDefaultString ("str-speedwalk", ".");
  setDefaultString ("str-macro", "/");
  setDefaultString ("str-multi", "#");
  setDefaultString ("str-focus", ":");
  setDefaultString ("str-noparse", "'");

  //MSP
  setDefaultInt ("smd-path-count", 0);
  setDefaultBool ("msp-allow", true);
  setDefaultBool ("msp-allow-downloads", false);
}

QString cGlobalSettings::profilePath ()
{
  // by default, we store profiles in a .kmuddy directory in the homedir
  return getString ("profile-path");
}

void cGlobalSettings::setProfilePath (const QString &path)
{
  setString ("profile-path", path);
}

void cGlobalSettings::load ()
{
  // loads settings from kmuddyrc
  KSharedConfig::Ptr config = KGlobal::config ();

  //reparse config configuration - needed to make updating of settings
  //via IPC work
  config->reparseConfiguration ();

  setDefaultOptions ();

  if (!config->hasGroup ("Version Info")) {
    disableEvents ();
    // load old-style configuration instead
    loadOldConfig ();
    // old settings won't be deleted, so that previous KMuddy versions work
    enableEvents ();
    return;
  }
  // we have version info - this is a new-style config file
  KConfigGroup g = config->group ("Version Info");
  int version = g.readEntry ("Version", 0);
  if (version < 1) return;  // something is wrong - unable to read settings

  disableEvents ();

  QMap<QString, QString> entries;
  QMap<QString, QString>::iterator it;
  g = config->group ("Boolean values");
  entries = g.entryMap ();
  for (it = entries.begin(); it != entries.end(); ++it)
    setBool (it.key(), g.readEntry (it.key(), false));
  g = config->group ("Numeric values");
  entries = g.entryMap ();
  for (it = entries.begin(); it != entries.end(); ++it)
    setInt (it.key(), g.readEntry (it.key(), 0));
  g = config->group ("String values");
  entries = g.entryMap ();
  for (it = entries.begin(); it != entries.end(); ++it)
    setString (it.key(), g.readEntry (it.key(), QString()));
  g = config->group ("Color values");
  entries = g.entryMap ();
  for (it = entries.begin(); it != entries.end(); ++it)
    setColor (it.key(), g.readEntry (it.key(), getColor(it.key())));
  QFont f = KGlobalSettings::fixedFont ();
  g = config->group ("Font values");
  entries = g.entryMap ();
  for (it = entries.begin(); it != entries.end(); ++it)
    setFont (it.key(), g.readEntry (it.key(), f));

  d->pendingNotify = false;  // we do not need to notify ...
  enableEvents ();

  // Plugins
  g = config->group ("Plugins");
  QStringList nl;
  nl = g.readEntry ("Do not load", nl);
  cPluginManager::self()->setNoAutoLoadList (nl);
}

// TODO: what to do with this ? KDE4 apps can't read old config, can they ?
void cGlobalSettings::loadOldConfig ()
{
  //Loads settings from kmuddyrc. Defaults are used if necessary.
  KSharedConfig::Ptr config = KGlobal::config ();

  //reparse config configuration - needed to make updating of settings
  //via IPC work
  config->reparseConfiguration ();

  //Window
  KConfigGroup g = config->group ("Window");
  setBool ("always-tab-bar", g.readEntry ("Always show tab bar", true));
  setBool ("aux-input", g.readEntry ("Auxiliary input", false));
  setBool ("global-notify", g.readEntry ("Global notification", true));
  setBool ("local-notify", g.readEntry ("Local notification", true));
  setBool ("always-notify", g.readEntry ("Always notify", true));
  setString ("auto-connect",  g.readEntry ("Auto connect", QString()));
  //all these are kept in Window group for historical reasons
  // (it used to be there in 0.3.1 and before :))
  setInt ("bg-color", g.readEntry ("Background color index", CL_BLACK));
  setInt ("fg-color", g.readEntry ("Foreground color index", CL_WHITE));
  setInt ("echo-color", g.readEntry ("Echo color index", CL_YELLOW | CL_BRIGHT));
  setInt ("system-color", g.readEntry ("System color index", CL_CYAN | CL_BRIGHT));
  setBool ("command-echo", g.readEntry ("Command echo", true));
  setBool ("lpmud-style", g.readEntry("LPMud style", false));
  setBool ("show-messages", g.readEntry ("Display messages", true));
  setBool ("allow-blink", g.readEntry ("Allow blinking", true));
  setBool ("word-wrap", g.readEntry ("Word wrapping", true));
  setInt ("indent", g.readEntry ("Indentation", 0));
  setInt ("wrap-pos", g.readEntry ("Wrap position", 0));
  setInt ("history-size", g.readEntry ("History size", 1000));
  setInt ("force-redraw", g.readEntry ("Force redraw", 10));
  
  //Fonts
  g = config->group ("Fonts");
  QFont f = KGlobalSettings::fixedFont ();
  setFont ("console-font", g.readEntry ("Console font", f));
  setFont ("input-font", g.readEntry ("Input line font", f));
  setFont ("multi-line-font", g.readEntry ("Multi-line input font", f));

  //Input line
  g = config->group ("Input line");
  setBool ("keep-text", g.readEntry ("Keep text", true));
  setBool ("select-kept", g.readEntry ("Select kept text", true));
  setBool ("cursors-browse", g.readEntry ("Cursor keys browse history", true));
  setBool ("auto-completion", g.readEntry ("Use auto-completion", false));
  setInt ("auto-completion-type", g.readEntry ("Auto-completion type", 0));
  setBool ("telnet-style-paste", g.readEntry ("Telnet-style paste", false));
  setBool ("trim-spaces", g.readEntry ("Trim spaces", false));
  setInt ("input-bg-color", g.readEntry ("Background color", CL_BLACK));
  setInt ("input-fg-color", g.readEntry ("Text color", CL_YELLOW | CL_BRIGHT));
  setBool ("swap-enters", g.readEntry ("Swap ENTERs", false));

  g = config->group ("Colors");
  setColor ("color-0", g.readEntry ("Black", (QColor) Qt::black));
  setColor ("color-1", g.readEntry ("Red", (QColor) Qt::darkRed));
  setColor ("color-2", g.readEntry ("Green", (QColor) Qt::darkGreen));
  setColor ("color-3", g.readEntry ("Yellow", (QColor) Qt::darkYellow));
  setColor ("color-4", g.readEntry ("Blue", (QColor) Qt::darkBlue));
  setColor ("color-5", g.readEntry ("Magenta", (QColor) Qt::darkMagenta));
  setColor ("color-6", g.readEntry ("Cyan", (QColor) Qt::darkCyan));
  setColor ("color-7", g.readEntry ("Gray", (QColor) Qt::lightGray));
  setColor ("color-8", g.readEntry ("Dark gray", (QColor) Qt::darkGray));
  setColor ("color-9", g.readEntry ("Bright red", (QColor) Qt::red));
  setColor ("color-10", g.readEntry ("Bright green", (QColor) Qt::green));
  setColor ("color-11", g.readEntry ("Bright yellow", (QColor) Qt::yellow));
  setColor ("color-12", g.readEntry ("Bright blue", (QColor) Qt::blue));
  setColor ("color-13", g.readEntry ("Bright magenta", (QColor) Qt::magenta));
  setColor ("color-14", g.readEntry ("Bright cyan", (QColor) Qt::cyan));
  setColor ("color-15", g.readEntry ("White", (QColor) Qt::white));

  //Characters (these are no longer characters, name wasn't changed
  //to make upgrading easier)
  g = config->group ("Characters");
  setString ("str-separator", g.readEntry ("Command separator", ";"));
  setString ("str-speedwalk", g.readEntry ("Speed walking", "."));
  setString ("str-macro", g.readEntry ("Macro character", "/"));
  setString ("str-multi", g.readEntry ("Multi-command", "#"));
  setString ("str-focus", g.readEntry ("Focus command", ":"));
  setString ("str-noparse", g.readEntry ("No-parse prefix", "'"));
  setBool ("empty-walk", g.readEntry ("Empty walkstring", false));
  setBool ("expand-backslashes", g.readEntry ("Expand backslashes", true));

  //MSP
  g = config->group ("Sound Protocol");
  setBool ("msp-allow", g.readEntry ("Allow MSP", true));
  setBool ("msp-allow-downloads", g.readEntry ("Allow downloads", false));
  int count = g.readEntry ("Path count", -1);
  if (count == -1)
  {
    // count = 1;
    // sounddirs << "/usr/share/sounds";
    setInt ("snd-path-count", 0);
  }
  else
  {
    setInt ("snd-path-count", count);
    for (int i = 1; i <= count; i++)
      setString ("snd-path-"+QString::number(i), g.readEntry (QString("Path ") + QString::number(i),
            QString()));
  }
}

void cGlobalSettings::save ()
{
  //Saves settings back to kmuddyrc.
  KSharedConfig::Ptr config = KGlobal::config ();

  config->deleteGroup ("Boolean values");
  KConfigGroup g = config->group ("Boolean values");
  map<QString, bool>::iterator it1;
  for (it1 = d->boolValues.begin(); it1 != d->boolValues.end(); ++it1)
    g.writeEntry (it1->first, it1->second);
  config->deleteGroup ("Numeric values");
  g = config->group ("Numeric values");
  map<QString, int>::iterator it2;
  for (it2 = d->intValues.begin(); it2 != d->intValues.end(); ++it2)
    g.writeEntry (it2->first, it2->second);
  config->deleteGroup ("String values");
  g = config->group ("String values");
  map<QString, QString>::iterator it3;
  for (it3 = d->stringValues.begin(); it3 != d->stringValues.end(); ++it3)
    g.writeEntry (it3->first, it3->second);
  config->deleteGroup ("Color values");
  g = config->group ("Color values");
  map<QString, QColor>::iterator it4;
  for (it4 = d->colorValues.begin(); it4 != d->colorValues.end(); ++it4)
    g.writeEntry (it4->first, it4->second);
  g = config->group ("Font values");
  map<QString, QFont>::iterator it5;
  for (it5 = d->fontValues.begin(); it5 != d->fontValues.end(); ++it5)
    g.writeEntry (it5->first, it5->second);

  g = config->group ("Version Info");
  g.writeEntry ("Version", 1);

  //Shortcuts
  KActionCollection *acol = cActionManager::self()->getACol ();
  acol->setConfigGroup ("Shortcuts");
  acol->writeSettings ();

  //Plugins
  g = config->group ("Plugins");
  g.writeEntry ("Do not load", cPluginManager::self()->noAutoLoadList());

  config->sync ();
}

void cGlobalSettings::disableEvents ()
{
  d->allowEvents = false;
}

void cGlobalSettings::enableEvents ()
{
  d->allowEvents = true;
  if (d->pendingNotify)
    notifyChange ();
}

void cGlobalSettings::notifyChange ()
{
  if (!d->allowEvents) {
    d->pendingNotify = true;
  } else {
    d->pendingNotify = false;
    cActionManager::self()->invokeEvent ("global-settings-changed", 0);
    save ();
  }
}

