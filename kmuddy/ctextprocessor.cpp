/***************************************************************************
                          ctextprocessor.cpp  -  text processor
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po oct 14 2002
    copyright            : (C) 2002-2003 by Tomas Mecir
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

#ifndef QT_CLEAN_NAMESPACE
#define QT_CLEAN_NAMESPACE
#endif

#include <config-mxp.h>

#include "ctextprocessor.h"

#include "cansiparser.h"
#include "cconsole.h"
#include "clistmanager.h"
#include "coutput.h"
#include "ctextchunk.h"
#include "ctriggerlist.h"
#include "cvariablelist.h"
#include "cwindowlist.h"

#ifdef HAVE_MXP
#include "cmxpmanager.h"
#endif //HAVE_MXP

#include <knotification.h>
#include <qtimer.h>

cTextProcessor::cTextProcessor (int sess) : cActionBase ("textproc", sess)
{
  chunk = nullptr;

  flush ();
  gotprompt = false;

  //some defaults...
  fg = Qt::gray;
  bg = Qt::black;
  attrib = 0;

  elapsedticks = 0;
  pdtimer = new QTimer;
  connect (pdtimer, SIGNAL (timeout ()), this, SLOT (timeout ()));
  pdtimer->start (250);
  
  // some events ahd stuff ...
  addEventHandler ("data-received", 50, PT_STRING);
  addEventHandler ("received-ga", 50, PT_NOTHING);
  addEventHandler ("connected", 100, PT_NOTHING);
  addEventHandler ("command-sent", 50, PT_STRING);
  
  // Both ANSI parser and MXP parser must already exist when this is being created
  // That is due to the following code ...

  cANSIParser *ap = dynamic_cast<cANSIParser *>(object ("ansiparser", sess));
#ifdef HAVE_MXP
  cMXPManager *mxpmanager = dynamic_cast<cMXPManager *>(object ("mxpmanager", sess));
  connect (mxpmanager, SIGNAL (gotNewText (const QString &)),
      ap, SLOT (parseText (const QString &)));
  connect (mxpmanager, SIGNAL (gotNewLine ()), this, SLOT (gotNewLine ()));
  connect (mxpmanager, SIGNAL (gotFgColor (QColor)), this, SLOT (gotFgColor (QColor)));
  connect (mxpmanager, SIGNAL (gotBgColor (QColor)), this, SLOT (gotBgColor (QColor)));
  connect (mxpmanager, SIGNAL (gotAttrib (int)), this, SLOT (gotAttrib (int)));
  connect (mxpmanager, SIGNAL (gotALink (const QString &, const QString &, const QString &,
      const QString &)), this, SLOT (gotALink (const QString &, const QString &,
      const QString &, const QString &)));
  connect (mxpmanager, SIGNAL (gotSENDLink (const QString &, const QString &, const QString &,
      const QString &, bool, bool)), this, SLOT (gotSENDLink (const QString &,
      const QString &, const QString &, const QString &, bool, bool)));
  connect (mxpmanager, SIGNAL (gotExpire (const QString &)), this,
      SLOT (gotExpire (const QString &)));
  
#endif
  connect (this, SIGNAL (plainText (const QString &)), ap, SLOT (parseText (const QString &)));
  connect (ap, SIGNAL (fgColor (QColor)), this, SLOT (gotFgColor (QColor)));
  connect (ap, SIGNAL (bgColor (QColor)), this, SLOT (gotBgColor (QColor)));
  connect (ap, SIGNAL (attrib (int)), this, SLOT (gotAttrib (int)));
  connect (ap, SIGNAL (plainText (const QString &)), this, SLOT (gotNewText (const QString &)));
}

cTextProcessor::~cTextProcessor ()
{
  removeEventHandler ("data-received");
  removeEventHandler ("received-ga");
  removeEventHandler ("connected");
  removeEventHandler ("command-sent");

  pdtimer->stop ();
  delete pdtimer;
}

void cTextProcessor::eventStringHandler (QString event, int, QString &par1,
    const QString &)
{
  if (event == "data-received")
    parseText (par1);
  else if (event == "command-sent")
    sendingCommand ();
}

void cTextProcessor::eventNothingHandler (QString event, int)
{
  if (event == "received-ga")
    receivedGA ();
  else if (event == "connected")
    flush ();
  else if (event == "text-here")
    moreTextHere ();
}

void cTextProcessor::parseText (const QString &text)
{
  // TODO: Both MXP and ANSI parsing should be done via events, not like this ...
  // This is so that we can easily add new protocols and so on ...
#ifdef HAVE_MXP
  //if MXP is on, pass the text to MXP parser...
  cMXPManager *mxpmanager = dynamic_cast<cMXPManager *>(object ("mxpmanager", sess()));
  if (mxpmanager->isMXPActive ())
  {
    mxpmanager->processText (text);
    return;
  }
#endif
  //this will only be run if MXP is off...
  QString t;
  int l = text.length ();
  for (int i = 0; i < l; i++)
  {
    QChar ch = text[i];
    if (ch == '\n')
    {
      if (t.length())
        emit plainText (t);
      gotNewLine ();
      t = "";
    }
    else
      t += text[i];
  }

  if (t.length())
    emit plainText (t);
}

void cTextProcessor::sendingCommand ()
{
  //we can receive a prompt again, even if no newline arrives
  //this is to support things like 'Name:', then 'Password:', but
  //no newline between them (as that comes from the user)
  gotprompt = false;
}

void cTextProcessor::gotNewText (const QString &text)
{
  //if we already have a prompt, then this will go to a new line
  //this is to prevent multiple prompts on one line and similar things
  // *** CODE DISABLED FOR NOW ***
  //if (gotprompt)
  //  gotNewLine ();

  if (chunk == nullptr)
    createChunk ();

  //reset tick-counter
  elapsedticks = 0;

  int len = text.length();
  QString txt;
  for (int i = 0; i < len; i++)
  {
    if (text[i] == '\t')  //TAB character
      //add 8 spaces - not perfect, but hey ;)
      for (int i = 0; i < 8; i++)
        txt += QChar (' ');
    else if (text[i] == '\a')  //beep
      KNotification::beep ();
    else
      txt += text[i];
  }

  //create the text chunk an'all :)
  chunkText *cht = new chunkText;
  cht->setText (txt);
  chunk->appendEntry (cht);

  //prompt detection (only if no prompt has been recvd since end of line)
  if ((!txt.isEmpty()) && (!gotprompt))
  {
    haveprompt = false;

    cList *tl = cListManager::self()->getList (sess(), "triggers");
    cTriggerList *triggers = tl ? dynamic_cast<cTriggerList *>(tl) : nullptr;
    if (triggers)
    {
      //only prompt-detecting triggers will be executed here
      triggers->setDetectingPrompt (true);
      triggers->matchString (chunk);
    }
    if (haveprompt)
      //if triggers say that this is a prompt, then we behave as if
      //telnet-GA was received
      receivedGA ();
  }
}

void cTextProcessor::gotFgColor (QColor color)
{
  if (chunk == nullptr)
    createChunk ();

  chunkFg *chfg = new chunkFg;
  chfg->setFg (color);
  chunk->appendEntry (chfg);

  //remember the color...
  fg = color;
}

void cTextProcessor::gotBgColor (QColor color)
{
  if (chunk == nullptr)
    createChunk ();

  chunkBg *chbg = new chunkBg;
  chbg->setBg (color);
  chunk->appendEntry (chbg);

  //remember the color...
  bg = color;
}

void cTextProcessor::gotAttrib (int a)
{
  if (chunk == nullptr)
    createChunk ();

  chunkAttrib *chatt = new chunkAttrib;
  chatt->setAttrib (a);
  chunk->appendEntry (chatt);

  //remember the attribute...
  attrib = a;
}

void cTextProcessor::gotALink (const QString &name, const QString &url, const QString &text,
    const QString &hint)
{
  if (chunk == nullptr)
    createChunk ();

  chunkLink *chl = new chunkLink;
  chl->setIsCommand (false);
  chl->setIsMenu (false);
  chl->setName (name);
  chl->setTarget (url);
  chl->setText (text);
  chl->setHint (hint);
  
  chunk->appendEntry (chl);
}

void cTextProcessor::gotSENDLink (const QString &name, const QString &command,
    const QString &text, const QString &hint, bool toprompt, bool ismenu)
{
  if (chunk == nullptr)
    createChunk ();

  chunkLink *chl = new chunkLink;
  chl->setIsCommand (true);
  chl->setName (name);
  chl->setTarget (command);
  chl->setText (text);
  chl->setHint (hint);
  chl->setToPrompt (toprompt);
  chl->setIsMenu (ismenu);
  
  //parse menu information of needed
  if (ismenu)
    chl->parseMenu();

  chunk->appendEntry (chl);
}

void cTextProcessor::gotExpire (const QString &name)
{
  cConsole *console = (dynamic_cast<cOutput *>(object ("output")))->console();
  console->expireNamedLinks (name);
}

void cTextProcessor::createChunk ()
{
  chunk = new cTextChunk ((dynamic_cast<cOutput *>(object ("output")))->console());
  chunkStart startattr;
  startattr.startpos = 0;
  startattr.fg = fg;
  startattr.bg = bg;
  startattr.attrib = attrib;
  chunk->setStartAttr (startattr);
}

void cTextProcessor::gotNewLine ()
{
  if (!chunk)  //empty line - create an empty one
    createChunk ();

  //inform plug-ins - phase 1 (before triggers)
  invokeEvent ("got-line", sess(), chunk);

  //is a gagging trigger gets activated, it will call gagLine(), which will
  //set this flag
  gag = false;
  //the same for primary gag, which is set by window-redirection triggers
  primarygag = false;
  //also clear window name
  wname = QString();

  //first call trigger comparison/execution...

  cList *tl = cListManager::self()->getList (sess(), "triggers");
  cTriggerList *triggers = tl ? dynamic_cast<cTriggerList *>(tl) : nullptr;
  if (triggers)
  {
    triggers->setDetectingPrompt (false);
    triggers->matchString (chunk);
  }

  //inform plug-ins - phase 2 (after triggers)
  if (gag) invokeEvent ("will-gag", sess());
  invokeEvent ("got-line-after-triggers", sess(), chunk);

  QString plainText = chunk->plainText();

  //now emit some events, if the line shouldn't be gagged...
  //the cOutput class is connected here, amongst others
  if (!gag)
  {
    if (!wname.isEmpty())
    {
      cWindowList *wlist = dynamic_cast<cWindowList *>(object ("windowlist"));
      if (wlist) {
        //send the text to the output window, if it exists
        if (wlist->exists (wname))
          wlist->textToWindow (wname, chunk);
        else
          //if the window doesn't exist, display the text in the main window, even if it would
          //normally be gagged
          //this is to prevent loss of information in case that the user removes some window,
          //but keeps triggers that use that window
          primarygag = false;
      }
    }
    if (!primarygag)
      invokeEvent ("display-line", sess(), chunk);
  }
  //got-line is emitted even if line is gagged - this means that scripts will receive the line
  //even if it is gagged...
  invokeEvent ("got-line", sess(), plainText);

  delete chunk;
  chunk = nullptr;

  //increase line counter (even if line was gagged), and set $line variable in variableList
  lines++;

  cVariableList *vl = dynamic_cast<cVariableList *>(object ("variables"));
  if (vl) {
    QString str_lines;
    str_lines.setNum(lines);
    vl->set (QString("line"), str_lines);
  }

  //we can receive prompt again
  gotprompt = false;
}

void cTextProcessor::flush ()
{
  lines = 0;

  delete chunk;
  chunk = nullptr;

  gag = primarygag = false;
  haveprompt = gotprompt = false;
}

void cTextProcessor::receivedGA ()
{
  if (!gotprompt)  //avoid 2+ prompts on one line
  {
    if (chunk)
    {
      invokeEvent ("got-prompt", sess(), chunk->plainText());
      invokeEvent ("got-prompt", sess(), chunk);

      cList *tl = cListManager::self()->getList (sess(), "triggers");
      cTriggerList *triggers = tl ? dynamic_cast<cTriggerList *>(tl) : nullptr;
      if (triggers)
      {
        triggers->setDetectingPrompt (false);
        triggers->matchString (chunk);
        invokeEvent ("got-prompt-after-triggers", sess(), chunk);
      }
      invokeEvent ("display-prompt", sess(), chunk);
    
      delete chunk;
      chunk = nullptr;
    }

    gotprompt = true;

    //increase line counter (even if line was gagged), and set $line variable in variableList
    lines++;

    cVariableList *vl = dynamic_cast<cVariableList *>(object ("variables"));
    if (vl)
    {
      QString str_lines;
      str_lines.setNum(lines);
      vl->set (QString("line"), str_lines);
    }
  }
}

void cTextProcessor::moreTextHere ()
{
  if (chunk && !chunk->entries().empty())
   invokeEvent ("partial-line", sess(), chunk->plainText());
}

void cTextProcessor::recolorize (list<colorChange> &changes)
{
  if (chunk)
    chunk->applyColorChanges (changes);
}

void cTextProcessor::timeout ()
{
  //If some part of the line is waiting, and no prompt has been received
  //so far, we increase our counter. If the counter reaches 4 and no more
  //text arrives, we assume that the waiting text is a prompt and we
  //treat it as such.

  if ((!gotprompt) && chunk && (!chunk->plainText().isEmpty()))
    elapsedticks++;
  if (elapsedticks == 4)  //it's probably a prompt
    receivedGA ();
}

#include "moc_ctextprocessor.cpp"
