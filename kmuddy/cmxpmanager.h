//
// C++ Interface: cmxpmanager
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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
  virtual void eventNothingHandler (QString event, int session) override;
  virtual QString actionIntHandler (QString action, int session, int par1, int par2 = 0) override;
  virtual QString actionNothingHandler (QString action, int session) override;
  
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
