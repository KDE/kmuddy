//
// C++ Implementation: dlgmxpconsole
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

void dlgMXPConsole::createDialog ()
{
  // setInitialSize (QSize (300, 200));
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

#endif  //HAVE_MXP

#include "dlgmxpconsole.moc"
