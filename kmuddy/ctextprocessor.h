/***************************************************************************
                          ctextprocessor.h  -  text processor
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po oct 14 2002
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

#ifndef CTEXTPROCESSOR_H
#define CTEXTPROCESSOR_H

#include <qobject.h>
#include <qcolor.h>

#include <list>

#include "cactionbase.h"
#include "ctextchunk.h"

using namespace std;

class cTextChunk;

class QTimer;

/**
Text processor - handles trigger execution and such...
  *@author Tomas Mecir
  */

class cTextProcessor : public QObject, public cActionBase {
   Q_OBJECT
public: 
  cTextProcessor (int sess);
  ~cTextProcessor ();
  int linesReceived () { return lines; };

  /** recolorize the line - used by color triggers */
  void recolorize (list<colorChange> &changes);
  /** this line should be gagged - called by gagging triggers */
  void gagLine () { gag = true; };
  /** this line should be gagged in primary window */
  void gagLineInPrimary () { primarygag = true; };
  void setHavePrompt () { haveprompt = true; };
  /** handle some issued when sending a command */
  void sendingCommand ();

  void setOutputWindow(QString winname) { wname = winname; };
  
  virtual void eventStringHandler (QString event, int session, QString &par1, const QString &) override;
  virtual void eventNothingHandler (QString event, int session) override;

public slots:
  // processing of various received items...
  
  /** process this text */
  void gotNewText (const QString &text);
  /** process fg color */
  void gotFgColor (QColor color);
  /** process bg color */
  void gotBgColor (QColor color);
  /** process attribute */
  void gotAttrib (int a);
  /** process A-link */
  void gotALink (const QString &name, const QString &url, const QString &text,
      const QString &hint);
  /** process SEND-link */
  void gotSENDLink (const QString &name, const QString &command, const QString &text,
      const QString &hint, bool toprompt, bool ismenu);
  /** process newline */
  void gotNewLine ();
  /** process tag expiration */
  void gotExpire (const QString &name);
    
signals:
  /** we got this plain text - ANSI parser should be connected here */
  void plainText (const QString &text);
  
  void textLine (const QString &text);
protected slots:
  /** used by prompt-detection timer */
  void timeout ();
protected:

  /** parse this text, pass it to MXP parser if needed and so... */
  void parseText (const QString &text);
  void receivedGA ();
  void flush ();
  /** used to display partial lines in the status bar */
  void moreTextHere ();

  /** create a new chunk */
  void createChunk ();

  /** a timer used to auto-detect prompts */
  QTimer *pdtimer;
  int elapsedticks;

  cTextChunk *chunk;
  
  int lines;

  /** gagging */
  bool gag, primarygag;
  /** prompt detection */
  bool haveprompt, gotprompt;

  bool owin;
  QString wname;
  
  //current text attributes, used to set startpos correctly when breaking lines
  QColor fg, bg;
  int attrib;
};

#endif
