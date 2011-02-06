//
// C++ Interface: cCmdParser
//
// Description: command parser
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CCMDPARSER_H
#define CCMDPARSER_H

#include <cactionbase.h>
#include <kmuddy_export.h>

#include <qstringlist.h>

/**
The command parser. Parses command separators, speed-walk and similar things. Does not expand variables or internal scripting code.

@author Tomas Mecir
*/
class KMUDDY_EXPORT cCmdParser : public cActionBase
{
 public:
  /** constructor */
  cCmdParser (int sess);
  /** destructor */
  ~cCmdParser ();
  
  /** are we parsing commands ? */
  bool parsing () { return isparsing; };
  /** enable/disable command parsing */
  void setParsing (bool value);

  void setCmdSeparatorString (QString str);
  void setSpeedWalkString (QString str);
  void setMultiCommandString (QString str);
  void setNoParseString (QString str);
  void setMacroString (QString str);

  void setAllowEmptyWalkStr (bool val);
  /** set spaces stripping */
  void setStripSpaces (bool strip) { stripSpaces = strip; };
  /** backslash (\) expansion yes/no */
  void setExpandBackslashes (bool val) { expandbackslashes = val; };
  
  /** parse the command, return list of expanded commands */
  QStringList parse (const QString &command, bool expandAliases = true);
  
  /** Is this a macro call ? Also splits into name and params. */
  bool isMacroCall (const QString &cmd, QString &mname, QString &params);
  
  /** does this command need to be sent as-is ? */
  bool mustSendRaw (const QString &command);
  /** remove the raw flag from the command */
  QString fixRaw (const QString &command);
 protected:
  virtual void eventNothingHandler (QString event, int session);

  /** split text into commands, using command separators and \n */
  QStringList splitIntoCommands (const QString &text);

  QStringList parseCommand (const QString &command, bool expandAliases = true);

  /** mark this command as raw, returning the marked command */
  QString markAsRaw (const QString &command);

  /** if this is a speed-walk command, returns length of speed-walk char
  or string (it returns 0 if it isn't speedwalk) */
  int isSpeedWalkCommand (const QString &command);
  /** if the command is speed-walk, this will process it, starting
  at position <pos> (computed by isSpeedWalkCommand) */
  QStringList expandSpeedWalk (const QString &command, int pos);

  /** handles repeater (multi-command) character. !!! modifies
  parameter command if there is a repeater !!!
  @param command Where to look for the repeater, repeater sequence is
  stripped off the command, if any */
  unsigned int repeater (QString &command);

  /** expand backslashes in the command */
  void expandBackslashes (QString &command);

  QString separstr, walkstr, multistr, noparsestr, macrostr;
  bool isparsing, allowemptywalkstr, stripSpaces, expandbackslashes;
};

#endif
