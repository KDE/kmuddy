/***************************************************************************
                          cmsp.h  -  MUD Sound Protocol
                             -------------------
    begin                : Ne mar 16 2003
    copyright            : (C) 2003 by Tomas Mecir
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

#ifndef CMSP_H
#define CMSP_H

#include <qstring.h>
#include <qstringlist.h>
#include <string>

#include <cactionbase.h>
#include <kmuddy_export.h>

class cDownloader;
class cSoundPlayer;

/**
MUD Sound Protocol implementation.

Implementation notes (those that are not specified in the protocol):
- sound trigger is only caught at beginning of line (whitespaces can precede
  it, but nothing else)
- anything following the sound trigger till end of line is discarded
- unterminated sound triggers are discarded as well
- unknown trigger parameters are ignored (to ensure compatibility with
  future version of protocol, if any)
- only one download can be active at any time
- official protocol specification is poor

  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cMSP : public cActionBase {
public: 
  cMSP (int sess);
  ~cMSP () override;
  void reset (const QString &serverName);
  /** enable MSP support */
  void enableMSP ();
  /** disable MSP support */
  void disableMSP ();
  /** when MSP is not allowed, sound triggers are received, but ignored */
  void setMSPAllowed (bool allow) { mspallowed = allow; };
  /** enable/disable downloading stuff from the server */
  void setDownloadAllowed (bool allow) { dloadallowed = allow; };
  /** Parse server output, looking for MSP tags. If some tag is found, parse
  it and act as required. The argument may be modified if needed (the MSP
  tag needs to be removed to prevent it from being displayed) */
  std::string parseServerOutput (const std::string &output);

  void setGlobalPaths (const QStringList &paths) { globaldirs = paths; };
  
  /** process a request - called after successful parsing, may also be called separately */
  void processRequest (bool isSOUND, QString fName, int volume, int repeats, int priority,
      QString type, QString url);
protected:
  /** get next token */
  QString nextToken (QString &from);
  /** received corrupted trigger! */
  void corruptedTrigger (const QString &reason);

  /** parse !!SOUND / !!MUSIC trigger */
  void parseTrigger (const QString &seq, bool isSOUND);
  /** sound off */
  void soundOff ();
  /** music off */
  void musicOff ();
  /** play sound file */
  void playSound (const QString &path, int volume, int repeats, int priority);
  /** play music file */
  void playMusic (const QString &path, int volume, int repeats, bool continueIfRerequested);

  /** download file, as specified in the dl_* attributes, then play it */
  void downloadFile ();
  /** called by cDownloader */
  void downloadCompleted ();
  /** called by cDownloader */
  void downloadFailed (const QString &reason);
  
  /** get file name, if any, according to MSP rules (random choice if
  multiple matches) */
  QString getFileName (QString where, QString what);
  /** find a sound file in a list of paths specified by the user, returning
  a path to the file if it's found, QString() if it isn't */
  QString findFile (const QString &path);

  bool mspenabled, mspallowed, dloadallowed;

  /** global directories where sounds can be stored */
  QStringList globaldirs;
  
  /** default URL */
  QString defaultURL;

  /** local directory, where downloaded files will be stored */
  QString localdir;
  
  /** current state of the MSP parser (finite state machine) */
  int state;
  
  /** sound players */
  cSoundPlayer *soundPlayer, *midiPlayer;
  
  /** true = in SOUND, false = in MUSIC; only valid if applicable*/
  bool inSOUND;
  std::string cachedString, triggerContents;

  /** download information */
  cDownloader *downloader;
  QString dl_fName, dl_type, dl_url;
  int dl_volume, dl_repeats, dl_priority;
  bool dl_issound;

  friend class cDownloader;
};

#endif //CMSP_H
