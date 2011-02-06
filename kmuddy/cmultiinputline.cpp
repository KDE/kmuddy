//
// C++ Implementation: cmultiinputline
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmultiinputline.h"

#include "cglobalsettings.h"
#include "ccmdqueue.h"
#include "ccmdqueues.h"

#include <kglobalsettings.h>
#include <QKeyEvent>

//maximum number of lines ...
#define MAXLINES 10

cMultiInputLine::cMultiInputLine (int sess, QWidget *parent)
 : Q3TextEdit(parent), cActionBase ("multiinputline", sess)
{
  setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed);

  setTextFormat (Qt::PlainText);

  setWordWrap (Q3TextEdit::WidgetWidth);
  setWrapPolicy (Q3TextEdit::AtWordOrDocumentBoundary);

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

    setFont (gs->getFont ("input-font"));
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
  setHScrollBarMode (Q3ScrollView::AlwaysOff);
  setVScrollBarMode (Q3ScrollView::AlwaysOn);
  
  //change font
  setFont (KGlobalSettings::fixedFont ()); //default system fixed font
}

void cMultiInputLine::setFont (const QFont &font)
{
  Q3TextEdit::setFont (font);
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
  setLinesHeight (lines ());
}

void cMultiInputLine::sendCommands ()
{
  int pars = paragraphs();

  cCmdQueues *queues = (cCmdQueues *) object ("cmdqueues");
  if (!queues) return;
  // create a command queue with all the commands
  cCmdQueue *queue = new cCmdQueue (sess());
  for (int i = 0; i < pars; i++)
    queue->addCommand (text (i));
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
      Q3TextEdit::keyPressEvent (e);
  }
  else
    Q3TextEdit::keyPressEvent (e);
}

#include "cmultiinputline.moc"
