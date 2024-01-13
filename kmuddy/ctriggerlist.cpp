//
// C++ Implementation: ctriggerlist
//
// Description: 
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "ctriggerlist.h"

#include "cactionmanager.h"
#include "ccmdqueue.h"
#include "ccmdqueues.h"
#include "cpattern.h"
#include "ctextchunk.h"
#include "ctrigger.h"
#include "ctriggereditor.h"

#include <kmessagebox.h>
#include <KLocalizedString>

//used in loop detection
#define MAXTRIGGERS 50

struct cTriggerList::Private {
  bool detectingPrompt;

  cTextChunk *curline;
  QString plainText;
};

cTriggerList::cTriggerList ()
  : cList ("triggers")
{
  // create properties
  // basic ones
  addStringProperty ("pattern", "Matching pattern");
  addIntProperty ("newtext-count", "Replacement line count", 0);
  // then we have a "newtext-"+i string for each
  addIntProperty ("matching", "Comparison type", int(cPattern::substring));
  addStringProperty ("condition", "Matching condition");

  // script
  addStringProperty ("script", "Script to execute");

  // options
  addBoolProperty ("cs", "Case sensitive", true);
  addBoolProperty ("dont-send", "Don't send", false);
  addBoolProperty ("whole-words", "Whole words", true);
  addBoolProperty ("global", "Global matching", false);
  addIntProperty ("action-matched", "Action if matched", (int) cList::Stop);
  addIntProperty ("action-not-matched", "Action if not matched", (int) cList::Continue);

  // colorizations
  addBoolProperty ("colorize", "Color trigger", false);
  addIntProperty ("colorize-count", "Colorization count", 0);
  // then we have a "colorize-variable-"+i - which variable is affected
  // "colorize-fg-"+i - foreground color
  // "colorize-bg-"+i - background color
  // the FG and BG numbers: positive means a RGB value (subtract 1 to get real value), negative means a color from the ANSI set (add 16 to get the real color), 0 means no change

  // special triggers
  addBoolProperty ("rewrite", "Rewrite trigger", false);
  addStringProperty ("rewrite-var", "Rewrite variable");
  addStringProperty ("rewrite-text", "Rewrite new text");

  addBoolProperty ("gag", "Gag trigger", false);
  addBoolProperty ("notify", "Notify trigger", false);
  addBoolProperty ("prompt", "Prompt detection trigger", false);
  addBoolProperty ("sound", "Sound trigger", false);
  addStringProperty ("sound-file", "Sound file name");

  addBoolProperty ("output-window", "Send output to separate window", false);
  addBoolProperty ("output-gag-in-main", "Gag main window in favour of output", false);
  addStringProperty ("output-window-name", "Output window name");
  
  d = new Private;

  d->detectingPrompt = false;
  d->curline = nullptr;
}

cTriggerList::~cTriggerList ()
{
  delete d;
}

cListObject *cTriggerList::newObject ()
{
  return new cTrigger (this);
}

cListEditor *cTriggerList::editor (QWidget *parent)
{
  return new cTriggerEditor (parent);
}

void cTriggerList::matchString (cTextChunk *line)
{
  d->curline = line;
  d->plainText = d->curline->plainText();

  traverse (TRIGGER_MATCH);

  d->curline = nullptr;
}

bool cTriggerList::detectingPrompt ()
{
  return d->detectingPrompt;
}

cTextChunk *cTriggerList::lineToMatch ()
{
  return d->curline;
}

QString cTriggerList::stringToMatch ()
{
  return d->plainText;
}


void cTriggerList::setDetectingPrompt (bool val)
{
  d->detectingPrompt = val;
}

void cTriggerList::processCommands (const QStringList &commands)
{
  if (commands.isEmpty())  //do nothing if there are no commands
    return;

  //okay, send the commands!
  int sess = session();
  cCmdQueues *queues = (cCmdQueues *) cActionManager::self()->object ("cmdqueues", sess);
  if (!queues) return;
  cCmdQueue *queue = new cCmdQueue (sess);
  QStringList::const_iterator it;
  for (it = commands.begin(); it != commands.end(); ++it)
    queue->addCommand (*it);
  queues->addQueue (queue);
}

void cTriggerList::rewriteText (int pos, int len, const QString &newtext)
{
  //perform the actual replace
  d->curline->replace (pos, len, newtext);
  
  //plainText has been changed - regenerate it
  d->plainText = d->curline->plainText();
}

