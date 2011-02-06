//
// C++ Interface: dlgmxpconsole
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
