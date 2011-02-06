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

#include <Phonon/MediaObject>
#include <Phonon/Path>
#include <Phonon/AudioOutput>
#include <Phonon/Global>

struct cSoundPlayer::Private {
  Phonon::MediaObject *media;
  Phonon::AudioOutput *output;

  bool isWave;
  bool nosound;
  
  bool playing;
  
  QString fName;
  int repeatCount, priority, volume;
  QString newFName;
  int newRepeatCount, newPriority, newVolume;
};

cSoundPlayer::cSoundPlayer (bool isWAVE)
    : cActionBase (isWAVE ? "soundplayer" : "musicplayer", 0)
{
  d = new Private;

  d->media = 0;
  d->isWave = isWAVE;
  d->nosound = false;

  d->playing = false;
}

cSoundPlayer::~cSoundPlayer()
{
  stop ();
  delete d;
}

void cSoundPlayer::init ()
{
  if (d->media) return;
  d->media = new Phonon::MediaObject (this);
  d->output = new Phonon::AudioOutput (Phonon::MusicCategory, this);
  createPath (d->media, d->output);
  connect (d->media, SIGNAL (finished()), this, SLOT (finished()));
  connect (d->media, SIGNAL (stateChanged(Phonon::State, Phonon::State)), this, SLOT (stateChanged(Phonon::State)));
}

bool cSoundPlayer::isPlaying ()
{
  return d->playing;
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
  d->playing = false;

  //apply new parameters
  d->fName = d->newFName;
  d->repeatCount = d->newRepeatCount;
  d->priority = d->newPriority;
  d->volume = d->newVolume;

  // Intialise the play object, if needed
  init ();

  // assign the source
  d->media->setCurrentSource (d->fName);
  // set volume
  d->output->setVolume ((qreal) d->volume / 100);
  // and play
  d->media->play ();
}

void cSoundPlayer::stop ()
{
  if (d->nosound) return;
  if (!d->media) return;
  d->media->stop();
  d->playing = false;
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

void cSoundPlayer::stateChanged (Phonon::State newState)
{
  d->playing = (newState == Phonon::PlayingState);
  // TODO error reporting
}

void cSoundPlayer::finished ()
{
  d->playing = false;
  
  if (d->repeatCount != -1)  //-1 means infinite playing
    d->repeatCount--;  //decrease repeat count
  if (d->repeatCount != 0) {
    // we need to play again - so play again
    d->media->setCurrentSource (d->fName);
    d->media->play ();
  }
}

#include "csoundplayer.moc"

