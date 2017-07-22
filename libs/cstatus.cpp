/***************************************************************************
                          cstatus.cpp -  manages status bar
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ne Jul 7 2002
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

#include "cstatus.h"

#include <QTextStream>
#include <klocale.h>

#include "cprofilesettings.h"

#define ID_DIMENSION 1
#define ID_TIMER 2
#define ID_IDLE 3
#define ID_CONNECTED 5
#define ID_VARIABLES 10
#define ID_PARTIAL 11

cStatus::cStatus (int sess, KStatusBar *statusbar)
  : cActionBase ("status", sess)
{
  sb = statusbar;

  sb->insertItem ("", ID_PARTIAL);
  sb->insertItem ("", ID_VARIABLES, 20);
  sb->setItemAlignment (ID_VARIABLES, Qt::AlignLeft | Qt::AlignVCenter);
  sb->insertPermanentItem (" " + i18n ("Off-line") + " ", ID_CONNECTED);
  sb->insertPermanentItem (" ??x?? ", ID_DIMENSION);
  sb->insertPermanentItem (" 0:00:00 ", ID_TIMER);
  sb->insertPermanentItem (" " + i18n ("idle") + " 0:00 ", ID_IDLE);
  timerShown = true;

  //dimension will be signaled soon - don't bother with it now...

  timing = false;
  timer = new QTimer;
  timer1 = new QTimer;
  connect (timer, SIGNAL (timeout ()), this, SLOT (timerTick ()));
  connect (timer1, SIGNAL (timeout ()), this, SLOT (timer1Tick ()));
  timer->start (1000);

  timerTick ();

  addEventHandler ("connected", 200, PT_NOTHING);
  addEventHandler ("disconnected", 200, PT_NOTHING);
  addEventHandler ("got-prompt", 100, PT_STRING);
  addEventHandler ("partial-line", 100, PT_STRING);
  addEventHandler ("command-sent", 100, PT_STRING);
  addEventHandler ("dimensions-changed", 100, PT_INT);
}

cStatus::~cStatus()
{
  removeEventHandler ("connected");
  removeEventHandler ("disconnected");
  removeEventHandler ("got-prompt");
  removeEventHandler ("partial-line");
  removeEventHandler ("command-sent");
  removeEventHandler ("dimensions-changed");
  
  timer->stop ();
  timer1->stop ();
  delete timer;
  delete timer1;
  
  delete sb;
}

void cStatus::eventNothingHandler (QString event, int)
{
  if (event == "connected") {
    connected ();
    timerReset ();
  }
  else if (event == "disconnected") {
    disconnected ();
    timerStop ();
  }
}

void cStatus::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "got-prompt") {
    partialLine (par1);
  }
  else if (event == "partial-line") {
    partialLine (par1);
  }
  else if (event == "command-sent") {
    gotCommand ();
  }
}

void cStatus::eventIntHandler (QString event, int, int par1, int par2)
{
  if (event == "dimensions-changed") {
    dimensionsChanged (par1, par2);
  }
}

void cStatus::showTimer ()
{
  if (!timerShown)
  {
    timerReset ();
    sb->changeItem (" 0:00:00 ", ID_TIMER);
  }
  timerShown = true;
}

void cStatus::hideTimer ()
{
  if (timerShown)
  {
    timerStop ();
    sb->changeItem ("", ID_TIMER);  //item is invisible when its text is empty
  }
  timerShown = false;
}

void cStatus::showMessage (const QString & message)
{
  sb->showMessage (message, 2000);
}

void cStatus::dimensionsChanged (int x, int y)
{
  QString s = " " + QString::number (x) + "x" + QString::number (y) + " ";
  sb->changeItem (s, ID_DIMENSION);
}

void cStatus::timerStart ()
{
  timing = true;
}

void cStatus::timerStop ()
{
  timing = false;

  timer1->stop ();
  sb->changeItem ("", ID_IDLE);
}

void cStatus::timerReset ()
{
  timing = true;
  conntime = 0;
  idletime1 = 0;
  if (timerShown)
    sb->changeItem (" 0:00:00 ", ID_TIMER);
  sb->changeItem (" " + i18n ("idle") + " 0:00", ID_IDLE);
  timer1->start (1000);
}

void cStatus::connected ()
{
  sb->changeItem (" " + i18n ("Connected") + " ", ID_CONNECTED);
  sb->changeItem ("", ID_PARTIAL);
  sb->changeItem (" " + i18n ("idle") + " 0:00 ", ID_IDLE);
  showMessage (i18n ("Connected."));
}

void cStatus::disconnected ()
{
  sb->changeItem (" " + i18n ("Off-line") + " ", ID_CONNECTED);
  showMessage (i18n ("Disconnected."));
  sb->changeItem ("", ID_PARTIAL);
  invokeEvent ("message", sess(), i18n ("Connection has been closed."));
  timerStop ();
}

void cStatus::partialLine (const QString &line)
{
  //partial line displayed if enabled in profile prefs, or if this is a quick
  //connection
  cProfileSettings *sett = settings ();
  if ((!sett) || sett->getBool ("prompt-status"))
    sb->changeItem (line, ID_PARTIAL);
  else
    clearPartialLine();  // clear existing prompt, if any
}

void cStatus::clearPartialLine ()
{
  sb->changeItem (QString(), ID_PARTIAL);
}

void cStatus::gotCommand ()
{
  if (timing)
  {
    idletime1 = 0;
    timer1->stop ();
    timer1->start (1000);
    sb->changeItem (" " + i18n ("idle") + " 0:00 ", ID_IDLE);
  }
}

const QString cStatus::connTimeString ()
{
  int h = conntime / 3600;
  int s = conntime % 3600;
  int m = s / 60;
  s = s % 60;
  QString str;
  QTextStream ts(&str);
  ts << " " << h << ":" << qSetFieldWidth(2) << qSetPadChar('0') << m << qSetFieldWidth(1) << ":" << qSetFieldWidth(2) << s;
  return str;
}

void cStatus::displayVariables (const QString varText)
{
  sb->changeItem (varText, ID_VARIABLES);
}

void cStatus::timerTick ()
{
  QString s2;
  if (timing)
  {
    conntime++;

    s2 = connTimeString ();
    sb->changeItem (s2, ID_TIMER);
  }
}

void cStatus::timer1Tick ()
{
  ++idletime1;
  
  int h, m, s;
  s = idletime1 % 60;
  m = ((idletime1 - s) / 60) % 60;
  h = (idletime1 - s) / 3600;
  
  QString ss;
  if (h > 0)
    ss.sprintf (" %d:%02d:%02d ", h, m, s);
  else
    ss.sprintf (" %d:%02d ", m, s);

  sb->changeItem (" " + i18n ("idle") + ss, ID_IDLE);
}

#include "cstatus.moc"

