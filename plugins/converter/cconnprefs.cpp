/***************************************************************************
                    cconnprefs.cpp  -  connection preferences, aliases, ...
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Jul 22 2002
    copyright            : (C) 2002-2005 by Tomas Mecir
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

#include "cconnprefs.h"

#include <qdir.h>
#include <kconfig.h>
#include <kconfiggroup.h>

cConnPrefs::cConnPrefs (QString dir)
{
  directory = dir;
}

cConnPrefs::~cConnPrefs()
{
}

void cConnPrefs::load ()
{
  //prepare configfile
  KConfig *config = new KConfig (directory + "/preferences");

  //load basic info
  KConfigGroup g = config->group ("General");
  setServer (g.readEntry ("Server", ""));
  setPort (g.readEntry ("Port", 0));
  setLogin (g.readEntry ("Login", ""));
  setPassword (g.readEntry ("Password", ""));

  //login sequence
  g = config->group ("Login sequence");
  _connstr.clear ();
  int lsc = g.readEntry ("Count", 0);
  for (int i = 1; i <= lsc; i++)
  {
    QString s = g.readEntry ("Line " + QString::number(i), QString());
    _connstr.append (s);    
  }
  if (lsc == 0) //default login sequence
  {
    _connstr.append ("$name");
    _connstr.append ("$password");
  }
  
  //connection parameters
  g = config->group ("Connection");
  setAnsiColors (g.readEntry ("ANSI Colors", true));
  setLimitTriggers (g.readEntry ("Limit triggers", true));
  setLimitRepeater (g.readEntry ("Limit repeater", true));
  setNegotiateOnStartup (g.readEntry ("Negotiate on startup", true));
  setPromptLabel (g.readEntry ("Prompt label", false));
  setLPMudStyle (g.readEntry ("LPMud style", false));
  setStatusPrompt (g.readEntry ("Status prompt", false));
  setConsolePrompt (g.readEntry ("Console prompt", true));
  setAutoAdvTranscript (g.readEntry ("Auto logging", false));

  //commands
  g = config->group ("Commands");
  setCmd (North, g.readEntry ("North", "n"));
  setCmd (NorthEast, g.readEntry ("NorthEast", "ne"));
  setCmd (East, g.readEntry ("East", "e"));
  setCmd (SouthEast, g.readEntry ("SouthEast", "se"));
  setCmd (South, g.readEntry ("South", "s"));
  setCmd (SouthWest, g.readEntry ("SouthWest", "sw"));
  setCmd (West, g.readEntry ("West", "w"));
  setCmd (NorthWest, g.readEntry ("NorthWest", "nw"));
  setCmd (Up, g.readEntry ("Up", "u"));
  setCmd (Down, g.readEntry ("Down", "d"));
  setQuit (g.readEntry ("Quit", "quit"));

  //scripts
  g = config->group ("Scripts");
  setScriptDir (g.readEntry ("Script directory", QDir::homePath()));
  setWorkDir (g.readEntry ("Working directory", QDir::homePath()));

  //directories (the above two should go here, but we don't want to corrupt
  // existing settings)
  g = config->group ("Directories");
  setTranscriptDir (g.readEntry ("Transcript directory", QDir::homePath()));
  
  //MSP
  g = config->group ("Sound Protocol");
  sounddirs.clear ();
  setUseMSP (g.readEntry ("Use MSP", true));
  setAlwaysMSP (g.readEntry ("Always MSP", false));
  setMidlineMSP (g.readEntry ("Midline MSP", false));
  int count = g.readEntry ("Path count", -1);
  if (count == -1)
  {
    //default sound dirs
    count = 1;
    sounddirs << QDir::homePath() + "/sounds";
  }
  else
    for (int i = 1; i <= count; i++)
      sounddirs << g.readEntry (QString("Path ") + QString::number(i),
            QString());

  //MXP
  g = config->group ("MUD Extension Protocol");
  //how to use MXP? By default, we auto-detect it (i.e., we parse for MXP stuff, but we
  //remain in LOCKED mode by default, until changed by server)
  setUseMXP (g.readEntry ("Use MXP", 3));
  varprefix = g.readEntry ("Variable prefix", "");

  delete config;

}

//some functions that get/set preferences; most of them are in header file

void cConnPrefs::setPort (int port)
{
  if ((port > 0) && (port <= 65535))
    _port = port;
}

void cConnPrefs::setAnsiColors (bool val)
{
  _ansicolors = val;
}

void cConnPrefs::setCmd (int which, QString val)
{
  if ((which >= 0) && (which <= 9))
    _cmd[which] = val;
}

void cConnPrefs::setQuit (QString val)
{
  _cmdquit = val;
}

void cConnPrefs::setPromptLabel (bool val) 
{ 
  _promptlabel = val; 
  
}

void cConnPrefs::setNegotiateOnStartup (bool val)
{
  _startupneg = val;
}

void cConnPrefs::setLPMudStyle(bool val)
{
  _lpmudstyle = val;
}

void cConnPrefs::setStatusPrompt(bool val)
{
  _statusprompt = val;
}

void cConnPrefs::setUseMXP (int val)
{
  usemxp = val;
}


