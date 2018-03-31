//
// C++ Implementation: cmultiinputline
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cmultiinputline.h"

#include "cglobalsettings.h"
#include "ccmdqueue.h"
#include "ccmdqueues.h"

#include <QKeyEvent>
#include <QFontDatabase>
#include <QTextBlock>

//maximum number of lines ...
#define MAXLINES 10

cMultiInputLine::cMultiInputLine (int sess, QWidget *parent)
 : QTextEdit(parent), cActionBase ("multiinputline", sess)
{
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed);

  setAcceptRichText (false);
  setWordWrapMode (QTextOption::WrapAtWordBoundaryOrAnywhere);

  //height: 2 lines
  setLinesHeight (2);

  connect (this, SIGNAL (textChanged ()), this, SLOT (updateHeight()));

  keeptext = true;
  selectkepttext = true;
  swapenters = false;

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}


cMultiInputLine::~cMultiInputLine()
{
  removeGlobalEventHandler ("global-settings-changed");
}

void cMultiInputLine::eventNothingHandler (QString event, int session)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();

    setMyFont (gs->getFont ("input-font"));
    keepText (gs->getBool ("keep-text"));
    selectKeptText (gs->getBool ("keep-text") ? gs->getBool ("select-kept") : false);
    swapEnters (gs->getBool ("swap-enters"));
    QPalette p = palette ();
    p.setColor (QPalette::Base, gs->getColor("color-" + QString::number (gs->getInt ("input-bg-color"))));
    p.setColor (QPalette::Text, gs->getColor("color-" + QString::number (gs->getInt ("input-fg-color"))));
    setPalette (p);
  }
}

void cMultiInputLine::initialize ()
{
  //change colors
  // TODO: full color support
  cGlobalSettings *gs = cGlobalSettings::self();
  QPalette p = palette ();
  p.setColor (QPalette::Base, gs->getColor ("color-0"));  // black
  p.setColor (QPalette::Text, gs->getColor ("color-14"));  // yellow
  setPalette (p);

  //scroll-bars
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
  
  //change font
  setMyFont (QFontDatabase::systemFont (QFontDatabase::FixedFont)); //default system fixed font
}

void cMultiInputLine::setMyFont (const QFont &font)
{
  setCurrentFont (font);
  setLinesHeight (_lines);
}

void cMultiInputLine::keepText (bool value)
{
  keeptext = value;
}

void cMultiInputLine::selectKeptText (bool value)
{
  selectkepttext = value;
}

void cMultiInputLine::swapEnters (bool value)
{
  swapenters = value;
}

void cMultiInputLine::setLinesHeight (int lines)
{
  if (lines < 2) lines = 2;
  if (lines > MAXLINES) lines = MAXLINES;
  _lines = lines;
  QFontMetrics fm (font());
  //one pixel space between lines, also one pixel spaces at the top/bottom
  int newheight = lines * (fm.height() + 1) + 2;  
  int frameheight = rect().height() - contentsRect().height();
  setFixedHeight (frameheight + newheight + 1);
}

void cMultiInputLine::updateHeight()
{
  setLinesHeight (document()->lineCount());
}

void cMultiInputLine::sendCommands ()
{
  cCmdQueues *queues = (cCmdQueues *) object ("cmdqueues");
  if (!queues) return;
  // create a command queue with all the commands
  cCmdQueue *queue = new cCmdQueue (sess());
  for (QTextBlock block = document()->begin(); block.isValid(); block = block.next())
    queue->addCommand (block.text());
  queues->addQueue (queue);

  //delete text if not needed
  if (!keeptext)
    setText ("");
  //select everything if needed
  if (selectkepttext)
    selectAll ();
}

void cMultiInputLine::keyPressEvent (QKeyEvent *e)
{
  bool ctrl = false;
  if (e->modifiers() & Qt::ControlModifier)
    ctrl = true;
  if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))
  {
    //ENTER pressed, handle it !
    //we send sommands on plain ENTER, or on CTRL+ENTER if we have to swap them ...
    if (ctrl == swapenters)
      sendCommands ();
    else
      //standard behaviour otherwise
      QTextEdit::keyPressEvent (e);
  }
  else
    QTextEdit::keyPressEvent (e);
}

