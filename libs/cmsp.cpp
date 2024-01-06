/***************************************************************************
                          cmsp.cpp  -  MUD Sound Protocol
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

#include "cmsp.h"

#include "cprofilesettings.h"
#include "cdownloader.h"
#include "csoundplayer.h"

#include <stdlib.h>

#include <QDir>
#include <QStandardPaths>

#include <KLocalizedString>

using namespace std;

cMSP::cMSP (int sess) : cActionBase ("msp", sess)
{
  //random number generator is initialized in KMuddy::KMuddy
  downloader = new cDownloader (this);

  // retrieve soundPlayer and musicPlayer
  soundPlayer = dynamic_cast<cSoundPlayer *>(object ("soundplayer", 0));
  midiPlayer = dynamic_cast<cSoundPlayer *>(object ("musicplayer", 0));
  
  mspenabled = false;
  mspallowed = true;
  dloadallowed = false;
  //reset() must NOT be called here - it will be called in cTelnet::connectIt
}

cMSP::~cMSP ()
{
  delete downloader;
}

void cMSP::reset (const QString &serverName)
{
  mspenabled = false;
  mspallowed = true;
  dloadallowed = false;
  state = 1;
  cachedString = "";
  triggerContents = "";
  defaultURL = QString();
  localdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds/" + serverName;
  downloader->reset ();
}

void cMSP::enableMSP ()
{
  mspenabled = true;
}

void cMSP::disableMSP ()
{
  mspenabled = false;
  state = 1;
  cachedString = "";
  triggerContents = "";
}

string cMSP::parseServerOutput (const string &output)
{
  /*
  Explanation of parser states:
  0 = no MSP tags allowed here; any that come shall not be caught
  1 = MSP tag could begin now; machine enters this state after \n has been
      received
  2 = got first !
  3 = got second !
  4-8 = got (state-3)th letter of word SOUND
  9-13 = got (state-8)th letter of word MUSIC
  14 = got opening parenthesis, now reading parameters; if we get \n while
      in this state, we discard that sound trigger (unfinished); we jump
      back to 0 or 1 when the trigger is complete (depending on mid-line
      setting)
  */

  //support mid-line triggers if necessary
  bool allowmidline = false;
  cProfileSettings *sett = settings();
  if (sett) allowmidline = sett->getBool ("midline-msp");
  if (allowmidline && (state == 0))
    state = 1;  //no zero-state when mid-line support is ON
  //state to enter after the MSP trigger; this is to allow multiple sound
  //triggers on one line (if mid-line triggers are supported)
  int endingstate = allowmidline ? 1 : 0;

  string newoutput;
  int len = output.length ();
  string SOUND = "SOUND";
  string MUSIC = "MUSIC";
  for (int i = 0; i < len; i++)
  {
    char ch = output[i];
    switch (state)
    {
      case 0:
        //going to state 1 is handled after the switch statement
        newoutput += ch;
        break;
      case 1:
        if (ch == '!')
        {
          state = 2;
          cachedString = "!";
        }
        else
        {
          if ((!allowmidline) && (!(QChar(ch).isSpace ()))) //not a sound trigger
            state = 0;
          newoutput += ch;
        }
        break;
      case 2:
        cachedString += ch;
        if (ch == '!')
          state = 3;
        else
        {
          state = endingstate;
          newoutput += cachedString;
          cachedString = "";
        }
        break;
      case 3:
        cachedString += ch;
        if (ch == SOUND[0])
          state = 4;
        else
          if (ch == MUSIC[0])
            state = 9;
          else
          {
            state = endingstate;
            newoutput += cachedString;
            cachedString = "";
          }
        break;
      case 4:
      case 5:
      case 6:
      case 7:
        cachedString += ch;
        if (ch == SOUND[state - 3])
          state++;
        else
        {
          state = endingstate;
          newoutput += cachedString;
          cachedString = "";
        }
        break;
      case 8:
        cachedString += ch;
        if (ch == '(')
        {
          state = 14;
          inSOUND = true;
          cachedString = "";
          triggerContents = "";
        }
        else
          if (ch != ' ')    //allow spaces between SOUND and (
          {
            state = endingstate;
            newoutput += cachedString;
            cachedString = "";
          }
        break;
      case 9:
      case 10:
      case 11:
      case 12:
        cachedString += ch;
        if (ch == MUSIC[state - 8])
          state++;
        else
        {
          state = endingstate;
          newoutput += cachedString;
          cachedString = "";
        }
        break;
      case 13:
        cachedString += ch;
        if (ch == '(')
        {
          state = 14;
          inSOUND = false;
          cachedString = "";
          triggerContents = "";
        }
        else
          if (ch != ' ')    //allow spaces between SOUND and (
          {
            state = endingstate;
            newoutput += cachedString;
            cachedString = "";
          }
        break;
      case 14:
        if ((ch == '\n') || (ch == '\r'))
        //unfinished trigger! - IGNORE IT!
        {
          cachedString = "";
          triggerContents = "";
          state = 1;
          newoutput += ch;
          invokeEvent ("message", sess(), "MSP: unfinished sound trigger!");
        }
        else
          if (ch == ')')
          {
            state = endingstate;
            //we negotiate and parse MSP stuff even if we're told not
            //to do so; we therefore need to look if the user wants to hear
            //sounds
            if (mspallowed && ((!sett) || sett->getBool ("use-msp")))
              parseTrigger (QString (triggerContents.c_str()), inSOUND);
            triggerContents = "";
          }
          else
            triggerContents += ch;
        break;
    }
    //go to state 1 if we're in state 0 and end-of-line comes
    //this must be placed outside the switch statement, because we can enter
    //state 0 when an unfinished trigger is received; and if it's terminated
    //by \n, we would end up going through the next line in state 0, ignoring
    //a sound trigger on that line (if any)
    if ((state == 0) && ((ch == '\n') || (ch == '\r')))
      state = 1;
  }
  return newoutput;
}

QString cMSP::nextToken (QString &from)
{
  //strip whitespaces
  from = from.trimmed ();
  if (from.length() == 0)
    return QString();
  
  //return value...
  QString ret;
  //'=' sign first
  if (from[0] == '=')
  {
    ret = "=";
    from = from.remove (0, 1);
  }
  else
  {
    //space and = are separators
    ret = from.section (' ', 0, 0, QString::SectionSkipEmpty);
    ret = ret.section ('=', 0, 0, QString::SectionSkipEmpty);
    int len = ret.length();
    from = from.remove (0, len);
  }
  from = from.trimmed ();
  return ret;
}

void cMSP::corruptedTrigger (const QString &reason)
{
  invokeEvent ("message", sess(), i18n ("MSP: Received corrupted sound trigger!"));
  invokeEvent ("message", sess(), i18n ("MSP: Problem was: %1", reason));
}

void cMSP::parseTrigger (const QString &seq, bool isSOUND)
{
  QString fName = QString(), type, url;
  int volume = 100, repeats = 1, priority = isSOUND ? 50 : 1;
  int parserState = 0;
  QString paramName, paramValue;

  QString trigger = seq;
  while (trigger.length() > 0)
  {
    QString token = nextToken (trigger);
    if (fName.length() == 0)  //no fName yet => this token is fName
      fName = token;
    else
    {
      if (parserState == 0)
      {
        paramName = token;
        parserState++;
      }
      else
      if (parserState == 1)
      {
        if (token == QString("="))
          parserState++;
        else
        {
          corruptedTrigger (i18n ("Parameter names must be followed by '='."));
          return;
        }
      }
      else
      if (parserState == 2)
      {
        paramValue = token;
        parserState = 0;

        //only if param name length is exactly 1; all params in MSP 0.3
        //should have this length; longer params are ignored, but not
        //reported as error, as they could be valid in later MSP versions
        if (paramName.length() == 1)
        {
          bool ok;
          int number = 0;

          char ch = paramName[0].toUpper().toLatin1();
          //we'll need this with these params
          if ((ch == 'V') || (ch == 'L') || (ch == 'P') || (ch == 'C'))
          {
            number = paramValue.toInt (&ok);
            if (!ok)
            {
              corruptedTrigger (i18n ("Parameter %1 requires a numeric argument", QChar(ch)));
              return;
            }
          }

          //fill in appropriate variable

          if (ch == 'V')
          {
            if ((number < 0) || (number > 100))
            {
              corruptedTrigger (i18n ("Value of param V is out of 0-100 range."));
              return;
            }
            volume = number;
          }
          if (ch == 'L')
          {
            if (((number < 1) && (number != -1)) || (number > 100))
            {
              corruptedTrigger (i18n ("Value of param L is out of -1;1-100 range."));
              return;
            }
            repeats = number;
          }
          if (ch == 'T')
            type = paramValue;
          if (ch == 'U')
            url = paramValue;
          if ((ch == 'P') && (isSOUND))
          {
            if ((number < 0) || (number > 100))
            {
              corruptedTrigger (i18n ("Value of param P is out of 0-100 range."));
              return;
            }
            priority = number;
          }
          if ((ch == 'C') && (!isSOUND))
          {
            if ((number != 0) && (number != 1))
            {
              corruptedTrigger (i18n ("Value of param C must be 0 or 1."));
              return;
            }
            priority = number;
          }
        }
      }
    }
  }
  if (parserState != 0)   //unfinished sequence!!!
  {
    corruptedTrigger (i18n ("Received unfinished sequence."));
    return;
  }

  //trigger parsed successfully, so process the request
  processRequest (isSOUND, fName, volume, repeats, priority, type, url);
}

void cMSP::processRequest (bool isSOUND, QString fName, int volume, int repeats, int priority,
      QString type, QString url)
{
  //determine file name
  //special case - name is OFF
  if (fName.toLower() == "off")
  {
    if (url.length() > 0)
      //if U param is given here, we're setting the default URL; this is not
      //in official standard, but it's in zMUD, so I implement it as well
      defaultURL = url;
    else
      //off received - turn sound/music off
      if (isSOUND)
        soundOff ();
      else
        musicOff ();
    return;
  }

  //apply default URL, if we have one and if it's needed
  if ((url.length() == 0) && (defaultURL.length() > 0))
    url = defaultURL;

  //sometimes the URL already includes the file name...
  if (url.right (fName.length()) == fName)
    url = url.remove (url.length() - fName.length(), fName.length());

  //if no extension is specified, assume .wav or .mid, depending on trigger
  //type (SOUND/MUSIC)
  QString filename = fName.section ('/', -1, -1, QString::SectionSkipEmpty);
  if (!(filename.contains (".")))
    fName += isSOUND ? QString(".wav") : QString(".mid");

  //try to find that file in MUD-specific directory and in directories
  //specified in config

  QString file = findFile (fName);
  if (file == QString())  //failed to find that file :(
  {
    //no file found... will need to download it, if possible
    if ((url.length() > 0) && dloadallowed)
    {
      if (downloader->downloading())
      {
        //ignore requests for multiple downloading; the second file will
        //be downloaded when it's requested again
        invokeEvent ("message", sess(),
          i18n ("MSP: Multiple downloads not supported, request ignored."));
        return;
      }
      else
      {
        //store download parameters
        dl_fName = fName;
        dl_type = type;
        dl_url = url;
        dl_volume = volume;
        dl_repeats = repeats;
        dl_priority = priority;
        dl_issound = isSOUND;
        downloadFile ();
      }
    }
    else
      if (!url.isEmpty())
        //inform about failed request
        invokeEvent ("message", sess(),
            i18n ("MSP: downloading of sounds disabled - request ignored."));
    //not found & no url available or dloads not allowed -> NO SOUND
  }
  else
  {
    //great, file is here - play it!
    if (isSOUND)
      playSound (file, volume, repeats, priority);
    else
      playMusic (file, volume, repeats, (priority == 1));
  }
}

void cMSP::soundOff ()
{
  soundPlayer->stop ();
}

void cMSP::musicOff ()
{
  midiPlayer->stop ();
}

void cMSP::playSound (const QString &path, int volume, int repeats, int priority)
{
  if (soundPlayer->isPlaying())
  {
    //soundPlayer is currently playing; we'll see who has greater priority...
    if (soundPlayer->curPriority() >= priority)
      //he should continue...
      return;
    //our priority is greater - we will play, so let's stop old sound!
    soundPlayer->stop ();
  }

  //ok, now go and play :)
  soundPlayer->setFileName (path);
  soundPlayer->setPriority (priority);
  soundPlayer->setRepeatsCount (repeats);
  soundPlayer->setVolume (volume);
  soundPlayer->play ();
}

void cMSP::playMusic (const QString &path, int volume, int repeats, bool continueIfRerequested)
{
  if (midiPlayer->isPlaying())
  {
    //midiPlayer is currently playing...

    //see what's being played...
    if (midiPlayer->fileName() == path)
    {
      if (midiPlayer->curPriority () == 1)
      {
        //continue playing, but with new params
        midiPlayer->setRepeatsCount (repeats);
        midiPlayer->setPriority (continueIfRerequested ? 1 : 0);
        midiPlayer->setVolume (volume);
        midiPlayer->forceUpdateParams ();
      }
      else
      {
        //restart playing
        midiPlayer->stop ();
      }
    }
    else
    {
      //if he's playing something different, he should stop and play new music
      midiPlayer->stop ();
    }
  }

  //ok, now go and play :)
  midiPlayer->setFileName (path);
  midiPlayer->setPriority (continueIfRerequested ? 1 : 0);
  midiPlayer->setRepeatsCount (repeats);
  midiPlayer->setVolume (volume);
  midiPlayer->play ();
}

void cMSP::downloadFile ()
{
  downloader->download (dl_url + "/" + dl_fName, localdir + "/" + dl_fName);
}

void cMSP::downloadCompleted ()
{
  QString file = localdir + "/" + dl_fName;
  if (dl_issound)
    playSound (file, dl_volume, dl_repeats, dl_priority);
  else
    playMusic (file, dl_volume, dl_repeats, (dl_priority == 1));
}

void cMSP::downloadFailed (const QString &reason)
{
  invokeEvent ("message", sess(), "MSP: " + reason);
}

QString cMSP::getFileName (QString where, QString what)
{
  QString fName = what.section ("/", -1, -1, QString::SectionSkipEmpty);
  QString attempt1 = where + "/" + what;
  QString attempt2 = where + fName;
  //directory is path without last part (file name)
  QString dir1 = attempt1.section ("/", 0, -2, QString::SectionSkipEmpty | QString::SectionIncludeLeadingSep);
  QString dir2 = attempt2.section ("/", 0, -2, QString::SectionSkipEmpty | QString::SectionIncludeLeadingSep);
  QStringList fileList;
  QString retString;
  
  QDir d1 (dir1, fName);
  retString = dir1;
  fileList = d1.entryList ();
  if (fileList.count() == 0)
  {
    //no files in dir1
    QDir d2 (dir2, fName);
    retString = dir2;
    fileList = d2.entryList ();
  }
  int count = fileList.count();
  if (count > 0)
  {
    //found!!! Return (count+1)th file name
    int randomNumber = random() % count;
    QStringList::iterator it;
    for (it = fileList.begin(); it != fileList.end(); ++it)
      if ((randomNumber--) == 0)
        break;
    return retString + "/" + *it;
  }
  return QString();
}

QString cMSP::findFile (const QString &path)
{
  //okay, let's work...
  QString file;
  QStringList dirlist;
  QStringList::iterator it;
  
  //first of all, have a look in profile directory
  file = getFileName (localdir, path);
  if (file != QString())
    return file;
    
  //nothing... check profile-specific sound directories
  cProfileSettings *sett = settings ();
  if (sett)
  {
    int cnt = sett->getInt ("sound-dir-count");
    for (int i = 1; i <= cnt; ++i)
    {
      file = getFileName (sett->getString ("sound-dir-"+QString::number(i)), path);
      if (!file.isEmpty())
        return file;
    }
  }
  
  //still nothing... check global sound directories
  for (it = globaldirs.begin(); it != globaldirs.end(); ++it)
  {
    file = getFileName (*it, path);
    if (file != QString())
      return file;
  }
  
  //nothing :(
  return QString();
}

