/***************************************************************************
                          cstatus.h - manages status bar
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

#ifndef CSTATUS_H
#define CSTATUS_H

#include <qobject.h>
#include <qtimer.h>

#include <cactionbase.h>
#include <kmuddy_export.h>

class QStatusBar;

/**
 This class manages the status bar.
 @author Tomas Mecir
  */

class KMUDDY_EXPORT cStatus : public QObject, public cActionBase {
   Q_OBJECT
public: 
  cStatus (int sess, KStatusBar *statusbar);
  ~cStatus() override;
  void showTimer ();
  void hideTimer ();
  /** shows a message for 2 seconds */
  void showMessage (const QString & message);
  const QString connTimeString ();
  void displayVariables (const QString varText);
  void clearPartialLine ();
  QStatusBar *statusBar() { return sb; };

protected:
  void eventNothingHandler (QString event, int session) override;
  void eventStringHandler (QString event, int session,
      QString &par1, const QString &) override;
  void eventIntHandler (QString event, int session, int par1, int par2) override;

  void dimensionsChanged (int x, int y);
  void timerStart ();
  void timerStop ();
  void timerReset ();
  void connected ();
  void disconnected ();
  void partialLine (const QString &line);
  void gotCommand ();

  QStatusBar *sb;
  QTimer *timer, *timer1;
  bool timing;
  int conntime;
  int idletime1;
  bool timerShown;
protected slots:
  void timerTick ();
  void timer1Tick ();
};

#endif
