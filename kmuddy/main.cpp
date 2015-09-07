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

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>

#include <signal.h>

#include "kmuddy-version.h"
#include "kmuddy.h"
#include "cglobalsettings.h"

#define DESCRIPTION ki18n("KMuddy is a MUD (Multi-User Dungeon) client for KDE with a variety of features.")

int main(int argc, char *argv[])
{
  KAboutData aboutData ("kmuddy", "", ki18n("KMuddy"),
    VERSION, DESCRIPTION, KAboutData::License_GPL,
    ki18n("(c) 2002-2009, Tomas Mecir"), KLocalizedString(), "http://www.kmuddy.com/", "kmuddy@kmuddy.com");
  aboutData.addAuthor(ki18n ("Tomas Mecir"),
      ki18n("Main developer and maintainer"), "kmuddy@kmuddy.com");
  aboutData.addCredit(ki18n("Alex Bache"),
      ki18n("Many improvements in external scripting, internal script parser, output view and aliases/triggers, bugfixes"), "alex.bache@ntlworld.com");
  aboutData.addCredit(ki18n("Alsherok staff"),
      ki18n("for releasing AFKMud codebase, used to test MCCP/MSP"));
  aboutData.addCredit(ki18n("Andrea Cascio"),
      ki18n("SuSE RPMs, speed-walking improvements"), "hacksaw@triangolo.it");
  aboutData.addCredit(ki18n("Orion Elder"),
      ki18n("KMuddy forum, many ideas"), "orion@mudplanet.org");
  aboutData.addCredit(ki18n("Scott Monachello"),
      ki18n("Improvements in scripting, bugfixes"), "smonachello@yahoo.com");
  aboutData.addCredit(ki18n("Tyler Montbriand"),
      ki18n("Help with SDL support"), "tsm@accesscomm.ca");
  aboutData.addCredit(ki18n("Marco Mastroddi"),
      ki18n("Tick timers"), "marco.mastroddi@libero.it");
  aboutData.addCredit (ki18n("Henrikki Almusa"),
      ki18n("Commands to enable/disable groups"), "hena@iki.fi");
  aboutData.addCredit(ki18n("Drossos Stamboulakis"),
      ki18n("Selection improvements, full screen mode, cross-tab commands"), "adsluard@tpg.com.au");
  aboutData.addCredit (ki18n("Antonio J. Soler Sanchez"),
      ki18n("Spanish translation"), "redtony@telefonica.net");
  aboutData.addCredit(ki18n("lontau"),
      ki18n("Code to access KMuddy variables in Ruby"));
  aboutData.addCredit (ki18n("Magnus Lundborg"), ki18n("Tab-expansion improvements"), "lundborg.magnus@gmail.com");
  aboutData.addCredit(ki18n("Vladimir Lazarenko"),
      ki18n("Many improvements. Hosting KMuddy CVS."), "vlad@lazarenko.net");
  aboutData.addCredit(ki18n("Stuart Henshall"),
      ki18n("Speed improvements."), "shenshall@blueyonder.co.uk");
  aboutData.addCredit(ki18n("Vadim Peretokin"),
      ki18n("Many improvements, website"), "vadimuses@gmail.com");
  aboutData.addCredit(ki18n("Heiko Koehn"),
      ki18n("Scripting improvements"), "koehnheiko@googlemail.com");
  aboutData.addCredit(ki18n("Robert Marmorstein"),
      ki18n("Systray icon support"), "robertandbeth@gmail.com");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineOptions options;   // no options at the moment
  KCmdLineArgs::addCmdLineOptions( options );

  //alarm signal is sometimes causing KMuddy to terminate incorrectly
  //when switching tabs using Alt+number - ignoring the signal, hoping
  //for the best...
  signal (SIGALRM, SIG_IGN);
  
  KApplication a;
  a.setApplicationName ("KMuddy");
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

