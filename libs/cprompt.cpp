//
//
// C++ Implementation: cprompt
//
// Description:
//
/*
Copyright 2003-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

