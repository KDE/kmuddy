//
// C++ Interface: cOutput
//
// Description: wrapper for cConsole
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef COUTPUT_H
#define COUTPUT_H

#include <qcolor.h>
#include <QScrollArea>

#include <cactionbase.h>
#include <kmuddy_export.h>

class cConsole;

/**
This class serves as a wrapper for the main cConsole widget. It translates its signal-slot mechanism to the cAction-based approach. This is necessary, because cConsole is used in other places as well.
It also handles scrollbars.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cOutput : public QWidget, public cActionBase
{
Q_OBJECT
public:
  /** constructor */
  cOutput (int sess, QWidget *parent);
  /** destructor */
  ~cOutput() override;
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

  /** resizing aconsole (CTRL+keys) */
  void aconUp ();
  void aconDown ();

protected slots:
  void dimensionsChanged (int x, int y);
  void sendCommand (const QString &command);
  void promptCommand (const QString &command);
protected:
  void eventStringHandler (QString event, int, QString &par1, const QString &) override;
  void eventChunkHandler (QString event, int, cTextChunk *chunk) override;
  void eventNothingHandler (QString event, int session) override;

  /** show commands / messages ? */
  bool cmdecho, messages;
  
  QColor echocolor;
  QColor systemcolor;

  //default bg color
  QColor bgcolor;
  
  /** stored cConsole widget */
  cConsole *con;
  /** size of auxiliary console (in percents) */
  int aconsize;
};

#endif
