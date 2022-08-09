/***************************************************************************
                          ccmdprocessor.h  -  command processor
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pi Jul 5 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#ifndef CCMDPROCESSOR_H
#define CCMDPROCESSOR_H

#include <cactionbase.h>
#include <cvalue.h>
#include <kmuddy_export.h>

class cCmdQueue;
class cExpCache;
class cExpResolver;

/**
class cCmdProcessor expands internal scripting, variables, and also handles
 * macro calls by passing them to the cMacroManager class.

  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cCmdProcessor : public cActionBase {
public: 
  cCmdProcessor (int sess);
  ~cCmdProcessor();

  void setFocusCommandString (QString str);
  
  void processCommand (const QString &command, cCmdQueue *queue);
  /** Evaluate macro <mname> with parameters <params> */
  void processMacro (const QString &name, const QString &params, cCmdQueue *queue);
  
  /** expand internal scripting sequences in a command */
  void expandInternalScripting (QString &command, cCmdQueue *queue = nullptr);
  /** expand variables in a command */
  void expandVariables (QString &command, cCmdQueue *queue = nullptr);

  cValue eval (const QString &exp, cCmdQueue *queue, bool &ok);
protected:
  virtual void eventNothingHandler (QString event, int session) override;

  /** Looks to see if this is a tab/window command call. If it is, the command
  is split into :window:mytext */
  int isFocusCommand (const QString &command);
  /** Send <command> to <window> */
  int processFocusCommand (const QString &command, int pos);
  
  QString focusstr;
  
  // variable and function resolver, used in internal scripting
  cExpResolver *resolver;
  cExpCache *expcache;
};

  
#endif
