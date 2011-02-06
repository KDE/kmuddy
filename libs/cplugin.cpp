//
// C++ Implementation: cplugin
//
// Description: one plug-in
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cplugin.h"

#include "cactionmanager.h"

cPlugin::cPlugin ()
{
  _priority = 50;
}

cPlugin::~cPlugin()
{
}

void cPlugin::systemMessage (int sess, const QString &message)
{
  cActionManager::self()->invokeEvent ("message", sess, message);
}

void cPlugin::sendCommand (int sess, const QString &command)
{
  cActionManager::self()->invokeEvent ("command", sess, command);
}

#include "cplugin.moc"
