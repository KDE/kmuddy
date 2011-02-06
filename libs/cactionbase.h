//
// C++ Interface: cActionBase
//
// Description: action base
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, 2005-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CACTIONBASE_H
#define CACTIONBASE_H

#include <qstring.h>

#include <kmuddy_export.h>

struct cActionBasePrivate;

class cProfileSettings;
class cTextChunk;
enum ParamType { PT_INT, PT_STRING, PT_NOTHING, PT_TEXTCHUNK, PT_POINTER };

/**
Base class for every object that wants to use cActionManager.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cActionBase {
public:
  /** Constructor. Registers itself with cActionManager, if session >= 0 */
  cActionBase (const QString &name, int session = -1);
  /** destructor */
  virtual ~cActionBase ();

  /** object's name */
  QString objName () { return _name; };
  /** session number */
  int sess ();

  int attrib (const QString &name);
  void setAttrib (const QString &name, int value);
  QString strAttrib (const QString &name);
  void setStrAttrib (const QString &name, const QString &value);
protected:
  void addEventHandler (QString name, int priority, ParamType pt);
  void removeEventHandler (QString name);
  void addGlobalEventHandler (QString name, int priority, ParamType pt);
  void removeGlobalEventHandler (QString name);

  /** Returns the settings of the session. Provided for convenience. */
  cProfileSettings *settings ();

  cActionBase *object (const QString &name, int session = -1);

// the following 10 methods exist so that we don't have to #include cActionManager everywhere
  /** call an object's action - PT_INT parameter */
  QString callAction (QString objectName, QString action, int session, int par1, int par2=0);
  /** call an object's action - PT_STRING parameter */
  QString callAction (QString objectName, QString action, int session,
      QString &par1, const QString &par2=QString());
  /** as above, but with const string */
  QString callAction (QString objectName, QString action, int session,
      const QString &par1, const QString &par2=QString());
  /** call an object's action - PT_NOTHING parameter */
  QString callAction (QString objectName, QString action, int session);
  /** call an object's action - PT_TEXTCHUNK parameter */
  QString callAction (QString objectName, QString action, int session, cTextChunk *par);
  /** call an object's action - PT_POINTER parameter */
  QString callAction (QString objectName, QString action, int session, void *par);
  /** invoke an event - PT_INT parameter */
  void invokeEvent (QString event, int session, int par1, int par2 = 0);
  /** invoke an event - PT_STRING parameter */
  void invokeEvent (QString event, int session, QString &par1,
      const QString &par2 = QString());
  /** as above, but const param - so that passing constant strings works */
  void invokeEvent (QString event, int session, const QString &par1,
      const QString &par2 = QString());
  /** invoke an event - PT_NOTHING parameter */
  void invokeEvent (QString event, int session);
  /** invoke an event - PT_TEXTCHUNK parameter */
  void invokeEvent (QString event, int session, cTextChunk *par);
  /** invoke an event - PT_POINTER parameter */
  void invokeEvent (QString event, int session, void *par);

    
  virtual QString actionIntHandler (QString /*action*/, int /*session*/, int /*par1*/, int /*par2*/)
      { return QString(); };
  virtual QString actionStringHandler (QString /*action*/, int /*session*/,
      QString &/*par1*/, const QString &/*par2*/)
      { return QString(); };
  virtual QString actionNothingHandler (QString /*action*/, int /*session*/)
      { return QString(); };
  virtual QString actionChunkHandler (QString /*action*/, int /*session*/, cTextChunk * /*par*/)
      { return QString(); };
  virtual QString actionVoidHandler (QString /*action*/, int /*session*/, void * /*par*/)
      { return QString(); };

  virtual void eventNothingHandler (QString /*event*/, int /*session*/);
  virtual void eventIntHandler (QString /*event*/, int /*session*/, int /*par1*/, int /*par2*/);
  virtual void eventStringHandler (QString /*event*/, int /*session*/,
      QString &/*par1*/, const QString &/*par2*/);
  virtual void eventChunkHandler (QString /*event*/, int /*session*/, cTextChunk * /*par*/);
  virtual void eventVoidHandler (QString /*event*/, int /*session*/, void * /*par*/);

  QString _name;
  cActionBasePrivate *d;

  // cActionManager will want to access event handlers ...
  friend class cActionManager;
};

#endif
