//
// C++ Interface: cmxpmanager
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CMXPMANAGER_H
#define CMXPMANAGER_H

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>

#include <config-mxp.h>

#include "cactionbase.h"

#ifdef HAVE_MXP
#include <libmxp/libmxp.h>
#endif //HAVE_MXP

/**
This class interfaces with the MXP library and provides information about received text and tags, as sent by the MXP library.

@author Tomas Mecir
*/

class cMXPManager : public QObject, public cActionBase
{
Q_OBJECT
public:
  /** constructor */
  cMXPManager (int sess);
  /** destructor */
  ~cMXPManager ();
  
#ifdef HAVE_MXP
  virtual void eventNothingHandler (QString event, int session);
  virtual QString actionIntHandler (QString action, int session, int par1, int par2 = 0);
  virtual QString actionNothingHandler (QString action, int session);
  
  bool isMXPActive () { return mxpactive; };
  void setMXPActive (bool active);

  /** is the server actually sending us MXP sequences? */
  bool isMXPInUse () { return mxpinuse; };
    
  /** process text coming from the server */
  void processText (const QString &text);

  void switchToOpenMode ();
  void setDefaultFont (QFont font, QColor fg, QColor bg);
#endif //HAVE_MXP

//some signals... these must not be #defined out, or MOC would complain...
signals:
  void gotNewText (const QString &text);
  void gotFgColor (QColor color);
  void gotBgColor (QColor color);
  void gotAttrib (int a);
  void gotALink (const QString &name, const QString &url, const QString &text,
      const QString &hint);
  void gotSENDLink (const QString &name, const QString &command, const QString &text,
      const QString &hint, bool toprompt, bool ismenu);
  void gotExpire (const QString &name);
  void gotNewLine ();
  
//same holds for slots...
public slots:
  void reset ();

#ifdef HAVE_MXP

protected:
  void processResult (mxpResult *res);
  QColor toColor (RGB mxpColor);
  void sendDefaultFont ();
  void initLibrary ();
  bool mxpactive, havehandler;
  bool mxpinuse;
  
  QFont deffont;
  QColor deffg, defbg;
  int curattrib;
    
  MXPHANDLER h;
#endif //HAVE_MXP

};

#endif
