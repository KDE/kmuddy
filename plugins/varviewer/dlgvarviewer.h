//
// C++ Interface: dlgvarviewer
//
// Description: 
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef DLGVARVIEWER_H
#define DLGVARVIEWER_H

#include <QDockWidget>

#include "cactionbase.h"

class QTreeView;
class VariableModel;

/**
Dialog showing the variable list.

@author Tomas Mecir
*/
class dlgVarViewer : public QDockWidget, public cActionBase
{
Q_OBJECT
public:

  /** constructor */
  dlgVarViewer (QWidget *parent = 0);
  /** destructor */
  ~dlgVarViewer ();
protected:
  void createDialog ();

  void eventNothingHandler (QString event, int sess);
  void eventIntHandler (QString event, int, int val, int);
  void eventStringHandler (QString event, int sess, QString &par1, const QString &);

  void showEvent (QShowEvent *);

  QTreeView *viewer;
  VariableModel *model;
};

#endif
