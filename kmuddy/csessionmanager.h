//
// C++ Interface: cSessionManager
//
// Description: Session manager.
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

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
  ~cSessionManager () override;

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

  void eventNothingHandler (QString event, int session) override;

  enum ProfileIcon {
    IconOk, IconNo, IconFlag
  };
  /** Set an icon for the session. */
  void setIcon (int sess, ProfileIcon icon);

  static cSessionManager *_self;
  cSessionManagerPrivate *d;
};

#endif
