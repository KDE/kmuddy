//
// C++ Interface: cmxpconsole
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

#ifndef CMXPCONSOLE_H
#define CMXPCONSOLE_H

#include <qobject.h>

#include <config-mxp.h>

class QDockWidget;
class dlgMXPConsole;

/**
This class manages the MXP console dialog box.

It follows the Singleton pattern.

@author Tomas Mecir
*/
class cMXPConsole : public QObject
{
Q_OBJECT
public:

#ifdef HAVE_MXP
  
  /** destructor */
  ~cMXPConsole ();
  static cMXPConsole *self ();
  QDockWidget *dialog ();

#endif  //HAVE_MXP

public slots:   //slots need to be defined at all occassions, otherwise MOC would have problems
  void addError (int sess, const QString &text);
  void addWarning (int sess, const QString &text);

#ifdef HAVE_MXP

protected:
  /** constructor */
  cMXPConsole ();
  /** create the dialog */
  void createDialog ();
  dlgMXPConsole *dlgmxpconsole;
  
  static cMXPConsole *_self;

#endif  //HAVE_MXP

};

#endif
