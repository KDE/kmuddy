/***************************************************************************
                          cvariablelist.h  -  list of variables
                             -------------------
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

#ifndef CVARIABLELIST_H
#define CVARIABLELIST_H

#include <map>

#include <cactionbase.h>
#include <kmuddy_export.h>

class cCmdQueue;
class cVariable;
class cValue;
/**
This class represents the list of variables.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cVariableList : public cActionBase {
public:
  cVariableList (int sess);
  ~cVariableList ();
  QString getValue (const QString &varname);
  int getIntValue (const QString &varname);
  /** Retrieve a variable value. If queue is given, attempts to get local
  variable value first, if one exists. */
  cValue *value (const QString &varname, cCmdQueue *queue = nullptr);
  /** as above, but if value doesn't exist, it gets created */
  cValue *valueNotEmpty (const QString &varname, cCmdQueue *queue = nullptr);
  bool exists (const QString &varname);
  void set (const QString &varname, const QString &value);
  void set (const QString &varname, cValue *value);
  void unset (const QString &varname);
  /** increases value by delta */
  void inc (const QString &varname, double delta);
  /** decreases value by delta; value can become negative */
  void dec (const QString &varname, double delta);
  /** resource support; provides one unit of a resource */
  void provideResource (const QString &varname);
  /** resource support; requests one resource, if available */
  bool requestResource (const QString &varname);
  QStringList getList ();
  /** expand all variables in the string and return the result */
  QString expandVariables (const QString &string, bool recursive = true, cCmdQueue *queue = nullptr);

  void save ();
private:
  void load ();

  QString doExpandVariables (const QString &string, int recursionCounter, cCmdQueue *queue);
  QString processVariable (const QString &varname, cCmdQueue *queue = nullptr);
  /** variable mapping, used to store the variables */
  std::map<QString, cVariable *> vars;
};

#endif
