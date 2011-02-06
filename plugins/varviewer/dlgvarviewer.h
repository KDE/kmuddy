//
// C++ Interface: dlgvarviewer
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
