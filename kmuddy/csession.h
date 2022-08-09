/***************************************************************************
                         csession.h  -  basis of multi-world support
                          -------------------
    begin                : Pi nov 15 2002
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

#ifndef CSESSION_H
#define CSESSION_H

#include <config-mxp.h>
#include "cactionbase.h"

#include <QWidget>

class cTelnet;
class cANSIParser;
class cOutput;
class cInputLine;
class cMultiInputLine;
class cCmdParser;
class cCmdProcessor;
class cCmdQueues;
class cTextProcessor;
class cStatus;
class cConnection;
class cTranscript;
class cPrompt;
class cGaugeBar;
class cScriptAPI;
class cScriptEval;

#ifdef HAVE_MXP
class cMXPManager;
#endif  //HAVE_MXP

/**
This class handles one session.
It's the basis of multi-world support.
It's the widget that contains inputline, console and friends for one active connection.
  *@author Tomas Mecir
  */

class cSession : public QWidget, public cActionBase {
   Q_OBJECT
public: 
  cSession (int sess, QWidget *parent=nullptr);
  ~cSession () override;

  //name
  void setName (const QString &name, bool defName = true);
  const QString &name () { return _name; };
  //flashing
  void setFlashing (bool val) { _flashing = val; };
  bool flashing () { return _flashing; };

  void setAuxInput (bool val);

  QString actionNothingHandler (QString action, int session) override;
  void eventNothingHandler (QString event, int session) override;
  void eventStringHandler (QString event, int session, QString &par1, const QString &par2) override;
protected slots:
  /** switch between standard input and multi-line input */
  void switchInputLines ();

protected:
  //flashing
  bool _flashing;
  //name
  QString _name;
  //do we have a default name ?
  bool defaultName;
  
  //objects
  cTelnet *_telnet;
  cANSIParser *_ansiparser;
  cOutput *_output;
  cInputLine *_inputline, *_auxinputline;
  cMultiInputLine *_multiinputline;
  cCmdParser *_parser;
  cCmdQueues *_cmdqueues;
  cCmdProcessor *_cmdproc;
  cTextProcessor *_textproc;
  cStatus *_status;
  cConnection *_connection;
  cTranscript *_transcript;
  cPrompt *_prompt;
  cGaugeBar *_gaugebar;
  cScriptAPI *_scriptapi;
  cScriptEval *_scripteval;
#ifdef HAVE_MXP
  cMXPManager *_mxpmanager;
#endif //HAVE_MXP
};

#endif
