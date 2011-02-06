//
// C++ Interface: cscriptapi
//
// Description: Scripting API - functions exported to the scripts.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
