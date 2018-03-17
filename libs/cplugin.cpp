//
// C++ Implementation: cplugin
//
// Description: one plug-in
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

