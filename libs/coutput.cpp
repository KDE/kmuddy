//
// C++ Implementation: cOutput
//
// Description: the cOutput class
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

#include "coutput.h"

#include "cglobalsettings.h"
#include "cconsole.h"
#include "cprofilesettings.h"
#include "ctextchunk.h"

#include <QScrollBar>

#include <kapplication.h>
#include <KLocalizedString>
#include <krandom.h>

cOutput::cOutput (int sess, QWidget *parent) : QScrollArea (parent), cActionBase ("output", sess)
{
  con = new cConsole (this);
  setWidget (con);
  con->setSession (sess);

  echocolor = Qt::yellow;
  systemcolor = Qt::cyan;
  bgcolor = Qt::black;

  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);

  // enable vertical scrollbar, disable the other one
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

  // connect cConsole to us ...
  connect (con, SIGNAL (dimensionsChanged (int, int)), this,
      SLOT (dimensionsChanged (int, int)));
  connect (con, SIGNAL (sendCommand (const QString &)), this,
      SLOT (sendCommand (const QString &)));
  connect (con, SIGNAL (promptCommand (const QString &)), this,
      SLOT (promptCommand (const QString &)));

  //and connect() slider so that aconsole is shown/hidden as needed
  connect (verticalScrollBar (), SIGNAL (sliderMoved (int)), this, SLOT (sliderChanged (int)));
  connect (verticalScrollBar (), SIGNAL (valueChanged (int)), this, SLOT (sliderChanged (int)));
    
  // react on events
  addEventHandler ("display-line", 20, PT_TEXTCHUNK);
  addEventHandler ("display-prompt", 20, PT_TEXTCHUNK);
  addEventHandler ("command-sent", 50, PT_STRING);
  addEventHandler ("message", 50, PT_STRING);
  addEventHandler ("user-message", 50, PT_STRING);
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);

  aconsize = 25;
  con->setScrollTextSize (aconsize);
}


cOutput::~cOutput ()
{
  removeEventHandler ("display-line");
  removeEventHandler ("display-prompt");
  removeEventHandler ("display-sent");
  removeEventHandler ("message");
  removeEventHandler ("user-message");
  removeGlobalEventHandler ("global-settings-changed");
}

void cOutput::eventNothingHandler (QString event, int /*session*/)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    con->setInitialHistorySize (gs->getInt ("history-size"));

    con->setFont (gs->getFont ("console-font"));
    setEchoColor (gs->getColor ("color-" + QString::number (gs->getInt ("echo-color"))));
    setSystemColor (gs->getColor ("color-" + QString::number (gs->getInt ("system-color"))));
    setEnableEcho (gs->getBool ("command-echo"));
    setEnableMessages (gs->getBool ("show-messages"));
    con->setEnableBlinking (gs->getBool ("allow-blink"));
    con->setIndentation (gs->getInt ("indent"));
    con->setRepaintCount (gs->getInt ("force-redraw"));

    //changing font often causes view to move - move to the very bottom
    verticalScrollBar()->setValue (verticalScrollBar()->maximum());

    con->tryUpdateHistorySize ();
  }
}

void cOutput::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "command-sent") {
    if (cmdecho)
      addCommand (par1);
  }
  else if (event == "message") {
    if (messages)  //only if messages are enabled
      systemMessage (par1);
  }
  else if (event == "user-message") {
    systemMessage (par1);
  }
}

void cOutput::eventChunkHandler (QString event, int, cTextChunk *chunk)
{
  if (event == "display-line")
    addLine (chunk);
  if (event == "display-prompt")
    addText (chunk);
}

void cOutput::dimensionsChanged (int x, int y)
{
  invokeEvent ("dimensions-changed", sess(), x, y);
}

void cOutput::sliderChanged (int val)
{
  int maxval = verticalScrollBar()->maximum ();
  bool vis = (val < maxval);
  con->setScrollTextVisible (vis);
}

void cOutput::sendCommand (const QString &command)
{
  // we send the command directly, with no parsing, because these commands
  // are coming from the server, and we want to keep some security ...
  invokeEvent ("send-command", sess(), command);
}

void cOutput::promptCommand (const QString &command)
{
  callAction ("inputline", "set-text", sess(), command);
}

void cOutput::setDefaultBkColor (QColor color)
{
  bgcolor = color;
  con->setDefaultBkColor (color);
}

void cOutput::setEchoColor (QColor color)
{
  echocolor = color;
}

void cOutput::setSystemColor (QColor color)
{
  systemcolor = color;
}

void cOutput::getAllColors (QColor &_echo, QColor &_system)
//this is used by cTranscript class to determine correct colors for command output
{
  _echo = echocolor;
  _system = systemcolor;
}

void cOutput::setEnableEcho (bool value)
{
  cmdecho = value;
}

void cOutput::setEnableMessages (bool value)
{
  messages = value;
}

void cOutput::addText (cTextChunk *chunk)
{
  cProfileSettings *sett = settings ();
  //prompt displayed if enabled in profile prefs, or if it's a quick-connection
  if ((!sett) || sett->getBool ("prompt-console"))
  {
    con->addText (chunk);
    invokeEvent ("displayed-prompt", sess(), chunk);
  }
}

void cOutput::addLine (cTextChunk *chunk)
{
  con->addLine (chunk);
  invokeEvent ("displayed-line", sess(), chunk);
}

void cOutput::addCommand (const QString &text)
//addText plus some colouring...
{
  //IMPORTANT: the signal emitted here is also captured by session transcript
  //so, if you disable cmd echo here, it won't work with session
  //transcript!!!

  cTextChunk *chunk = cTextChunk::makeLine (text, echocolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::systemMessage (const QString &text)
//addText plus some colouring...
{
  con->forceBeginOfLine ();
  cTextChunk *chunk = cTextChunk::makeLine (text, systemcolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::decisionMessage (const QString &text)
//addText plus some colouring...
{
  con->forceBeginOfLine ();
  cTextChunk *chunk = cTextChunk::makeLine (text, systemcolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::makeDecision ()
{
  QString ss;
  //generate a random number in 0..9 range
  int which = KRandom::random() % 10;
  switch (which) {
    case 0: ss = i18n ("No, no, no!"); break;
    case 1: ss = i18n ("I don't agree with it."); break;
    case 2: ss = i18n ("Better not."); break;
    case 3: ss = i18n ("I'd probably suggest to reject."); break;
    case 4: ss = i18n ("Saying no seems a bit better."); break;
    case 5: ss = i18n ("Saying yes seems a bit better."); break;
    case 6: ss = i18n ("I'd probably suggest to accept."); break;
    case 7: ss = i18n ("Sounds good."); break;
    case 8: ss = i18n ("I agree with it."); break;
    case 9: ss = i18n ("Definitely yes!"); break;
  };
  QString s = i18n ("My decision: %1", ss);
  //now display that decision
  decisionMessage (s);
}

void cOutput::aconUp ()
{
  //85% is max size
  if (aconsize > 80) return;
  aconsize += 5;
  
  con->setScrollTextSize (aconsize);
}

void cOutput::aconDown ()
{
  //5% is min size
  if (aconsize < 10) return;
  aconsize -= 5;
  
  con->setScrollTextSize (aconsize);
}

