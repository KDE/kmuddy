//
// C++ Interface: testplugin
//
// Description: Testing Plugin.
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <cplugin.h>
#include <QVariantList>

class cTestPlugin : public cPlugin
{
public:
  cTestPlugin (QObject *, const QVariantList &);
  virtual ~cTestPlugin ();

  virtual void sessionAdd (int sess, bool fresh = true);
  virtual void sessionRemove (int sess, bool closed = true);
  virtual void sessionSwitch (int sess);
  virtual void connected (int sess);
  virtual void disconnected (int sess);
  virtual void rawData (int sess, char * data);
  virtual void decompressedData (int sess, char * data);
  virtual void processInput (int sess, int phase, cTextChunk * chunk,
      bool gagged);
  virtual void processPrompt (int sess, cTextChunk * chunk);
  virtual void processCommand (int sess, QString & command, bool & dontSend);
};


#endif // TEST_PLUGIN_H

