/***************************************************************************
                          csoundplayer.h  -  sound player
                             -------------------
    begin                : So apr 19 2003
    copyright            : (C) 2003-2008 by Tomas Mecir
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

#ifndef CSOUNDPLAYER_H
#define CSOUNDPLAYER_H

#include <cactionbase.h>
#include <kmuddy_export.h>

#include <qobject.h>
#include <Phonon/Global>

/**
Plays sound via Phonon.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cSoundPlayer : public QObject, public cActionBase {
   Q_OBJECT
public: 
  cSoundPlayer (bool isWAVE = true);
  // constructor must always be visible as well, else compilation fails
  ~cSoundPlayer ();
  /** are we currently playing? */
  bool isPlaying ();
  /** priority of currently played sound */
  int curPriority ();
  /** set new priority */
  void setPriority (int val);
  /** remaining # of repeats, INCLUDING current one */
  int remainingRepeats ();
  /** set repeats count; will NOT affect playing sound */
  void setRepeatsCount (int val);
  /** played file name */
  QString fileName ();
  /** set new file name */
  void setFileName (const QString &name);
  /** current volume */
  int curVolume ();
  /** set new volume */
  void setVolume (int val);
  /** play sound, stopping previous one, if any */
  void play ();
  /** stop playing */
  void stop ();
  /** will force updating of some parameters for currently played sound;
  this includes repeat count, priority and volume */
  void forceUpdateParams ();
  void disableSound ();
protected slots:
  void stateChanged (Phonon::State newState);
  void finished ();
protected:
  void init ();

  struct Private;
  Private *d;
};

#endif
