/***************************************************************************
            cinputline.cpp  -  input line widget
  This file is a part of KMuddy distribution.
              -------------------
  begin                : So Jun 29 2002
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

#include "cinputline.h"

#include "cglobalsettings.h"
// cConsole and cOutput are needed for tab-expansion
#include "cconsole.h"
#include "coutput.h"

#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QKeyEvent>
#include <QTextCursor>
#include <kglobalsettings.h>

cInputLine::cInputLine (int sess, QString objName, QWidget *parent)
: KLineEdit(parent), cActionBase (objName, sess)
{
  connect (this, SIGNAL (returnPressed (const QString &)), this, SLOT (handleEnter (const QString &)));

  //default values for selection
  ss = sl = 0;

  //enable completion
  KCompletion *comp = completionObject();
  comp->setOrder (KCompletion::Weighted);
  comp->setSoundsEnabled (false);
  connect (this, SIGNAL (returnPressed (const QString &)), comp, SLOT (addItem (const QString &)));

  //put some commands to completion to prevent unwanted expansion
  comp->addItem ("n");
  comp->addItem ("w");
  comp->addItem ("s");
  comp->addItem ("e");
  comp->addItem ("nw");
  comp->addItem ("ne");
  comp->addItem ("sw");
  comp->addItem ("se");
  comp->addItem ("d");
  comp->addItem ("u");

  //and the context menu
  menuitems = false;
  lastid = -1;
  menuitems = 0;

  //make Tab completion work
  tabExpanding = false;
  expandPos = 0;
  tabListPos = 0;

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cInputLine::~cInputLine()
{
  removeGlobalEventHandler ("global-settings-changed");
}

void cInputLine::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();

    setFont (gs->getFont ("input-font"));
    keepText (gs->getBool ("keep-text"));
    selectKeptText (keeptext ? gs->getBool ("select-kept") : false);
    setArrowsHistory (gs->getBool ("cursors-browse"));
    setAC (gs->getBool ("auto-completion"));
    setACType (gs->getInt ("auto-completion-type"));
    setTelnetPaste (gs->getBool ("telnet-style-paste"));
    QPalette p = palette ();
    QColor bg = gs->getColor ("color-" + QString::number (gs->getInt ("input-bg-color")));
    QColor fg = gs->getColor ("color-" + QString::number (gs->getInt ("input-fg-color")));
    p.setColor (QPalette::Active, backgroundRole(), bg);
    p.setColor (QPalette::Active, foregroundRole(), fg);
    p.setColor (QPalette::Inactive, backgroundRole(), bg);
    p.setColor (QPalette::Inactive, foregroundRole(), fg);
    setPalette (p);
  }
}

QString cInputLine::actionStringHandler (QString action, int, QString &par1,
const QString &)
{
  if (action == "set-text") {
    setText (par1);
  }

  return QString();
}

void cInputLine::initialize ()
{
  //change colors
  // TODO: full color support
  cGlobalSettings *gs = cGlobalSettings::self();
  QPalette p = palette ();
  p.setColor (QPalette::Active, backgroundRole(), gs->getColor ("color-0"));
  p.setColor (QPalette::Active, foregroundRole(), gs->getColor ("color-11"));
  p.setColor (QPalette::Inactive, backgroundRole(), gs->getColor ("color-0"));
  p.setColor (QPalette::Inactive, foregroundRole(), gs->getColor ("color-11"));
  setPalette (p);

  //change font
  setFont (KGlobalSettings::fixedFont ()); //default system fixed font

  //set defaults
  keeptext = true;
  selectkepttext = true;
  arrowshistory = false;
  useac = false;
  curactype = 0;

  //position in history (for up/down browsing)
  historypos = 0;
}

void cInputLine::keepText (bool value)
{
  keeptext = value;
}

void cInputLine::selectKeptText (bool value)
{
  selectkepttext = value;
}

void cInputLine::setArrowsHistory (bool value)
{
  arrowshistory = value;
}

void cInputLine::addHistory(const QString &text)
{
  if (text.isEmpty()) return;  // do not add empty lines
  // do not update if the same command is sent more than once
  if ((lastid >= 0) && (text == menuitem[lastid])) return;
  if (menuitems < CMDHISTORYSIZE)
    menuitems++;
  lastid = (lastid + 1) % CMDHISTORYSIZE;
  menuitem[lastid] = text;
}

void cInputLine::setAC (bool ac)
{
  useac = ac;
  setACType (curactype);
}

void cInputLine::setACType (int typeofac)
{
  curactype = typeofac;
  if (!useac) {
    setCompletionMode (KGlobalSettings::CompletionNone);
    return;
  }
  KGlobalSettings::Completion comp;
  switch (typeofac) {
    case 1: comp = KGlobalSettings::CompletionMan; break;
    case 2: comp = KGlobalSettings::CompletionPopup; break;
    case 0:
    default: comp = KGlobalSettings::CompletionAuto; break;
  }
  setCompletionMode (comp);
}

void cInputLine::setTelnetPaste (bool tnp)
{
  tnpaste = tnp;
}

void cInputLine::handleEnter (const QString &text)
{
  // send the command
  invokeEvent ("command", sess(), text);

  //history position is 0 again, so that we can use arrows correctly
  historypos = 0;
  //update popup menu
  addHistory (text);
  //delete text if not needed
  if (!keeptext) setText ("");
  //select the whole line if needed
  if (selectkepttext) selectAll ();
}

void cInputLine::keyPressEvent (QKeyEvent *e)
{
  if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))
    returnPressed (text());

  //looks like auto-completion widget receives this event before I do, so
  //I don't have to care about possible conflicts...
  if (arrowshistory)
  {
    if (e->type() == QEvent::KeyPress)
    {
      //if none of the following is pressed
      if ((e->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == 0)
      {
        if (e->key() == Qt::Key_Up)
        //shifting UP in history!
        {
          if (historypos == 0)  // remember what we wrote, if anything
            addHistory (text());
          setText (getHistory (false));
          if (selectkepttext)
            selectAll ();
          e->accept ();
          return;
        }
        if (e->key() == Qt::Key_Down)
        //shifting DOWN in history!
        {
          setText (getHistory (true));
          if (selectkepttext)
            selectAll ();
          e->accept ();
          return;
        }
        //ctrl+v would somehow get by the overloaded paste function, this fixes it.
      } else if ((e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key_V)) {
        paste (QApplication::clipboard()->text());
        return;
      }
    }
  }
  //call parent class' event handler to handle other keys
  KLineEdit::keyPressEvent (e);
}

QString cInputLine::getHistory (bool next)
//returns old text if there's nothing to do
{
  //there's no history - nothing to do!
  if (menuitems == 0)
  return text ();

  //at the bottom - no change
  if (next && (historypos == 0)) return text();
  //at the top - do nothing!
  if ((!next) && (historypos == menuitems)) return text();
  //get requested command's ID
  int id = lastid - historypos + CMDHISTORYSIZE;
  //if we want NEXT:
  if (next) id++;
  //make it fit!
  id %= CMDHISTORYSIZE;
  historypos += (next ? (-1) : 1);

  //OKAY now that we have the ID, we get the command:
  QString cmd = menuitem[id];
  //We may have exactly the same command in the input line.
  //We know that there can't be two same commands next to each other in the history.
  //So we just recursively call this function.
  return (cmd == text()) ? getHistory(next) : cmd;
}

void cInputLine::focusInEvent (QFocusEvent *e)
{
  //restore selection if possible
  if (sl > 0)
  setSelection (ss, sl);

  KLineEdit::focusInEvent (e);
}

void cInputLine::focusOutEvent (QFocusEvent *e)
{
  int sels = selectionStart();
  if (sels >= 0)
  {
    ss = sels;
    sl = selectedText().length();
  }
  else
  ss = sl = 0;
  KLineEdit::focusOutEvent (e);
}

void cInputLine::mouseReleaseEvent (QMouseEvent *e)
{
  if (e->button() == Qt::MidButton)
  {
    deselect ();
    paste (QApplication::clipboard()->text (QClipboard::Selection));
  }
  else
  KLineEdit::mouseReleaseEvent (e);
}

void cInputLine::paste ()
{
  paste (QApplication::clipboard()->text());
}

void cInputLine::paste (const QString &txt)
{
  //This function was provided by Yui Unifex. I've modified it to be
  //better configurable
  QString t = txt;
  if (tnpaste)
  {
    QString line;
    for (int i = 0; i < t.length(); ++i) {
      if (t[i] == '\n') {
        insert (line);
        returnPressed (text());
        line = "";
      }
      else
      line.append(t[i]);
    }

    if (line.length() > 0)
    insert(line);
  }
  else
  {
    //standard paste - replace end-of-lines with spaces and paste
    int l = t.length();
    for (int i = 0; i < l; ++i)
    if (t[i] == '\n')
    t[i] = ' ';
    insert (t);
  }
}

bool cInputLine::event (QEvent *e)
{
  if (!e)
  return true;
  if (e->type() == QEvent::KeyPress)
  {
    QKeyEvent *ke = (QKeyEvent *) e;
    if ((ke->key() == Qt::Key_Tab) || (ke->key() == Qt::Key_Backtab))
    {
      handleTabExpansion ();
      ke->accept ();
      return true;  //event processed
    }
    else
    tabExpanding = false;
  }
  if (e->type() == QEvent::MouseButtonPress)
  tabExpanding = false;  //stop tab expansion if we click the mouse

  return KLineEdit::event (e);;
}

void cInputLine::handleTabExpansion ()
{
  QString t = text();

  if (!tabExpanding)
  {
    int cursorPos = cursorPosition () - 1;
    if (cursorPos <= 0)
    return;  //do nothing if we're too close to the start of line

    //find position of the start of a word
    expandPos = t.lastIndexOf (' ', cursorPos) + 1;
    if (expandPos == cursorPos + 1)
    return;  //do nothing if we're at a space

    //find prefix and get list of all matching words
    QString prefix = t.mid (expandPos, cursorPos - expandPos + 1);
    int prefixLen = prefix.length();
    if (prefixLen < 2)
    return;
    cOutput *output = dynamic_cast<cOutput *>(object ("output"));
    tabWords = output->console()->words (prefix);
    if (tabWords.count() == 0)
    return;  //do nothing if we fail to find any such word

    //initialize the expansion
    tabListPos = tabWords.count()-1;  // Set the position to the last occuranc of the prefix (ML 051006)

    //delete the prefix
    t.remove (expandPos, prefixLen);

  }
  else
  {
    //if we have expanded something previously, we need to delete it now (including prefix)
    int wlen = tabWords[tabListPos].length();
    t.remove (expandPos, wlen);  //length of that word, not including a trailing space

    //update position in the list
    tabListPos--;
    if (tabListPos < 0)
    tabListPos = tabWords.count() - 1;
  }

  //if we are here, we expand ...
  tabExpanding = true;
  t.insert (expandPos, tabWords[tabListPos]);
  setText (t);
  setCursorPosition (expandPos + tabWords[tabListPos].length());
}

#include "cinputline.moc"

