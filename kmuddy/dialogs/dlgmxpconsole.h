//
// C++ Interface: dlgmxpconsole
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

#ifndef DLGMXPCONSOLE_H
#define DLGMXPCONSOLE_H

#include <config-mxp.h>
#include <QDockWidget>

class QTextEdit;

/**
Dialog showing the MXP console.

@author Tomas Mecir
*/
class dlgMXPConsole : public QDockWidget
{
Q_OBJECT
public:

#ifdef HAVE_MXP
  
  /** constructor */
  dlgMXPConsole (QWidget *parent = 0);
  /** destructor */
  ~dlgMXPConsole ();
  /** add new line to the dialog */
  void addLine (const QString &line);
protected:
  void createDialog ();
  QTextEdit *viewer;

#endif  //HAVE_MXP

};

#endif
