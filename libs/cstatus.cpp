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

#include <QLabel>
#include <QStatusBar>
#include <QTextStream>
#include <KLocalizedString>

#include "cprofilesettings.h"

cStatus::cStatus (int sess, QStatusBar *statusbar)
  : cActionBase ("status", sess)
{
  sb = statusbar;

//  QLabel *labelDimension, *labelTimer, *labelIdle, *labelConnected, *labelVariables, *labelPartial;

  labelPartial = new QLabel();
  sb->addWidget(labelPartial);

  labelVariables = new QLabel();
  labelVariables->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  sb->addWidget(labelVariables, 20);

  labelConnected = new QLabel(" " + i18n ("Off-line") + " ");
  sb->addPermanentWidget(labelConnected);

  labelDimension = new QLabel(" ??x?? ");
  sb->addPermanentWidget(labelDimension);

  labelTimer = new QLabel(" 0:00:00 ");
  sb->addPermanentWidget(labelTimer);
  
  labelIdle = new QLabel(" " + i18n ("idle") + " 0:00 ");
  sb->addPermanentWidget(labelIdle);

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
    labelTimer->setText (" 0:00:00 ");
  }
  timerShown = true;
}

void cStatus::hideTimer ()
{
  if (timerShown)
  {
    timerStop ();
    labelTimer->setText(QString());   //item is invisible when its text is empty
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
  labelDimension->setText(s);
}

void cStatus::timerStart ()
{
  timing = true;
}

void cStatus::timerStop ()
{
  timing = false;

  timer1->stop ();
  labelIdle->setText(QString());
}

void cStatus::timerReset ()
{
  timing = true;
  conntime = 0;
  idletime1 = 0;
  if (timerShown)
    labelTimer->setText (" 0:00:00 ");
  labelIdle->setText (" " + i18n ("idle") + " 0:00");
  timer1->start (1000);
}

void cStatus::connected ()
{
  labelConnected->setText (" " + i18n ("Connected") + " ");
  labelPartial->setText (QString());
  labelIdle->setText (" " + i18n ("idle") + " 0:00");
  showMessage (i18n ("Connected."));
}

void cStatus::disconnected ()
{
  labelConnected->setText (" " + i18n ("Off-line") + " ");
  showMessage (i18n ("Disconnected."));
  labelPartial->setText (QString());
  invokeEvent ("message", sess(), i18n ("Connection has been closed."));
  timerStop ();
}

void cStatus::partialLine (const QString &line)
{
  //partial line displayed if enabled in profile prefs, or if this is a quick
  //connection
  cProfileSettings *sett = settings ();
  if ((!sett) || sett->getBool ("prompt-status"))
    labelPartial->setText (line);
  else
    clearPartialLine();  // clear existing prompt, if any
}

void cStatus::clearPartialLine ()
{
  labelPartial->setText (QString());
}

void cStatus::gotCommand ()
{
  if (timing)
  {
    idletime1 = 0;
    timer1->stop ();
    timer1->start (1000);
    labelIdle->setText (" " + i18n ("idle") + " 0:00 ");
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
  labelVariables->setText(varText);
}

void cStatus::timerTick ()
{
  QString s2;
  if (timing)
  {
    conntime++;

    s2 = connTimeString ();
    labelTimer->setText (s2);
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
  QTextStream(&ss) << " " << h << ":" << m << ":" << s << " ";
  if (h > 0)
    ss = QString(" %1:%2:%3 ").arg(h).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
  else
    ss = QString(" %1:%2 ").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));

  labelIdle->setText (" " + i18n ("idle") + ss);
}

#include "moc_cstatus.cpp"
