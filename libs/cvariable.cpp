/***************************************************************************
                          cvariable.cpp  -  one variable
                          ------------------------------
    begin                : Po sep 8 2003
    copyright            : (C) 2003-2008 by Tomas Mecir
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

#include "cvariable.h"
#include "cvalue.h"

cVariable::cVariable ()
{
  _name = QString();
  val = nullptr;
}

cVariable::~cVariable ()
{
  delete val;
}

QString cVariable::value ()
{
  if (val) return val->asString ();
  return QString();
}

void cVariable::setValue (const QString &newvalue)
{
  cValue *oldv = val;
  val = new cValue (newvalue);
  delete oldv;
}

void cVariable::setValue (const cValue *v)
{
  cValue *oldv = val;
  val = new cValue (*v);
  delete oldv;
}

