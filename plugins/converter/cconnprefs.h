/***************************************************************************
                          cconnprefs.h  -  connection preferences, aliases, ...
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

#ifndef CCONNPREFS_H
#define CCONNPREFS_H

/*
 TODO: currently passwords are saved WITHOUT ANY ENCRYPTION!!!
   This must be changed.
   IMPORTANT: it must be possible to decode the password, as it's sent
   to the MUD unencrypted => MD5 and friends can NOT be used.
*/

#include <qstring.h>
#include <qstringlist.h>

/**
Connection preferences - contains all settings,
aliases, triggers,
auto-mapper information and so on (most of them are included
in separate objects, but the class is still pretty big)

  *@author Tomas Mecir
  */

class cConnPrefs {
private:
//directory that contains preferences
  QString directory;

//Part 1 - preferences
  //basic info
  QString _name, _server;
  QString _login, _password;
  int _port;
  QStringList _connstr;
  
  //type of connection
   //NOTHING HERE
  //connection parameters
  bool _ansicolors;
  bool _limittriggers, _limitrepeater;
  bool _startupneg;
  bool _promptlabel;
  bool _lpmudstyle;
  bool _statusprompt;
  bool _consoleprompt;
  bool _autoadvtranscript;
    
  //auto-mapper settings
   //NOTHING HERE YET
   
  //default movement commands, quit command, ...
  enum Direction {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest, Up, Down};
  QString _cmd[10];
  QString _cmdquit;

  //default directories
  QString scriptdir;
  QString defworkdir;
  QString transcriptdir;

  //MUD Sound Prococol settings
  bool usemsp;
  QStringList sounddirs;
  /** alwaysmsp - use MSP even if not negotiated
  midlinemsp - allow mid-line MSP triggers */
  bool alwaysmsp, midlinemsp;

  //MXP settings
  int usemxp;
  QString varprefix;
  
public:
  cConnPrefs (QString directory);
  ~cConnPrefs ();

//load/save settings
  void load ();
  
//functions that get/set variables:

//Part 1 - preferences
  //basic info
  void setName (const QString &s) { _name = s; };
  const QString name () { return _name; };
  void setServer (const QString &s) { _server = s; };
  const QString server () { return _server; };
  void setLogin (const QString &s) { _login = s; };
  const QString login () { return _login; };
  void setPassword (const QString &s) { _password = s; };
  const QString password () { return _password; };
  void setPort (int port);
  int port () { return _port; };
  QStringList connStr () { return _connstr; };
  void setConnStr (QStringList cs) { _connstr = cs; };
  
  //type of connection
   //NOTHING HERE
   
  //connection parameters
  bool ansiColors () { return _ansicolors; };
  void setAnsiColors (bool val);
  bool limitTriggers () { return _limittriggers; };
  bool limitRepeater () { return _limitrepeater; };
  void setLimitTriggers (bool val) { _limittriggers = val; };
  void setLimitRepeater (bool val) { _limitrepeater = val; };
  bool negotiateOnStartup () { return _startupneg; };
  void setNegotiateOnStartup (bool val);
  bool promptLabel () { return _promptlabel; };
  void setPromptLabel (bool val);
  bool LPMudStyle () { return _lpmudstyle; };
  void setLPMudStyle (bool val);
  bool statusPrompt() { return _statusprompt; };
  void setStatusPrompt (bool val);
  bool consolePrompt() { return _consoleprompt; };
  void setConsolePrompt (bool val) { _consoleprompt = val; };
  bool autoAdvTranscript() { return _autoadvtranscript; };
  void setAutoAdvTranscript (bool val) { _autoadvtranscript = val; };
  
  //auto-mapper settings
   //NOTHING HERE YET
   
  //default movement commands, quit command, ...
  QString cmd (int which) { return ((which >= 0) && (which <= 9)) ? _cmd[which] : (QString)""; };
  void setCmd (int which, QString val);
  QString quit () { return _cmdquit; };
  void setQuit (QString val);
  
  //script settings
  QString scriptDir () { return scriptdir; };
  void setScriptDir (const QString &s) { scriptdir = s; };
  QString workDir () { return defworkdir; };
  void setWorkDir (const QString &s) { defworkdir = s; };
  QString transcriptDir () { return transcriptdir; };
  void setTranscriptDir (const QString &s) { transcriptdir = s; };
         
  //MSP settings
  QStringList soundDirs () { return sounddirs; };
  void setSoundDirs (QStringList dirs) { sounddirs = dirs; };
  bool useMSP () { return usemsp; };
  void setUseMSP (bool val) { usemsp = val; };
  bool alwaysMSP () { return alwaysmsp; };
  void setAlwaysMSP (bool val) { alwaysmsp = val; };
  bool midlineMSP () { return midlinemsp; };
  void setMidlineMSP (bool val) { midlinemsp = val; };

  //MXP settings
  int useMXP () { return usemxp; };
  void setUseMXP (int val);
  QString varPrefix () { return varprefix; };
  void setVarPrefix (const QString &prefix) { varprefix = prefix; };
};

#endif
