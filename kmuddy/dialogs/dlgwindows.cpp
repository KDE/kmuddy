//
// C++ Implementation: dlgwindows
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

#include "dlgwindows.h"

#include "cwindowlist.h"

#include <kapplication.h>
#include <klocale.h>

#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

dlgWindows::dlgWindows(cWindowList *wlist, QWidget *parent) : QDialog(parent)
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
  setWindowTitle (i18n("Output Windows"));
  
  //create main dialog's widget
  QGridLayout *layout = new QGridLayout (this);

  //create widgets
  QLabel *label = new QLabel (i18n ("&List of output windows"), this);
  box = new QListWidget (this);
  label->setBuddy (box);
  
  QFrame *buttons = new QFrame (this);
  QVBoxLayout *buttonslayout = new QVBoxLayout (buttons);
  
  btshow = new QPushButton (i18n("&Show"), buttons);
  bthide = new QPushButton (i18n("&Hide"), buttons);
  btdelete = new QPushButton (i18n("&Delete"), buttons);
  
  buttonslayout->setSpacing (5);
  
  buttonslayout->addWidget (btshow);
  buttonslayout->addWidget (bthide);
  buttonslayout->addWidget (btdelete);
  buttonslayout->addStretch (10);

  QDialogButtonBox *dlgbuttons = new QDialogButtonBox (QDialogButtonBox::Close, this);
  connect (dlgbuttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (dlgbuttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->setColumnStretch (0, 10);
  layout->setRowStretch (1, 10);
  layout->setSpacing (5);

  layout->addWidget (label, 0, 0);
  layout->addWidget (box, 1, 0);
  layout->addWidget (buttons, 1, 1);
  layout->addWidget (dlgbuttons, 2, 0, 1, 2);
  
  updateMe();
  
  connect (btshow, SIGNAL (clicked ()), this, SLOT (wshow ()));
  connect (bthide, SIGNAL (clicked ()), this, SLOT (whide ()));
  connect (btdelete, SIGNAL (clicked ()), this, SLOT (remove ()));

}

QSize dlgWindows::sizeHint() const
{
  return QSize (500, 400);
}

void dlgWindows::wshow()
{
  if (!box->count()) return;
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
  if (!box->count()) return;
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
  if (!box->count()) return;
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

