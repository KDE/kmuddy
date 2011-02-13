//
// C++ Interface: cMacroManager, cMacro, cFunction
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CMACROMANAGER_H
#define CMACROMANAGER_H

#include <cactionbase.h>

#include <cvalue.h>
#include <list>
#include <kmuddy_export.h>

class cActionManager;
class cFunction;
class cMacro;
class cVariableList;
class cCmdQueue;
class cCmdQueueEntry;
struct cMacroManagerPrivate;

/**
This is the macro manager. It manages and calls macros and functions.
It follows the Singleton pattern.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cMacroManager : public cActionBase
{
public:
  /** return an instance */
  static cMacroManager *self ();
  /** destructor */
  ~cMacroManager ();

  void addMacro (const QString &name, cMacro *macro);
  void removeMacro (const QString &name);
  /** return the macro with this name */
  cMacro *macro (const QString &name);
  /** Call a given macro with given parameters. May optionally accept a session number, and
  a pointer to the current command stack. */
  bool callMacro (const QString &name, const QString &par, int sess = -1, cCmdQueue *queue = 0);
  void addFunction (const QString &name, cFunction *function);
  void removeFunction (const QString &name);
  /** Call a given function with given parameters. May optionally accept a session number, and
  a pointer to the current command stack. */
  cValue callFunction (const QString &name, std::list<cValue> &params, int sess = -1,
      cCmdQueue *queue = 0);
  bool functionExists (const QString &name);
private:
  /** constructor */
  cMacroManager ();
  static cMacroManager *_self;

  cMacroManagerPrivate *d;  
};

/** The cMacro class represents one macro. */
class KMUDDY_EXPORT cMacro {
 public:
  cMacro (const QString &name);
  virtual ~cMacro ();
  virtual void eval (const QString &params, int sess = -1, cCmdQueue *queue = 0) = 0;
  virtual bool preprocess (cCmdQueue *, cCmdQueueEntry *) { return true; };
 protected:
  QString n;
  // just some convenience aliases ...
  cActionManager *am;
  cVariableList *varList (int sess);
  QString expandVariables (const QString &str, int sess, cCmdQueue *queue);
};

/** The cFunction class represents one function. */
class KMUDDY_EXPORT cFunction {
 public:
  cFunction (const QString &name);
  virtual ~cFunction ();
  virtual cValue eval (std::list<cValue> &params, int sess = -1, cCmdQueue *queue = 0) = 0;
 protected:
  QString n;
  // just some convenience aliases ...
  cActionManager *am;
  cVariableList *varList (int sess);
};

#endif
