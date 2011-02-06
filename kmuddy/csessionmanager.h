//
// C++ Interface: cSessionManager
//
// Description: Session manager.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CSESSIONMANAGER_H
#define CSESSIONMANAGER_H

struct cSessionManagerPrivate;
class cTabWidget;

#include "cactionbase.h"
             
#include <qstring.h>

/**
This class manages the open sessions.

@author Tomas Mecir
*/
class cSessionManager : public cActionBase {
public:
  /** returns pointer to instance */
  static cSessionManager *self();
  /** destructor */
  ~cSessionManager ();

  /** session count */
  int count ();
  
  int getSessionByTab (int tabnumber);
  int getTabBySession (int sess);

  int addSession (bool profile = true);
  bool removeSession (int which, bool dontClose = false);
  void setSession (int which);
  int activeSession ();
  bool isProfileSession (int sess);
  void setSessionName (int which, QString name, bool defName = true);
  void changeSession (int id);
  
  /** should the tab bar be displayed even if there is only one connection? */
  bool alwaysTabBar ();
  void setAlwaysTabBar (bool value);

  /** set notify flag for the given tab */
  void setNotifyFlag (int sess);

  /** The main widget. */
  void setMainWidget (cTabWidget *widget);
private:
  /** constructor */
  cSessionManager ();

  void eventNothingHandler (QString event, int session);

  enum ProfileIcon {
    IconOk, IconNo, IconFlag
  };
  /** Set an icon for the session. */
  void setIcon (int sess, ProfileIcon icon);

  static cSessionManager *_self;
  cSessionManagerPrivate *d;
};

#endif
