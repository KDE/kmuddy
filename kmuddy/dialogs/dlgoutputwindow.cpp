//
// C++ Implementation: dlgoutputwindow
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

#include "dlgoutputwindow.h"
#include "cconsole.h"

#include <KLocalizedString>

#include <QVBoxLayout>

dlgOutputWindow::dlgOutputWindow (QWidget *parent) : QDialog(parent, Qt::Tool)
{
  //initial size
  setWindowTitle (i18n ("Output window"));

  //main widget
  QVBoxLayout *layout = new QVBoxLayout (this);
  
  setFocusPolicy (Qt::StrongFocus);

  owindow = new cConsole(this);

  //no session information yet, will set it when it's available
  sess = 0;
  owindow->setSession (0);

  layout->setContentsMargins (0, 0, 0, 0);
  layout->addWidget(owindow);
}

dlgOutputWindow::~dlgOutputWindow()
{
}

QSize dlgOutputWindow::sizeHint() const
{
  return QSize (250, 300);
}

void dlgOutputWindow::setSession (int _sess)
{
  sess = _sess;
  //also inform console about the new session
  owindow->setSession (sess);
}

void dlgOutputWindow::addLine (cTextChunk *chunk)
{
  if (owindow)
    owindow->addLine (chunk);
}

void dlgOutputWindow::setOutputWindowName(const QString &name)
{
  if(!name.isEmpty())
    this->setWindowTitle(name);
}

void dlgOutputWindow::setFont(const QFont &font)
{
  owindow->setFont(font);
}

#include "moc_dlgoutputwindow.cpp"
