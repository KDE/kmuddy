//
// C++ Implementation: cCmdParser
//
// Description: command parser
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

// TODO: parts of this are rather ugly, we need to redesign the whole parsing

#include "ccmdparser.h"

#include "caliaslist.h"
#include "cglobalsettings.h"
#include "clistmanager.h"
#include "cprofilesettings.h"

#include <KLocalizedString>

cCmdParser::cCmdParser (int sess)
  : cActionBase ("cmdparser", sess)
{
  separstr = ";";
  walkstr = ".";
  multistr = "#";
  macrostr = "/";
  noparsestr = "'";

  isparsing = true;
  allowemptywalkstr = false;
  stripSpaces = false;
  expandbackslashes = true;

  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}

cCmdParser::~cCmdParser()
{
  removeGlobalEventHandler ("global-settings-changed");
}

void cCmdParser::eventNothingHandler (QString event, int /*session*/)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    setStripSpaces (gs->getBool ("trim-spaces"));
    setCmdSeparatorString (gs->getString ("str-separator"));
    setSpeedWalkString (gs->getString ("str-speedwalk"));
    setMacroString (gs->getString ("str-macro"));
    setMultiCommandString (gs->getString ("str-multi"));
    setNoParseString (gs->getString ("str-noparse"));
    setAllowEmptyWalkStr (gs->getBool ("empty-walk"));
    setExpandBackslashes (gs->getBool ("expand-backslashes"));
  }
}

void cCmdParser::setCmdSeparatorString (QString str)
{
  separstr = str.trimmed();
}

void cCmdParser::setMultiCommandString (QString str)
{
  multistr = str.trimmed();
}

void cCmdParser::setSpeedWalkString (QString str)
{
  walkstr = str.trimmed();
}

void cCmdParser::setNoParseString (QString str)
{
  noparsestr = str.trimmed();
}

void cCmdParser::setMacroString (QString str)
{
  macrostr = str.trimmed();
}

void cCmdParser::setAllowEmptyWalkStr (bool val)
{
  allowemptywalkstr = val;
}

void cCmdParser::setParsing (bool value)
{
  isparsing = value;
}

QStringList cCmdParser::parse (const QString &command, bool expandAliases)
{
  QStringList result;

  // no parsing or empty string- only return what was given
  if ((!isparsing) || command.isEmpty())
  {
    result.append (command);
    return result;
  }

  // look whether we should send the text as-is...
  if ((noparsestr.length() > 0) && (command.startsWith (noparsestr)))
  {
    QString t = command.mid (noparsestr.length());
    result.append (markAsRaw (t));
    return result;
  }

  // split into individual commands, then process each of them
  QStringList commands = splitIntoCommands (command);
  QStringList::iterator it;
  for (it = commands.begin(); it != commands.end(); ++it) {
    if ((*it).isEmpty())
      // empty command is simply added to the list
      result.append (*it);
    else
      // non-empty command - parse it, then add the results to our list
      result += parseCommand (*it, expandAliases);
  }

  return result;
}

#include <kdebug.h>
QStringList cCmdParser::parseCommand (const QString &command, bool expandAliases)
{
  QStringList result, res;
  bool aliasesExpanded = false;

  // look if this command should be sent as-is
  if ((noparsestr.length() > 0) && (command.startsWith (noparsestr)))
  {
    QString t = command.mid (noparsestr.length());
    result.append (markAsRaw (t));
    return result;
  }

  QString cmd = command;   // we need to modify the command

  // first look if there isn't a repeater sequence
  // if not, then repeater keeps original command and returns 1,
  // so we can handle both situations together
  int rep = repeater (cmd);

  // perform alias expansion
  if (expandAliases) {
    cList *al = cListManager::self()->getList (sess(), "aliases");
    cAliasList *aliases = al ? dynamic_cast<cAliasList *>(al) : 0;
    if (aliases && aliases->matchString (cmd))
    {
      aliasesExpanded = true;
      QStringList cmds = aliases->commandsToExec ();
      QStringList::iterator it;
      for (it = cmds.begin(); it != cmds.end(); ++it)
        // recursive call, but this time no alias expansion will occur
        // (we don't want recursive aliases)
        res += parse (*it, false);
    }
  }

  // if we expanded aliases, these things were already done for commands in the list, hence we
  // only do this if no expansion occured
  if (!aliasesExpanded) {
    // expand backslashes
    expandBackslashes (cmd);

    // add the command to the list, expanding speed-walk if needed
    int spdpos = isSpeedWalkCommand (cmd);
    if (spdpos != -1)
      res = expandSpeedWalk (cmd, spdpos);
    else
      res.append (cmd);
  }

  // return results ...

  // only one repeat - return the resulting list
  if (rep == 1) return res;

  // more repeats - generate the result, using requested number of repeats
  for (int r = 0; r < rep; r++)
    result += res;
  return result;
}

static QStringList mergeEscaped (const QStringList &cmds, QString separator)
{
  // for each command in the list, check if it ends in an odd number of \-s, if it does, and if
  // backslash expansion is allowed, we assume that the user wanted to protect this occurence
  // or command separator from expansion - hence we merge commands back ...
  QStringList cmds2;
  QString command = QString();
  QStringList::const_iterator it;
  for (it = cmds.begin(); it != cmds.end(); ++it)
  {
    command += *it;
    int len = command.length();
    if (!len) continue;
    int cnt = 0;
    if (command[len-1] == '\\') {
      // count trailing \s
      for (int i = 1; i <= len; ++i)
        if (command[len - i] == '\\')
          cnt = i;
        else
          break;
    }
    if (cnt % 2 == 1)
      command += separator;
    else
    {
      cmds2.append (command);
      command = QString();
    }
  }
  if (!command.isEmpty())
    cmds2.append (command);

  return cmds2;
}

QStringList cCmdParser::splitIntoCommands (const QString &text)
{
  QStringList cmds;
  if (text.length() == 0)
    return cmds;

  // split the command, basing on the separator string
  if (separstr.length() > 0)
    cmds = text.split (separstr);
  else
    cmds << text;
  QStringList::iterator it;

  // if we don't want to expand backslashes, most further things don't need to be done

  if (expandbackslashes) {
    QStringList cmds2 = mergeEscaped (cmds, separstr);

    // Now we have a list of commands, but we still need to expand \n separators ...
    cmds.clear ();
    for (it = cmds2.begin(); it != cmds2.end(); ++it)
    {
      // look if there's any "\n" in the string
      if ((*it).indexOf ("\\n") != -1) {
        // there is some \n
        QStringList c = (*it).split ("\\n");
        cmds += mergeEscaped (c, "\\n");
      }
      else {
        // there is no \n
        cmds.append (*it);
      }
    }
  }

  // finally, remove leading/trailing spaces from each command, if needed
  if (stripSpaces) {
    QStringList cmds2;
    for (it = cmds.begin(); it != cmds.end(); ++it)
      cmds2.append ((*it).trimmed());
    return cmds2;
  }

  return cmds;
}

bool cCmdParser::mustSendRaw (const QString &command)
{
    //This is to fix a qt4 empty string crash
    if (command.isEmpty()) return false;
  return (command[0].toLatin1() == 0x01);
}

QString cCmdParser::fixRaw (const QString &command)
{
  if (!mustSendRaw (command))
    return command;
  return command.mid (1);
}

QString cCmdParser::markAsRaw (const QString &command)
{
  return QChar (0x01) + command;
}

int cCmdParser::isSpeedWalkCommand (const QString &command)
{
  bool sw = false;

  if ((walkstr.length() > 0) && (command.startsWith (walkstr)))
    sw = true;

  //empty walk-string (if allowed)
  if ((walkstr.length() == 0) && allowemptywalkstr)
    sw = true;

  int pos = -1;
  if (sw)
  {
    pos = walkstr.length();
    for (int i = pos; i < command.length(); i++)
      switch (command[i].toLatin1()) {
        case ' ':   //spaces are ignored
        case 'n':
        case 'e':
        case 's':
        case 'w':
        case 'h':
        case 'j':
        case 'k':
        case 'l':
        case 'u':
        case 'd':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': break;
        default: return -1;
      };
  }
  else
    return -1;

  //passed all the tests - it's speed-walk command!
  return pos;
}

QStringList cCmdParser::expandSpeedWalk (const QString &command, int pos)
{
  //we could have computed the position again, but it would be inefficient
  QStringList list;
  QString cmd;
  bool wascmd;
  int count = 0;
  cProfileSettings *sett = settings();
  QString movecmds[10];
  for (int i = 0; i < 10; ++i)
    movecmds[i] = sett->getString ("movement-command-" + QString::number (i));
  for (int i = pos; i < command.length(); i++)
  {
    wascmd = true;
    switch (command[i].toLatin1()) {
      case 'n': cmd = movecmds[0]; break;
      case 'e': cmd = movecmds[2]; break;
      case 's': cmd = movecmds[4]; break;
      case 'w': cmd = movecmds[6]; break;
      case 'j': cmd = movecmds[1]; break;
      case 'l': cmd = movecmds[3]; break;
      case 'k': cmd = movecmds[5]; break;
      case 'h': cmd = movecmds[7]; break;
      case 'u': cmd = movecmds[8]; break;
      case 'd': cmd = movecmds[9]; break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        wascmd = false;
        count = count * 10 + (command[i].toLatin1() - '0');
        break;
      case ' ':
        //and spaces are ignored...
        wascmd = false;
        break;
    }
    if (wascmd)
    {
      if (count == 0) count = 1;
      for (int j = 0; j < count; j++)
        list.append (cmd);
      count = 0;
    }
  }

  return list;
}

unsigned int cCmdParser::repeater (QString &command)
{
  bool multi = false;
  if ((multistr.length() > 0) && (command.startsWith (multistr)))
    multi = true;

  if (multi)
  //chance is that there is some repeater
  {
    QString cmd = command;
    cmd.remove (0, multistr.length());
    cmd = cmd.trimmed ();
    QString rep = cmd.section (' ', 0, 0); //first section
    //if rep is a number, then this is a multi-command
    bool ok;
    unsigned int repc = rep.toUInt (&ok);
    if (ok) //GREAT!!! This really is a repeater command!!!
    {
      command = command.section (' ', 1); //everything except the 1st section

      //limit repeat count if needed
      cProfileSettings *sett = settings ();
      bool limitRepeats = ((sett == 0) ? true : sett->getBool ("limit-repeater"));

      if ((repc > 100) && limitRepeats)
      {
        invokeEvent ("message", sess(), i18n ("Sorry, count is limited to 100.\n"));
        if (sett)
          invokeEvent ("message", sess(),
              i18n ("You can disable this limit in Profile / MUD Preferences."));
        //return 0 - nothing will be sent...
        return 0;
      }
      return repc;
    }
    else       //no repeater here...
      return 1;
  }
  else  //no repeater -> count is 1
    return 1;
}

bool cCmdParser::isMacroCall (const QString &command, QString &mname, QString &params)
{
        //This is to fix a qt4 empty string crash
    if (command.isEmpty()) return false;
  bool sc1 = false, sc2 = false;
  if ((macrostr.length() > 0) && (command.startsWith (macrostr)))
    sc1 = true;
  else
    if (expandbackslashes && (command[0].toLatin1() == 0x02))
      sc2 = true;

  if (sc1 || sc2)
  {
    QString cmd = command;
    //remove the leading macrostr or 0x02 (\m gets expanded to this)
    if (sc1)
      cmd.remove (0, macrostr.length());
    else
      cmd.remove (0, 1);
    cmd = cmd.trimmed();
    //first word
    mname = cmd.section (' ', 0, 0);
    //everything else
    params = cmd.section (' ', 1);
    params = params.trimmed();
    return true;
  }
  else
    return false;
}

void cCmdParser::expandBackslashes (QString &command)
{
  //do nothing if we don't want to expand here
  if (!expandbackslashes)
    return;

  QString cmd = "";

  bool backslash = false;
  int len = command.length();
  for (int i = 0; i < len; i++)
  {
    QChar ch = command[i];
    if (backslash)
    {
      if (ch.toLatin1() == 't')  //got \t
        cmd += (QChar) '\t';
      else if (ch.toLatin1() == 'e')  //got \e
        cmd += (QChar) '\e';
      else if ((ch.toLatin1() == 'm') && (i == 1))  //got \m on the beginning of the string
        cmd += (QChar) 0x02;
      else
        cmd += ch;
      backslash = false;
    }
    else
    {
      if (ch == QChar ('\\'))
        backslash = true;
      else
        cmd += command[i];
    }
  }
  command = cmd;
}

