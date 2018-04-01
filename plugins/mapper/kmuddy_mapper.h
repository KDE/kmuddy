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

struct KMuddyMapperPrivate;

class KMuddyMapper : public cPlugin
{
Q_OBJECT
public:
  KMuddyMapper (QObject *, const QVariantList &);
  virtual ~KMuddyMapper ();

  virtual void sessionAdd (int sess, bool fresh = true) override;
  virtual void sessionRemove (int sess, bool closed = true) override;
  virtual void sessionSwitch (int sess) override;
  virtual void connected (int sess) override;
  virtual void disconnected (int sess) override;
  
  virtual void load (int sess);
  virtual void save (int sess) override;
  
  virtual void processInput (int sess, int phase, cTextChunk * chunk,
      bool gagged) override;
  virtual void processCommand (int sess, QString & command, bool &dontSend) override;

protected slots:
  void showMapper (bool b);
  void mapperClosed ();
private:
  KMuddyMapperPrivate *d;
};


#endif // KMUDDYMAPPER_H

