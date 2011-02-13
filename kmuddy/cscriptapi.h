//
// C++ Interface: cscriptapi
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

#ifndef CSCRIPTAPI_H
#define CSCRIPTAPI_H

#include <QObject>
#include "cactionbase.h"

class cVariableList;

class cScriptAPI : public QObject, public cActionBase
{
Q_OBJECT
public:  
  cScriptAPI (int sess);
// these need to be slots for the scripting to see them
public slots:
  void message (QString msg);
  void send (QString command);
  void sendraw (QString command);
  QString get (QString name);
  void set (QString name, QString value);
private:
  cVariableList *varList ();
};

#endif  // CSCRIPTAPI_H
