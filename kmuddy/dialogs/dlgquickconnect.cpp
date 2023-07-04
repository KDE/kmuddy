/***************************************************************************
                          dlgquickconnect.cpp  -  QuickConnect dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Út Jul 23 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#include "dlgquickconnect.h"

//needed by gcc 3.2
#define max(a,b) (((a) >= (b)) ? (a) : (b))

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <KLocalizedString>
#include <klineedit.h>
#include <krestrictedline.h>

dlgQuickConnect::dlgQuickConnect(QWidget *parent) : QDialog (parent)
{
  //initial dialog size
  setWindowTitle (i18n ("Quick Connect"));

  QGridLayout *layout = new QGridLayout (this);
  
  //put some edit boxes there
  QLabel *l1 = new QLabel (i18n ("&Host:"), this);
  ed1 = new KLineEdit (this);
  QLabel *l2 = new QLabel (i18n ("&Port:"), this);
  ed2 = new KRestrictedLine (this);
  ed2->setValidChars ("0123456789");
  int w = max (l1->width(), l2->width());
  l1->setFixedWidth (w);
  l2->setFixedWidth (w);
  ed1->setGeometry (ed1->x(), ed1->y(), 200, ed1->height());
  ed2->setFixedWidth (80);

  l1->setBuddy (ed1);
  l2->setBuddy (ed2);
  
  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  QPushButton *button = buttons->button (QDialogButtonBox::Ok);
  button->setText (i18n ("&Connect"));
  button->setToolTip (i18n ("Establishes connection with specified parameters."));
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->addWidget (l1, 0, 0);
  layout->addWidget (l2, 1, 0);
  layout->addWidget (ed1, 0, 1);
  layout->addWidget (ed2, 1, 1);
  layout->addWidget (buttons, 2, 0, 1, 2);

  //humm, this one seems to be causing some X Error 42 BadMatch...
  //I have no idea why does it happen; however, everything seems
  //to be working perfectly...
  ed1->setFocus ();
}

dlgQuickConnect::~dlgQuickConnect()
{
  //all widgets are destroyed in Qt's destructors, so we needn't care about it

}

QSize dlgQuickConnect::sizeHint() const
{
  return QSize (300, 120);
}

QString dlgQuickConnect::host ()
{
  return ed1->text();
}

int dlgQuickConnect::port ()
{
  return ed2->text().toInt ();
}

#include "moc_dlgquickconnect.cpp"
