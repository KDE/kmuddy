//
// C++ Implementation: cscriptapi
//
// Description: Scripting API - functions exported to the scripts.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

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


#include "cscriptapi.moc"
