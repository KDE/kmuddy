//
// C++ Implementation: dlgwindows
//
// Description: 
//
//
// Author: Vladimir Lazarenko <vlad@lazarenko.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dlgwindows.h"

#include "cwindowlist.h"

#include <kapplication.h>
#include <klocale.h>
#include <kpushbutton.h>

#include <qlabel.h>
#include <QGridLayout>
#include <QListWidget>
#include <QVBoxLayout>

dlgWindows::dlgWindows(cWindowList *wlist, QWidget *parent) : KDialog(parent)
{
  wl = wlist->windowList();
  winlist = wlist;
  
  createDialog();
}


dlgWindows::~dlgWindows()
{
}

void dlgWindows::createDialog()
{
  //initial dialog size
  setInitialSize (QSize (500, 400));
  setCaption (i18n("Output Windows"));
  setButtons (KDialog::Ok);
  
  //create main dialog's widget
  QWidget *page = new QWidget (this);
  QGridLayout *layout = new QGridLayout (page);

  setMainWidget (page);

  //change button text
  setButtonText (KDialog::Ok, i18n ("Done"));

  //create widgets
  QLabel *label = new QLabel (i18n ("&List of output windows"), page);
  box = new QListWidget (page);
  label->setBuddy (box);
  
  QFrame *buttons = new QFrame (page);
  QVBoxLayout *buttonslayout = new QVBoxLayout (buttons);
  
  btshow = new KPushButton (i18n("&Show"), buttons);
  bthide = new KPushButton (i18n("&Hide"), buttons);
  btdelete = new KPushButton (i18n("&Delete"), buttons);
  
  buttonslayout->setSpacing (5);
  
  buttonslayout->addWidget (btshow);
  buttonslayout->addWidget (bthide);
  buttonslayout->addWidget (btdelete);
  buttonslayout->addStretch (10);

  layout->setColumnStretch (0, 10);
  layout->setRowStretch (1, 10);
  layout->setSpacing (5);

  layout->addWidget (label, 0, 0);
  layout->addWidget (box, 1, 0);
  layout->addWidget (buttons, 1, 1);
  
  updateMe();
  
  connect (btshow, SIGNAL (clicked ()), this, SLOT (wshow ()));
  connect (bthide, SIGNAL (clicked ()), this, SLOT (whide ()));
  connect (btdelete, SIGNAL (clicked ()), this, SLOT (remove ()));

}

void dlgWindows::wshow()
{
  QString name;
  
  name = box->currentItem()->text();
  
  if(!winlist->exists(name))
    return;
  
  if(winlist->isShown(name))
    return;
  
  winlist->show(name);
}

void dlgWindows::whide()
{
  
  QString name;
  
  name = box->currentItem()->text();
  
  if(!winlist->exists(name))
    return;
  
  if(!winlist->isShown(name))
    return;
  
  winlist->hide(name);
}

void dlgWindows::remove()
{
  QString name;
  
  name = box->currentItem()->text();
  
  if(!winlist->exists(name))
    return;
  
  winlist->remove(name);
  updateMe();
}

void dlgWindows::updateMe ()
{
  //remember old index
  int idx = box->currentRow ();

  box->clear ();
  box->addItems(winlist->windowList());

  //make one item active, trying to be intelligent...
  box->setCurrentRow ((idx == -1) ? 0 : idx);
  if ((box->currentRow () == -1) && (idx != -1))
    box->setCurrentRow (idx - 1);
  if (box->currentRow () == -1)
    box->setCurrentRow (0);
}


#include "dlgwindows.moc"
