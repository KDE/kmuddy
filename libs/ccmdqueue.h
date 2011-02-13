//
// C++ Interface: cCmdQueues
//
// Description: one command queue
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

#ifndef CCMDQUEUE_H
#define CCMDQUEUE_H

#include <qstring.h>
#include <list>
#include <map>

#include <kmuddy_export.h>

struct KMUDDY_EXPORT cCmdQueueEntry {
  bool isMacro, canParse;
  QString command, macroName, macroParams;
};

class cConnPrefs;
class cCmdParser;
class cMacro;
class cValue;
class cExecStack;
class cExecStackItem;
class cPattern;

/**
One command queue. Stores, parses and executes commands. Also calls macros.
All sent commands must go through this.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cCmdQueue
{
 public:
  cCmdQueue (int _sess);
  ~cCmdQueue ();
  
  /** is this queue finished ? */
  bool finished ();
  /** is this queue waiting for something ? */
  bool waiting ();
  
  /** Add a new command to the queue, using the given parsing and alias expansion settings.
      Parameter expandAliases is ignored if parsing is false. */
  void addCommand (const QString &command, bool parsing = true, bool expandAliases = true);
  
  /** remove all commands from the queue */
  void deleteAllCommands ();
  
  bool varExists (const QString &name);
  QString getValue (const QString &name);
  /** This returns a pointer to the value, which can them be manipulated. Note that this
  pointer should not be stored anywhere, and only manipulated from within a single function,
  as setting the value using setValue might cause this instance to be deleted. */
  cValue *value (const QString &name);
  void setValue (const QString &name, const QString &value);
  void setValue (const QString &name, const cValue &value);
  void delValue (const QString &name);
  
  /** delete all variables from the queue */
  void deleteAllVariables ();
  
  /** Fill the queue from this pattern.  */
  void fillFromPattern (const cPattern *p);

  /** the execution stack */
  cExecStack *execStack (const QString &name);

  /** add a new proprocessor macro */
  void addPreprocessMacro (cMacro *macro);
  /** remove a preprocessor command */
  void removePreprocessMacro (cMacro *macro);
  /** is this preprocess macro registered ? */
  bool hasPreprocessMacro (cMacro *macro);
 protected:
  /** execute the next command in this queue - protected, so only cCmdQueues can use it */
  void executeNext ();
  /** call preprocessor commands on this queue entry */
  bool preprocess (cCmdQueueEntry *qe);
  
  int sess;
  
  std::list<cCmdQueueEntry *> commands;
  /** local variables */
  std::map<QString, cValue *> variables;
  /** execution stacks */
  std::map<QString, cExecStack *> stacks;
  /** pre-process commands */
  std::list<cMacro *> *preproc;
  
  cCmdParser *parser;

  friend class cCmdQueues;
};


/** cExecStack class - manages the execution stack */

class KMUDDY_EXPORT cExecStack {
 public:
  cExecStack ();
  ~cExecStack ();
  /** adds an item to the exec-stack */
  void push (const cExecStackItem &item);
  /** removes the top item from the exec-stack */
  cExecStackItem pop ();
  /** retrieves the top item of the exec-stack without removing it */
  cExecStackItem top ();
  /** is the stack empty ? */
  bool empty ();
  /** clear the stack */
  void clear ();
  protected:
   std::list<cExecStackItem> stack; 
};

/** cExecstackItem class - one item on the exec stack */

class KMUDDY_EXPORT cExecStackItem {
 public:
  cExecStackItem ();
  virtual ~cExecStackItem ();
  /** attribute of the item */
  int attrib (const QString &name);
  /** set an attribute of the item */
  void setAttrib (const QString &name, int val);
 protected:
  std::map<QString, int> attribs;
};


#endif
