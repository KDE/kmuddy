//
// C++ Interface: cplugin
//
// Description: see below
//
/*
Copyright 2006-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CPLUGIN_H
#define CPLUGIN_H

#include <ctextchunk.h>
#include <kmuddy_export.h>

/**
This class represents one plug-in for KMuddy. It is created by a plug-in factory.

The cSession pointer is passed in all routines, so that you can distinguish between events in
different connections.

Note that plug-ins link to libkmuddy, allowing you to use cActionManager, cMacroManager and other useful classes, if the provided
interface is not sufficient, or if you want to define your own macros or
functions in the plug-in.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cPlugin : public QObject
{
Q_OBJECT
public:
  cPlugin ();
  virtual ~cPlugin ();

  /** priority in which this plug-in receives texts/events. Lower number means higher priority. */
  int priority () { return _priority; };

  /** Called when a session has been added. If the session has existed before, fresh is set
  to false. This can happen if the plug-in is loaded manually. */
  virtual void sessionAdd (int /*sess*/, bool /*fresh*/ = true) {};
  /** Called when a session should be removed. Closed is false, if the session isn't being
  closed. This hapens when the plug-in is being unloaded manually. */
  virtual void sessionRemove (int /*sess*/, bool /*closed*/ = true) {};
  /** called when the user switches to another session. Sess is the number of the new session. */
  virtual void sessionSwitch (int /*sess*/) {};
  /** The session has just been connected. Not called when manually loading the plug-in. */
  virtual void connected (int /*sess*/) {};
  /** The session has just been disconnected. Not called when manually unloading the plug-in. */
  virtual void disconnected (int /*sess*/) {};
  /** Request to save data. */
  virtual void save (int /*sess*/) {};
  
  /** raw data from the socket, before any processing; data can be modified by the plug-in
  if desired */
  virtual void rawData (int /*sess*/, char * /*data*/) {};
  /** raw data, after MCCP decompression, data is equivalent to rawData if MCCP is not used;
  data can be modified by plug-in if desired */
  virtual void decompressedData (int /*sess*/, char * /*data*/) {};
  /** process one line of input from the MUD; called after telnet/ANSI processing;
  this function is called TWICE for each line, first time before triggers get the line, second
  time right before displaying the line (or discarding if gagged).
  Data can be modified if desired.
  @param sess session pointer
  @param phase which phase are we in? 1 means before triggers, 2 means after triggers
  @param chunk the line
  @param gagged only valid if phase is 2, it says whether this line is to be gagged */
  virtual void processInput (int /*sess*/, int /*phase*/, cTextChunk * /*chunk*/,
      bool /*gagged*/) {};
  /** prompt that will be displayed; you can modify it is desired */
  virtual void processPrompt (int /*sess*/, cTextChunk * /*chunk*/) {};
  /** Command that is to be sent to the MUD. Aliases have already been expanded.
  Command can be modified if desired. If you set dontSend to true, the command won't be
  sent and plug-ins with lower priority won't receive this command either. */
  virtual void processCommand (int /*sess*/, QString & /*command*/, bool & /*dontSend*/) {};

  // *** some functions, provided for convenience ***

  /** set active session - should only be used by cPluginManager */
  void setActiveSession (int sess) { activesess = sess; };
  /** Returns the active session. Provided for convenience. */
  int activeSession () { return activesess; };
  /** Displays a system message. Provided for convenience. */
  void systemMessage (int sess, const QString &message);
  /** Sends a command. Provided for convenience. */
  void sendCommand (int sess, const QString &command);
  
 protected:
  int activesess;
  int _priority;
};

#endif
