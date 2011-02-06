//
// C++ Interface: cOutput
//
// Description: wrapper for cConsole
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef COUTPUT_H
#define COUTPUT_H

#include <qcolor.h>
#include <qobject.h>

#include <cactionbase.h>
#include <kmuddy_export.h>

class cConsole;

/**
This class serves as a wrapper for the main cConsole widget. It translates its signal-slot mechanism to the cAction-based approach. This is necessary, because cConsole is used in other places as well.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cOutput : public QObject, public cActionBase
{
Q_OBJECT
public:
  /** constructor */
  cOutput (int sess, QWidget *parent);
  /** destructor */
  ~cOutput();
  cConsole *console () { return con; };

  void addText (cTextChunk *chunk);
  void addLine (cTextChunk *chunk);
  /** add command to console, if allowed */
  void addCommand (const QString &command);
  /** adds a new system message to the widget - calls addText and also
  does some extra things*/
  void systemMessage (const QString &text);
  /** decision message (Tools/Decide). The same as systemMessage, but
  it is shown even if system messages are off*/
  void decisionMessage (const QString &text);
  /** this is my decision helper similar to one found in zmud */
  void makeDecision ();

  void setDefaultBkColor (QColor color);
  QColor defaultBkColor () { return bgcolor; };
  /** change color of local echo (typed commands etc.) */
  void setEchoColor (QColor color);
  /** change color of system messages */
  void setSystemColor (QColor color);
  /** this function returns all the colors - well, actually, only echo and system color... */
  void getAllColors (QColor &_echo, QColor &_system);
  void setEnableEcho (bool value);
  void setEnableMessages (bool value);
protected slots:
  void dimensionsChanged (int x, int y);
  void sendCommand (const QString &command);
  void promptCommand (const QString &command);
protected:
  virtual void eventStringHandler (QString event, int, QString &par1, const QString &);
  virtual void eventChunkHandler (QString event, int, cTextChunk *chunk);
  virtual void eventNothingHandler (QString event, int session);

  /** show commands / messages ? */
  bool cmdecho, messages;
  
  QColor echocolor;
  QColor systemcolor;

  //default bg color
  QColor bgcolor;
  
  /** stored cConsole widget */
  cConsole *con;
};

#endif
