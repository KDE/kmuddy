//
// C++ Interface: dlgoutputwindow
//
// Description: 
//
/*
Copyright 2004 Vladimir Lazarenko <vlad@lazarenko.net>

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

#ifndef DLGOUTPUTWINDOW_H
#define DLGOUTPUTWINDOW_H

#include <qfont.h>
#include <QDialog>
#include <kwindowsystem.h>

class cConsole;
class cTextChunk;

/**
One output window.
@author Vladimir Lazarenko
*/

class dlgOutputWindow : public QDialog
{
  Q_OBJECT
public:
  dlgOutputWindow (QWidget *parent = nullptr);
  ~dlgOutputWindow() override;

  QSize sizeHint() const override;
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
