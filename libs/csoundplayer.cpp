/***************************************************************************
                          csoundplayer.cpp  -  sound player
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

#include "csoundplayer.h"

struct cSoundPlayer::Private {
  QMediaPlayer player;

  bool isWave;
  bool nosound;
  
  QString fName;
  int repeatCount, priority, volume;
  QString newFName;
  int newRepeatCount, newPriority, newVolume;
};

cSoundPlayer::cSoundPlayer (bool isWAVE)
    : cActionBase (isWAVE ? "soundplayer" : "musicplayer", 0)
{
  d = new Private;

  d->isWave = isWAVE;
  d->nosound = false;
}

cSoundPlayer::~cSoundPlayer()
{
  stop ();
  delete d;
}

void cSoundPlayer::init ()
{
  connect (&d->player, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (stateChanged(QMediaPlayer::State)));
}

bool cSoundPlayer::isPlaying ()
{
  return (d->player.state() == QMediaPlayer::PlayingState);
}

int cSoundPlayer::curPriority ()
{
  return d->priority;
}

int cSoundPlayer::remainingRepeats ()
{
  return d->repeatCount;
}

QString cSoundPlayer::fileName ()
{
  return d->fName;
}

int cSoundPlayer::curVolume ()
{
  return d->volume;
}

void cSoundPlayer::setPriority (int val)
{
  d->newPriority = val;
}

void cSoundPlayer::setRepeatsCount (int val)
{
  d->newRepeatCount = val;
}

void cSoundPlayer::setFileName (const QString &name)
{
  d->newFName = name;
}

void cSoundPlayer::setVolume (int val)
{
  d->newVolume = val;
}

void cSoundPlayer::play ()
{
  if (d->nosound) return;

  // stop existing sound, if any
  stop ();

  //apply new parameters
  d->fName = d->newFName;
  d->repeatCount = d->newRepeatCount;
  d->priority = d->newPriority;
  d->volume = d->newVolume;

  // Intialise the play object, if needed
  init ();

  d->player.setMedia (QUrl::fromLocalFile (d->fName));
  d->player.setVolume (d->volume);
  d->player.play ();
}

void cSoundPlayer::stop ()
{
  if (d->nosound) return;
  d->player.stop();
}

void cSoundPlayer::forceUpdateParams ()
{
  d->repeatCount = d->newRepeatCount;
  d->priority = d->newPriority;
  d->volume = d->newVolume;
}

void cSoundPlayer::disableSound ()
{
  d->nosound = true;
}

void cSoundPlayer::stateChanged (QMediaPlayer::State newState)
{
  if ((newState == QMediaPlayer::StoppedState) && (d->player.mediaStatus() == QMediaPlayer::EndOfMedia))
    finished();

  // TODO error reporting
}

void cSoundPlayer::finished ()
{
  if (d->repeatCount != -1)  //-1 means infinite playing
    d->repeatCount--;  //decrease repeat count
  if (d->repeatCount != 0) {
    // we need to play again - so play again
    d->player.setMedia (QUrl::fromLocalFile (d->fName));
    d->player.setVolume (d->volume);
    d->player.play ();
  }
}

#include "moc_csoundplayer.cpp"
