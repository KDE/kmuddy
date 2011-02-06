//
// C++ Implementation: dlgoutputwindow
//
// Description: 
//
//
// Author: Vladimir Lazarenko <vlad@lazarenko.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dlgoutputwindow.h"
#include "cconsole.h"

#include <klocale.h>
#include <kdeversion.h>

#include <kwindowsystem.h>

#include <netwm_def.h>
#include <QVBoxLayout>

dlgOutputWindow::dlgOutputWindow (QWidget *parent) : KDialog(parent)
{
  //initial size
  setInitialSize (QSize (250, 300));
  setCaption (i18n ("Output window"));
  setButtons (0);

  //main widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);
  
  setMainWidget (page);
  setFocusPolicy (Qt::StrongFocus);

  owindow = new cConsole(true, page);
  //our window shall also have a status bar
  owindow->setVScrollBarMode (Q3ScrollView::AlwaysOn);

  //no session information yet, will set it when it's available
  sess = 0;
  owindow->setSession (0);

  KDialog::resizeLayout (this, 0, 5);
  
  KWindowSystem::setType(this->winId(), NET::Utility);

  layout->setMargin (0);
  layout->addWidget(owindow);
}

dlgOutputWindow::~dlgOutputWindow()
{
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
    this->setCaption(name);
}

void dlgOutputWindow::setFont(QFont font)
{
  owindow->setFont(font);
}

#include "dlgoutputwindow.moc"

