//
// C++ Interface: KMuddyMapper
//
// Description: Plugin interface file for the KMuddy mapper.
//
//
// Author: Tomas Mecir, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KMUDDYMAPPER_H
#define KMUDDYMAPPER_H

#include "cplugin.h"

#include <QVariantList>

class KComponentData;

struct KMuddyMapperPrivate;

class KMuddyMapper : public cPlugin
{
Q_OBJECT
public:
  KMuddyMapper (QObject *, const QVariantList &);
  virtual ~KMuddyMapper ();

  KComponentData componentData () const;

  virtual void sessionAdd (int sess, bool fresh = true);
  virtual void sessionRemove (int sess, bool closed = true);
  virtual void sessionSwitch (int sess);
  virtual void connected (int sess);
  virtual void disconnected (int sess);
  
  virtual void load (int sess);
  virtual void save (int sess);
  
  virtual void processInput (int sess, int phase, cTextChunk * chunk,
      bool gagged);
  virtual void processCommand (int sess, QString & command, bool &dontSend);

protected slots:
  void showMapper (bool b);
  void mapperClosed ();
private:
  KMuddyMapperPrivate *d;
};


#endif // KMUDDYMAPPER_H

