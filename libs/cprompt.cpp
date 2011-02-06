//
//
// C++ Implementation: cprompt
//
// Description:
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cprompt.h"
#include "cprofilesettings.h"

cPrompt::cPrompt(int sess, QWidget *parent) :
  QLabel (parent), cActionBase ("prompt", sess)
{

  addEventHandler ("got-prompt", 50, PT_STRING);
  addEventHandler ("settings-changed", 50, PT_NOTHING);
}

cPrompt::~cPrompt()
{
  removeEventHandler ("got-prompt");
  removeEventHandler ("settings-changed");
}

void cPrompt::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "got-prompt")
    updatePrompt (par1);
}

void cPrompt::eventNothingHandler (QString event, int)
{
  if (event == "settings-changed") {
    // show/hide this widget based on whether we want the prompt label or not
    settings()->getBool ("prompt-label") ? show() : hide();
  }
}

void cPrompt::updatePrompt(const QString &text)
{
  setText (text);
}

#include "cprompt.moc"

