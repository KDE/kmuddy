//
// C++ Implementation: dlgmxpconsole
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

#include "dlgmxpconsole.h"

#ifdef HAVE_MXP

#include <klocale.h>
#include <QScrollBar>
#include <QTextEdit>

dlgMXPConsole::dlgMXPConsole (QWidget *parent) : QDockWidget (parent)
{
  createDialog ();
}


dlgMXPConsole::~dlgMXPConsole ()
{
}

QSize dlgMXPConsole::sizeHint() const
{
  return QSize (500, 300);
}

void dlgMXPConsole::createDialog ()
{
  setWindowTitle (i18n ("MXP Console"));

  viewer = new QTextEdit (this);
  viewer->setAcceptRichText (false);
  viewer->setReadOnly (true);
  
  setWidget (viewer);
  
  //no focus - we don't want this dialog to get focus
  setFocusPolicy (Qt::NoFocus);
  viewer->setFocusPolicy (Qt::NoFocus);
}

void dlgMXPConsole::addLine (const QString &line)
{
  viewer->append (line);
  QScrollBar *sb = viewer->verticalScrollBar();
  sb->setValue (sb->maximum ());
}

#include "moc_dlgmxpconsole.cpp"

#endif  //HAVE_MXP
