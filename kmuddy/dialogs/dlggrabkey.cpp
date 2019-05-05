/***************************************************************************
                          dlggrabkey.cpp  -  keygrabber
                             -------------------
    begin                : Pi máj 30 2003
    copyright            : (C) 2003 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dlggrabkey.h"

#include "kmuddy.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <KLocalizedString>

dlgGrabKey::dlgGrabKey (QWidget *parent) : QDialog (parent)
{
  KMuddy::self()->setGrabDialog (this);
  
  //initial dialog size
  setWindowTitle (i18n ("Keygrabber"));
  QVBoxLayout *layout = new QVBoxLayout (this);

  QLabel *label = new QLabel (i18n ("Press the desired shortcut..."), this);

  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Cancel,this);
  QPushButton *button = buttons->button (QDialogButtonBox::Cancel);
  button->setDefault(true);
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->addWidget (label);
  layout->addWidget (buttons);
}

dlgGrabKey::~dlgGrabKey ()
{
  KMuddy::self()->setGrabDialog (nullptr);
}

QSize dlgGrabKey::sizeHint() const
{
  return QSize (200, 80);
}

void dlgGrabKey::gotKey (int key, Qt::KeyboardModifiers state)
{
  _key = key;
  _state = state;
  accept ();
}

