/***************************************************************************
                          cconnection.h  -  handles connections
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Jul 22 2002
    copyright            : (C) 2002 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CCONNECTION_H
#define CCONNECTION_H

#include <qobject.h>
#include <qstringlist.h>

#include "cactionbase.h"

struct cConnectionPrivate;

/**
This class handles connecting, disconnecting, calls load/save
of connection settings and related things.
  *@author Tomas Mecir
  */

class cConnection : public QObject, public cActionBase {
   Q_OBJECT
public: 
  cConnection (int sess);
  ~cConnection () override;
  void establishConnection (const QString &profileName, bool sendNothing = false);
  void establishQuickConnection (const QString &server, int port);
  bool isConnected ();
  void disconnect ();
  void reconnect ();
  QString getCaption ();
  /** call enable/disable menu items in KMuddy as needed */
  void updateMenus ();
  void setConnectionClosed (bool canCloseTab);
  void unsetConnectionClosed ();
  bool connectionClosed ();
  
  void addCommands (const QStringList &commands);
  void addCommand (const QString &command);
  void sendCommands ();
  int sentCommands ();

  void eventNothingHandler (QString event, int session) override;
  void eventStringHandler (QString event, int session, QString &par1, const QString &) override;
  QString actionStringHandler (QString action, int session, QString &par1, const QString &) override;
  QString actionNothingHandler (QString action, int session) override;

public slots:
  void showConnPrefsDialog ();
  bool handleDisconnect ();
  /** shows Output Windows dialog box */
  void handleWindowsDialog ();
  /** enable/disable alias processing */
  void switchAliases (bool val);
  /** enable/disable trigger processing */
  void switchTriggers (bool val);
  /** enable/disable timer processing */
  void switchTimers (bool val);
  /** enable/disable macro key processing */
  void switchShortcuts (bool val);
  /** used to save settings */
  void saveSession ();
protected slots:
  /** attached to timeout() of connecting timer*/
  void sendLoginAndPassword ();
  /** called when user presses Ok or Apply in MUD Settings dialog */
  void getSettingsFromDialog ();
  /** save settings */
  void save ();
protected:
  /** puts profile settings to MUD Settings dialog */
  void putSettingsToDialog ();

  /** Register the internal lists. */
  void registerLists ();

  /** d-pointer for this class */
  cConnectionPrivate *d;
};

#endif
