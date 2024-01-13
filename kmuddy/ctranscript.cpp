/***************************************************************************
                          ctranscript.cpp  -  session transcript
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Thu oct 31 2002
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

#include "ctranscript.h"

#include "cactionmanager.h"
#include "cansiparser.h"
#include "cconsole.h"   // needed for buffer dump
#include "coutput.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"
#include "ctelnet.h"
#include "ctextchunk.h"
#include "dialogs/dlgtranscript.h"
#include "dialogs/dlgdumpbuffer.h"

#include <QDir>
#include <QFile>
#include <QPushButton>
#include <QTimer>
#include <KLocalizedString>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kconfiggroup.h>

cTranscript::cTranscript (int sess) : cActionBase ("transcript", sess)
{
  running = false;
  advrunning = false;
  rotatedaily = false;
  overwrite = true;
  includedump = false;
  includetimestamp = false;
  type = advtype = 1;
  nextLineGagged = false;
  
  addEventHandler ("disconnected", 20, PT_NOTHING);
  addEventHandler ("got-line-after-triggers", 50, PT_TEXTCHUNK);
  addEventHandler ("will-gag", 50, PT_NOTHING);
  addEventHandler ("displayed-line", 50, PT_TEXTCHUNK);
  addEventHandler ("displayed-prompt", 50, PT_TEXTCHUNK);
}

cTranscript::~cTranscript ()
{ 
  stopTranscript ();
  stopAdvTranscript ();

  removeEventHandler ("disconnected");
  removeEventHandler ("will-gag");
  removeEventHandler ("got-line-after-triggers");
  removeEventHandler ("displayed-line");
  removeEventHandler ("displayed-prompt");
}

void cTranscript::eventNothingHandler (QString event, int)
{
  if (event == "disconnected") {
    stopTranscript ();
    stopAdvTranscript ();
  }
  else if (event == "will-gag") {
    nextLineGagged = true;
  }
}

void cTranscript::eventChunkHandler (QString event, int, cTextChunk *chunk)
{
  if (event == "displayed-line") {
    addLineToTranscript (chunk);
    addLineToAdvTranscript (chunk);
  }
  else if (event == "displayed-prompt") {
    addLineToTranscript (chunk);
    addLineToAdvTranscript (chunk);
  }
  else if (event == "got-line-after-triggers") {
    // got-line-after-triggers is called for every line, gagged or not
    // if the line will be gagged, will-gag was invoked immediately before this
    if (nextLineGagged) {
      // gagged lines will also be added to the transcript
      // we don't use this event for everything, so that later on, we can configure, whether
      // we want to include gagged lines or not
      addLineToTranscript (chunk);
      addLineToAdvTranscript (chunk);
      nextLineGagged = false;
    }
  }
}

void cTranscript::addLineToTranscript (cTextChunk *chunk)
{
  if (!running)
    return;
  
  cANSIParser *ap = dynamic_cast<cANSIParser *>(object ("ansiparser"));
  QString s;
  switch (type) {
    case TRANSCRIPT_PLAIN: s = chunk->toText () + "\n"; break;
    case TRANSCRIPT_ANSI: s = chunk->toAnsi (ap) + "\n"; break;
    case TRANSCRIPT_HTML: s = chunk->toHTML (); break;
  };
  file.write (s.toLocal8Bit ());
  file.flush();
}

void cTranscript::addLineToAdvTranscript (cTextChunk *chunk)
{
  if (!advrunning)
    return;
  
  QString timestamp;
  if (includetimestamp)
  {
    QTime time = QTime::currentTime ();
    timestamp = QString("[%1:%2:%3.%4] ").arg(time.hour(), 2, QChar('0')).arg(time.minute(), 2, QChar('0')).arg(time.second(), 2, QChar('0')).arg(time.msec() / 10, 2, QChar('0'));
    advfile.write (timestamp.toLatin1());
  }
 
  cANSIParser *ap = dynamic_cast<cANSIParser *>(object ("ansiparser"));
  QString s;
  switch (advtype) {
    case TRANSCRIPT_PLAIN: s = chunk->toText () + "\n"; break;
    case TRANSCRIPT_ANSI: s = chunk->toAnsi (ap) + "\n"; break;
    case TRANSCRIPT_HTML: s = chunk->toHTML (); break;
  };
  advfile.write (s.toLocal8Bit ());
  
  advfile.flush();
}

void cTranscript::stopTranscript ()
{
  if (!running)
    return;
    
  file.write ("\n\n");
  
  if (type == TRANSCRIPT_ANSI)  //ANSI transcript
  {
    //set output color to default (usually gray on black)
    char defcolor[5];
    defcolor[0] = 27;
    defcolor[1] = '[';
    defcolor[2] = '0';
    defcolor[3] = 'm';
    defcolor[4] = 0;
    file.write (defcolor);
  }
  if (type == TRANSCRIPT_HTML)  //HTML transcript
  {
    //closing HTML tags
    file.write ("</pre></body></html>\n");
  }
  
  running = false;
  file.close();
  invokeEvent ("message", sess(), i18n ("Session transcript has been stopped."));
}

void cTranscript::stopAdvTranscript ()
{
  if (!advrunning)
    return;
    
  advfile.write ("\n\n");
  
  if (type == TRANSCRIPT_ANSI)  //ANSI transcript
  {
    //set output color to default (usually gray on black)
    char defcolor[5];
    defcolor[0] = 27;
    defcolor[1] = '[';
    defcolor[2] = '0';
    defcolor[3] = 'm';
    defcolor[4] = 0;
    advfile.write (defcolor);
  }
  if (type == TRANSCRIPT_HTML)  //HTML transcript
  {
    //closing HTML tags
    advfile.write ("</pre></body></html>\n");
  }
    
  advrunning = false;
  advfile.close();
  if (transtimer->isActive ())
  {
    transtimer->stop ();
    delete transtimer;
  }
  invokeEvent ("message", sess(), i18n ("Advanced session transcript has been stopped."));
}

void cTranscript::configure ()
{
  //so first we have to create the dialog...
  tdlg = new dlgTranscript (cActionManager::self()->mainWidget());

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (tdlg, SIGNAL (accepted()), this, SLOT (applyTranscript ()));
  connect (tdlg->button (QDialogButtonBox::Apply), SIGNAL (clicked()), this, SLOT (applyTranscript ()));

  //next we fill in its data
  tdlg->setEnabled (running);

  cProfileSettings *sett = settings ();
  fname = sett ? sett->getString ("transcript-directory") : QDir::homePath();
  
  fname += "/kmuddy_transcript.txt";

  tdlg->setTranscriptType (type);
  tdlg->setFName (fname);
  tdlg->setAFName (fileformat);
  tdlg->setOverwrite (overwrite);
  tdlg->setIncludeDump (includedump);
  tdlg->setAdvTranscript(advrunning);
  tdlg->setRotateDaily (rotatedaily);
  tdlg->setIncludeTimestamp(includetimestamp);  
  tdlg->setAdvTranscriptType (advtype);
  
  //dialog is ready - show it!
  tdlg->exec ();

  //finally, destroy that dialog!
  delete tdlg;
}

void cTranscript::startTranscript ()
{
  //we'll have multiple <html></html> sections in a transcript file if we continue an existing
  //one, but every browser that I've tested can handle it correctly :D
  if (running)
    stopTranscript ();
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet"));
  if (!(telnet->isConnected()))  //no transcript if we aren't connected
    return;
  
  file.setFileName (fname);
  if (!file.open (QIODevice::WriteOnly | QIODevice::Text | ( overwrite ? QIODevice::Truncate : QIODevice::Append )))
  {
    KMessageBox::detailedError (cActionManager::self()->mainWidget(),
        i18n ("Transcript file could not be opened."), file.errorString());
    invokeEvent ("message", sess(), i18n ("Session transcript could not be started."));
    return;
  }

  running = true;
  cOutput *output = dynamic_cast<cOutput *>(object ("output"));
  file.write ("\n\n");
  if (type == TRANSCRIPT_HTML)
  {
    cProfileSettings *sett = settings();
    //TODO: what if we're adding to an existing HTML transcript?
    file.write ("<html><head>\n");
    file.write ("<meta name=\"Generator\" content=\"KMuddy\">\n");
    file.write (("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=" + sett->getString ("encoding") + "\">").toLatin1());
    file.write ("<style> p { margin: 0px; padding: 2px; } </style>");
    file.write ("</head><body bgcolor=");
    file.write (output->defaultBkColor().name().toLatin1());
    file.write (">\n");
  }
  QString s = i18n ("Session transcript has just started.");
  file.write (s.toLatin1());
  if (type == TRANSCRIPT_HTML) file.write ("<br/><pre>");  //pre-formatted text starting...
  file.write ("\n\n");

  //add buffer dump if requested
  if (includedump)
    output->console()->dumpBuffer (false, file, type);

  invokeEvent ("message", sess(), i18n ("Session transcript has been started."));
}

void cTranscript::startAdvTranscript ()
{
  QString af;
  
  if (advrunning)
    stopAdvTranscript ();
  cTelnet *telnet = dynamic_cast<cTelnet *>(object ("telnet"));
  if (!(telnet->isConnected()))  //no transcript if we aren't connected
    return;
  
  if(advfname.isEmpty())
    return; // no transcript if advanced filename failed
  
  transtimer = new QTimer();
  connect (transtimer, SIGNAL (timeout ()), this, SLOT (timeout ()));

  cProfileSettings *sett = settings ();
  af = sett ? sett->getString ("transcript-directory") : QDir::homePath();
  af += advfname;

  file.setFileName (af);
  if (!advfile.open (QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
  {
    KMessageBox::detailedError (cActionManager::self()->mainWidget(),
        i18n ("Advanced transcript file could not be opened."), advfile.errorString());
    invokeEvent ("message", sess(), i18n ("Advanced session transcript could not be started."));
    return;
  }

  advrunning = true;
  cOutput *output = dynamic_cast<cOutput *>(object ("output"));
  advfile.write ("\n\n");
  if (advtype == TRANSCRIPT_HTML)
  {
    //TODO: what if we're adding to an existing HTML transcript?
    advfile.write ("<html>\n");
    advfile.write ("<meta name=\"Generator\" content=\"KMuddy\">\n");
    advfile.write ("<body bgcolor=");
    advfile.write (output->defaultBkColor().name().toLatin1());
    advfile.write (">\n");
  }
  QString s = i18n ("Advanced session transcript has just started.");
  advfile.write (s.toLatin1());
  if (advtype == TRANSCRIPT_HTML) advfile.write ("<br/><pre>");  //pre-formatted text starting...
  advfile.write ("\n\n");

  invokeEvent ("message", sess(), i18n ("Advanced session transcript has been started."));
  
  if (rotatedaily)
  {
    invokeEvent ("message", sess(), i18n ("Session transcript will be rotated at midnight."));
    transtimer->start (60000);
  }
}

void cTranscript::load()
{
  QString path = cProfileManager::self()->profilePath (sess());
  
  KConfig *config = new KConfig (path + "transcript");
  
  KConfigGroup g = config->group ("Advanced transcript");
  setEnableRotate (g.readEntry ("Daily rotation", false));
  setPrependTimestamp (g.readEntry("Prepend timestamp", false));
  setAFName (g.readEntry("Advanced logname", QString()));
  advtype = g.readEntry ("Transcript type", 1);
  delete config;
}

void cTranscript::save()
{
  QString path = cProfileManager::self()->profilePath (sess());
  KConfig *config = new KConfig ( path + "transcript");
  
  KConfigGroup g = config->group ("Advanced transcript");
  g.writeEntry ("Daily rotation", rotatedaily);
  g.writeEntry ("Prepend timestamp", includetimestamp);
  g.writeEntry ("Advanced logname", fileformat);
  g.writeEntry ("Transcript type", advtype);
  delete config;
}

void cTranscript::applyTranscript ()
//handler of OK/Apply in transcript dialog
{
  stopTranscript ();
  stopAdvTranscript();
  overwrite = tdlg->isOverwrite ();
  includedump = tdlg->includeDump ();
  fname = tdlg->getFName ();  
  type = tdlg->transcriptType();
  
  fileformat = tdlg->getAFName(); 
  setAFName (fileformat);
  
  if (tdlg->isEnabled ())
    startTranscript ();  
  
  rotatedaily = tdlg->rotateDaily();
  includetimestamp = tdlg->includeTimestamp();
  advtype = tdlg->advTranscriptType();
  
  if(tdlg->advTranscript())
    startAdvTranscript();
  
  save();
}

void cTranscript::dumpBuffer ()
{
  //so first we have to create the dialog...
  bdlg = new dlgDumpBuffer (cActionManager::self()->mainWidget());

  QString fName;
  cProfileSettings *sett = settings ();
  fName = sett ? sett->getString ("transcript-directory") : QDir::homePath();
  fName += "/buffer_dump.html";
  bdlg->setFileName (fName);
  bdlg->setType (TRANSCRIPT_HTML);
  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (bdlg, SIGNAL (accepted()), this, SLOT (doDumpBuffer ()));

  //dialog is ready - show it!
  bdlg->exec ();

  //finally, destroy that dialog!
  delete bdlg;
}

void cTranscript::doDumpBuffer ()
{
  QString fName = bdlg->fileName();
  int type = bdlg->type();
  QFile f (fName);
  if (!f.open (QIODevice::WriteOnly | QIODevice::Text)) {
    KMessageBox::detailedError (cActionManager::self()->mainWidget(),
        i18n ("Dump file could not be opened."), f.errorString());
    return;
  }
  cOutput *output = dynamic_cast<cOutput *>(object ("output"));
  if (type == TRANSCRIPT_HTML)
  {
    f.write ("<html>\n");
    f.write ("<meta name=\"Generator\" content=\"KMuddy\">\n");
    f.write ("<body bgcolor=");
    f.write (output->defaultBkColor().name().toLatin1());
    f.write ("><pre>\n");
  }
  output->console()->dumpBuffer (bdlg->curPos(), f, type);
  if (type == TRANSCRIPT_HTML)
    f.write ("\n</pre></body></html>\n");
  f.close ();
}

void cTranscript::timeout()
{
  QTime time = QTime::currentTime();
  
  if((time.hour() == 0) && (time.minute() == 0))
  {
    stopAdvTranscript();
    startAdvTranscript();
  }
}

void cTranscript::setAFName(const QString &what)
{
  QString sessname;

  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  
  fileformat = what;

  QString day   = QString("%1").arg(date.day(), 2, QChar('0'));
  QString month = QString("%1").arg(date.month(),  2, QChar('0'));
  QString year  = QString("%1").arg(date.year(),   2, QChar('0'));
  QString hour  = QString("%1").arg(time.hour(),   2, QChar('0'));
  QString min   = QString("%1").arg(time.minute(), 2, QChar('0'));

  if (settings()) {
    cProfileManager *pm = cProfileManager::self();
    sessname = pm->visibleProfileName (pm->profileName (sess()));
  }
  
  if (!what.isEmpty())
  {
    advfname = what;
    advfname.replace("$W", sessname);
    advfname.replace("$D", day);
    advfname.replace("$M", month);
    advfname.replace("$Y", year);
    advfname.replace("$h", hour);
    advfname.replace("$m", min);
    
  }
  else
  {
    if(!sessname.isEmpty())
      advfname = sessname + "-" + day + "-" + month + "-" + year + ".log";
    else
      advfname = "unnamed-" + day + "-" + month + "-" + year + ".log";
  }
}

QString cTranscript::getAFName()
{
  return advfname;
}

#include "moc_ctranscript.cpp"
