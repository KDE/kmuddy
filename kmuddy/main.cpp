/***************************************************************************
                          main.cpp  -  main file for KMuddy
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Fri Jun 14 12:37:51 CEST 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#include <KAboutData>
#include <klocale.h>
#include <QApplication>
#include <Kdelibs4ConfigMigrator>

#include <signal.h>

#include "kmuddy-version.h"
#include "kmuddy.h"
#include "cglobalsettings.h"

#define DESCRIPTION i18n("KMuddy is a MUD (Multi-User Dungeon) client by KDE with a variety of features.")

int main(int argc, char *argv[])
{
  QApplication a (argc, argv);
  KLocalizedString::setApplicationDomain("kmuddy");
  QApplication::setApplicationDisplayName(i18n("KMuddy"));

  KAboutData aboutData ("kmuddy", i18n("KMuddy"),
    VERSION, DESCRIPTION, KAboutLicense::GPL,
    i18n("(c) 2002-2018, Tomas Mecir"), QString(), "http://www.kmuddy.com/", "mecirt@gmail.com");
  aboutData.addAuthor(i18n ("Tomas Mecir"),
      i18n("Main developer and maintainer"), "mecirt@gmail.com");
  aboutData.addCredit(i18n("Alex Bache"),
      i18n("Many improvements in external scripting, internal script parser, output view and aliases/triggers, bugfixes"), "alex.bache@ntlworld.com");
  aboutData.addCredit(i18n("Alsherok staff"),
      i18n("for releasing AFKMud codebase, used to test MCCP/MSP"));
  aboutData.addCredit(i18n("Andrea Cascio"),
      i18n("SuSE RPMs, speed-walking improvements"), "hacksaw@triangolo.it");
  aboutData.addCredit(i18n("Orion Elder"),
      i18n("KMuddy forum, many ideas"), "orion@mudplanet.org");
  aboutData.addCredit(i18n("Scott Monachello"),
      i18n("Improvements in scripting, bugfixes"), "smonachello@yahoo.com");
  aboutData.addCredit(i18n("Tyler Montbriand"),
      i18n("Help with SDL support"), "tsm@accesscomm.ca");
  aboutData.addCredit(i18n("Marco Mastroddi"),
      i18n("Tick timers"), "marco.mastroddi@libero.it");
  aboutData.addCredit (i18n("Henrikki Almusa"),
      i18n("Commands to enable/disable groups"), "hena@iki.fi");
  aboutData.addCredit(i18n("Drossos Stamboulakis"),
      i18n("Selection improvements, full screen mode, cross-tab commands"), "adsluard@tpg.com.au");
  aboutData.addCredit (i18n("Antonio J. Soler Sanchez"),
      i18n("Spanish translation"), "redtony@telefonica.net");
  aboutData.addCredit(i18n("lontau"),
      i18n("Code to access KMuddy variables in Ruby"));
  aboutData.addCredit (i18n("Magnus Lundborg"), i18n("Tab-expansion improvements"), "lundborg.magnus@gmail.com");
  aboutData.addCredit(i18n("Vladimir Lazarenko"),
      i18n("Many improvements. Hosting KMuddy CVS."), "vlad@lazarenko.net");
  aboutData.addCredit(i18n("Stuart Henshall"),
      i18n("Speed improvements."), "shenshall@blueyonder.co.uk");
  aboutData.addCredit(i18n("Vadim Peretokin"),
      i18n("Many improvements, website"), "vadimuses@gmail.com");
  aboutData.addCredit(i18n("Heiko Koehn"),
      i18n("Scripting improvements"), "koehnheiko@googlemail.com");
  aboutData.addCredit(i18n("Robert Marmorstein"),
      i18n("Systray icon support"), "robertandbeth@gmail.com");
  KAboutData::setApplicationData (aboutData);
  QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kmuddy")));

  //alarm signal is sometimes causing KMuddy to terminate incorrectly
  //when switching tabs using Alt+number - ignoring the signal, hoping
  //for the best...
  signal (SIGALRM, SIG_IGN);

  // migrate settings from kde4
  Kdelibs4ConfigMigrator migrator(QStringLiteral("kmuddy")); // the same name defined in the aboutData
  // all the config files of your application
  migrator.setConfigFiles(QStringList() << QStringLiteral("kmuddyrc"));
  // list of KXMLGUI files used by your application
  migrator.setUiFiles(QStringList() << QStringLiteral("kmuddymapperpart.rc"));
  migrator.migrate();

  KMuddy *kmuddy = 0;

/*
  if (a.isSessionRestored ())  //session management
    RESTORE(KMuddy); //DOES NOT WORK - we need kmuddy object in the program
        //and I couldn't find a way to get to it after RESTORE (mainWidget
        //didn't work (?))
  else
  {
*/
    //normal startup
    kmuddy = KMuddy::self();
/*
  }
*/
  //event handler for macro keys
  a.installEventFilter (kmuddy);
  
  kmuddy->show();

  int retCode = a.exec ();

  //Looks like KApplication deletes the widget using deleteLater, resulting
  //in double deleting and problems. We therefore keep the object alive,
  //hoping for the best

  //delete kmuddy;
  
  return retCode;
}

