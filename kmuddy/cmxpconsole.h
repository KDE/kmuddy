//
// C++ Interface: cmxpconsole
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
