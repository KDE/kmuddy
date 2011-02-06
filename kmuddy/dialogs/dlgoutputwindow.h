//
// C++ Interface: dlgoutputwindow
//
// Description: 
//
//
// Author: Vladimir Lazarenko <vlad@lazarenko.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGOUTPUTWINDOW_H
#define DLGOUTPUTWINDOW_H

#include <qfont.h>
#include <kdialog.h>
#include <kwindowsystem.h>

class cConsole;
class cTextChunk;

/**
One output window.
@author Vladimir Lazarenko
*/

class dlgOutputWindow : public KDialog
{
  Q_OBJECT
public:
  dlgOutputWindow (QWidget *parent = 0);
  ~dlgOutputWindow();

  void addLine (cTextChunk *chunk);
  void setOutputWindowName(const QString &name);
  void setFont(QFont font);
  void setSession (int _sess);
  cConsole *console() { return owindow; };
protected:
  cConsole *owindow;
  int sess;
};

#endif
