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

#include <qlabel.h>

#include <klocale.h>
#include <kvbox.h>

dlgGrabKey::dlgGrabKey (QWidget *parent) : KDialog (parent)
{
  KMuddy::self()->setGrabDialog (this);
  
  //initial dialog size
  setInitialSize (QSize (200, 80));
  setCaption (i18n ("Keygrabber"));
  setButtons (KDialog::Cancel);
  setDefaultButton (KDialog::Cancel);

  //create main dialog's widget
  KVBox *vbox = new KVBox (this);
  setMainWidget (vbox);
  new QLabel (i18n ("Press the desired shortcut..."), vbox);
}

dlgGrabKey::~dlgGrabKey ()
{
  KMuddy::self()->setGrabDialog (0);
}

void dlgGrabKey::gotKey (int key, Qt::KeyboardModifiers state)
{
  _key = key;
  _state = state;
  accept ();
}

#include "dlggrabkey.moc"

