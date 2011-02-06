//
// C++ Interface: testplugin
//
// Description: Testing Plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

