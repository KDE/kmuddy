/***************************************************************************
                          cmultilineinput.cpp  -  multi line input handler
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Aug 26 2002
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

#include "cmultilineinput.h"

#include "cactionmanager.h"
#include "ccmdqueue.h"
#include "ccmdqueues.h"
#include "cglobalsettings.h"
#include "csessionmanager.h"
#include "dialogs/dlgmultiline.h"

#include <kmainwindow.h>

cMultiLineInput *cMultiLineInput::_self = nullptr;

cMultiLineInput *cMultiLineInput::self ()
{
  if (!_self)
    _self = new cMultiLineInput;
  return _self;
}

cMultiLineInput::cMultiLineInput () : cActionBase ("multi-line-input", 0)
{
  init ();
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cMultiLineInput::~cMultiLineInput ()
{
  removeGlobalEventHandler ("global-settings-changed");
  delete multiline;
  _self = nullptr;
}

QDockWidget *cMultiLineInput::dialog ()
{
  return multiline;
}

void cMultiLineInput::eventNothingHandler (QString event, int)
{
  if (event == "global-settings-changed") {
    setFont (cGlobalSettings::self()->getFont ("multi-line-font"));
  }
}

void cMultiLineInput::init ()
{
  KMainWindow *wnd = cActionManager::self()->mainWindow();
  multiline = new dlgMultiLine (wnd);
  multiline->hide();
  multiline->setObjectName ("multiline");
  wnd->addDockWidget (Qt::BottomDockWidgetArea, multiline);
  multiline->setFloating (true);
  connect (multiline, SIGNAL (commands (const QStringList &)), this, SLOT (sendInput (const QStringList &)));
}

void cMultiLineInput::sendInput (const QStringList &list)
{
  cActionManager *am = cActionManager::self();
  int asess = cSessionManager::self()->activeSession();
  cCmdQueues *queues = (cCmdQueues *) am->object ("cmdqueues", asess);
  if (!queues) return;
  // create a command queue with all the commands
  cCmdQueue *queue = new cCmdQueue (asess);
  QStringList::const_iterator it;
  for (it = list.begin(); it != list.end(); ++it)
    queue->addCommand (*it);
  queues->addQueue (queue);
}

void cMultiLineInput::setFont (QFont font)
{
  if (multiline) multiline->setFont (font);
}

#include "moc_cmultilineinput.cpp"
