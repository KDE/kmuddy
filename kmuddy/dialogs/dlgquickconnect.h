/***************************************************************************
                          dlgquickconnect.h  -  QuickConnect dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Út Jul 23 2002
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

#ifndef DLGQUICKCONNECT_H
#define DLGQUICKCONNECT_H

#include <kdialog.h>

class KLineEdit;
class KRestrictedLine;

/**
QuickConnect dialog - handled by cConnection.
  *@author Tomas Mecir
  */

class dlgQuickConnect : public KDialog {
   Q_OBJECT
public: 
  dlgQuickConnect (QWidget *parent=0);
  ~dlgQuickConnect ();

  /** values in LineEdits */
  QString host ();
  int port ();

protected:
  KLineEdit *ed1;
  KRestrictedLine *ed2;
};

#endif
