/***************************************************************************
                          cscript.h  -  one script
                             -------------------
    begin                : So dec 7 2002
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

#ifndef CSCRIPT_H
#define CSCRIPT_H

#include "csaveablefield.h"

class cRunningScript;

/**
Information about one script.
  *@author Tomas Mecir
  */

class cScript : public cSaveableField  {
public: 
  cScript (int _sess);
  ~cScript () override;

  cSaveableField *newInstance () override;

  /** load data from a config file */
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_SCRIPT; };

  /** get* methods */
  QString getName () { return name; };
  QString getComment () { return comment; };
  QString getCommand () { return command; };
  QString getWorkDir () { return workdir; };
  QString getPrefix () { return prefix; };
  QString getSuffix () { return suffix; };
  bool getEnableInput () { return enableinput; };
  bool getEnableOutput () { return enablestdout; };
  bool getSendOutput () { return sendstdout; };
  bool getEnableError () { return enablestderr; }
  bool getSendError () { return sendstderr; }
  bool getSendUserCommands () { return sendusercommands; };
    /** prepends every line with line type (command, server output, ...) */
  bool getUseAdvComm () { return useadvcomm; };
  bool getAllowParams () { return allowparams; };
  bool getSingleInstance () { return singleinstance; };
  bool getShellExpansion () { return shellexpansion; };
  bool getOnlyIfMatch () { return onlyifmatch; };
  bool getNoFlowControl () { return noflowcontrol; };
  bool getAllowVars () { return allowvars; };
  
  /** set* methods */
  void setName (const QString &str) { name = str; };
  void setComment (const QString &str) { comment = str; };
  void setCommand (const QString &str) { command = str; };
  void setWorkDir (const QString &str) { workdir = str; };
  void setPrefix (const QString &str) { prefix = str; };
  void setSuffix (const QString &str) { suffix = str; };
  void setEnableInput (bool b) { enableinput = b; };
  void setEnableOutput (bool b) { enablestdout= b; };
  void setSendOutput (bool b) { sendstdout= b; };
  void setEnableError (bool b) { enablestderr = b; };
  void setSendError (bool b) { sendstderr = b; };
  void setSendUserCommands (bool b) { sendusercommands = b; };
    /** prepends every line with line type (command, server output, ...) */
  void setUseAdvComm (bool b) { useadvcomm = b; };
  void setAllowParams (bool b) { allowparams = b; };
  void setSingleInstance (bool b) { singleinstance = b; };
  void setShellExpansion (bool b) { shellexpansion = b; };
  void setOnlyIfMatch (bool b) { onlyifmatch = b; };
  void setNoFlowControl (bool b) { noflowcontrol = b; };
  void setAllowVars (bool b) { allowvars = b; };
protected:
  /** some storeable variables */
  QString name, comment;
  QString command, workdir;
  QString prefix, suffix;
  bool enableinput, enablestdout, sendstdout, enablestderr, sendstderr;
  bool sendusercommands;
    /** prepends every line with line type (command, server output, ...) */
  bool useadvcomm;
  /** disable flow control? */
  bool noflowcontrol;
  /** is variable communication allowed? */
  bool allowvars;
  bool allowparams;
  bool singleinstance;
  bool shellexpansion;
  /** if there's some limiting text, this says if it must or mustn't pass */
  bool onlyifmatch;

  int sess;
};

#endif

