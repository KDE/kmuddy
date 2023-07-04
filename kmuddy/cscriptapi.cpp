//
// C++ Implementation: cscriptapi
//
// Description: Scripting API - functions exported to the scripts.
//
/*
Copyright 2010-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cscriptapi.h"
#include "cactionmanager.h"
#include "cvariablelist.h"

cScriptAPI::cScriptAPI (int sess) : cActionBase ("scriptapi", sess)
{
}

void cScriptAPI::message (QString msg)
{
  invokeEvent ("message", sess(), msg);
}

void cScriptAPI::send (QString command)
{
  invokeEvent ("command", sess(), command);
}

void cScriptAPI::sendraw (QString command)
{
  invokeEvent ("send-command", sess(), command);
}

QString cScriptAPI::get (QString name)
{
  return varList()->getValue (name);
}

void cScriptAPI::set (QString name, QString value)
{
  varList()->set (name, value);
}

cVariableList *cScriptAPI::varList ()
{
  cActionManager *am = cActionManager::self();
  return dynamic_cast<cVariableList *>(am->object ("variables", sess()));
}

#include "moc_cscriptapi.cpp"
