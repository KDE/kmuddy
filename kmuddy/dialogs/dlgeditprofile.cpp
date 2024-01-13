/***************************************************************************
                          dlgeditprofile.cpp  -  Edit Profile dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pá Aug 9 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
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

#include "dlgeditprofile.h"

//needed by gcc 3.2
#define max(a,b) (((a) >= (b)) ? (a) : (b))

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <KLocalizedString>
#include <kpassworddialog.h>
#include <kmessagebox.h>
#include <KTextEdit>

dlgEditProfile::dlgEditProfile(QWidget *parent) : QDialog (parent)
{
  setWindowTitle (i18n ("Edit profile"));

  //create main dialog's widget
  QGridLayout *layout = new QGridLayout (this);

  // put some edit boxes there
  QLabel *l1 = new QLabel (i18n ("Profile &name:"), this);
  ed1 = new QLineEdit (this);
  ed1->setWhatsThis( i18n ("Enter profile name here. You can enter any name, but it must be <b>unique</b>."));
  QLabel *l2 = new QLabel (i18n ("&Server:"), this);
  ed2 = new QLineEdit (this);
  ed2->setWhatsThis( i18n ("Enter address of the server where your MUD is running."));
  QLabel *l3 = new QLabel (i18n ("&Port:"), this);
  ed3 = new QSpinBox (this);
  ed3->setRange(1, 65535);
  ed3->setValue(23);
  ed3->setWhatsThis( i18n ("Enter port on which your MUD is running (usually some 4-digit number)."));
  QLabel *l4 = new QLabel (i18n ("&Login name:"), this);
  ed4 = new QLineEdit (this);
  ed4->setWhatsThis( i18n ("Enter your login name on the MUD here.<p><i>This setting is optional.</i>"));
  QLabel *l5 = new QLabel (i18n ("Pass&word:"), this);
  ed5 = new QLineEdit (this);
  ed5->setEchoMode (QLineEdit::Password);
  ed5->setWhatsThis( i18n ("Enter your password on the MUD here.<p><i>This setting is optional.</i>"));
  QLabel *l6 = new QLabel (i18n ("&Connect sequence"), this);
  connstr = new KTextEdit (this);
  connstr->setAcceptRichText (false);
  connstr->setWordWrapMode (QTextOption::NoWrap);
  connstr->setWhatsThis( i18n ("Commands to be sent to the MUD after "
      "logging in.<br><b>$name</b> expands to username.<br><b>$password</b> "
      "expands to your password.<p><b>Important:</b> these commands are sent "
      "exactly as you type them, with leading/trailing spaces removed. "
      "No aliases, no macros, no speed-walk. Also note that empty lines "
      "will also be sent, including the very last line."));
  
  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  l1->setBuddy (ed1);
  l2->setBuddy (ed2);
  l3->setBuddy (ed3);
  l4->setBuddy (ed4);
  l5->setBuddy (ed5);
  l6->setBuddy (connstr);
  
  layout->setSpacing (10);
//  layout->addWidget (btlist, 0, 0);
  layout->addWidget (l1, 1, 0);
  layout->addWidget (l2, 2, 0);
  layout->addWidget (l3, 3, 0);
  layout->addWidget (l4, 4, 0);
  layout->addWidget (l5, 5, 0);
  layout->addWidget (l6, 6, 0);
  layout->addWidget (ed1, 1, 1);
  layout->addWidget (ed2, 2, 1);
  layout->addWidget (ed3, 3, 1);
  layout->addWidget (ed4, 4, 1);
  layout->addWidget (ed5, 5, 1);
  layout->addWidget (connstr, 7, 0, 1, 2);
  layout->addWidget (buttons, 8, 0, 1, 2);

  ed1->setFocus ();
}

dlgEditProfile::~dlgEditProfile()
{
  //nothing here
}

QSize dlgEditProfile::sizeHint() const
{
  return QSize (350, 350);
}

QString dlgEditProfile::name ()
{
  return ed1->text ();
}

QString dlgEditProfile::server ()
{
  return ed2->text();
}

int dlgEditProfile::port ()
{
  return ed3->value();
}

QString dlgEditProfile::login ()
{
  return ed4->text ();
}

QString dlgEditProfile::password ()
{
  return ed5->text ();
}

QStringList dlgEditProfile::connectionString ()
{
  return connstr->toPlainText().split ("\n");
}

void dlgEditProfile::setName (QString name)
{
  ed1->setText (name);
}

void dlgEditProfile::setServer (QString server)
{
  ed2->setText (server);
}

void dlgEditProfile::setPort (int port)
{
  ed3->setValue (port);
}

void dlgEditProfile::setLogin (QString login)
{
  ed4->setText (login);
}

void dlgEditProfile::setPassword (QString password)
{
  ed5->setText (password);
}

void dlgEditProfile::setConnectionString (QStringList conn)
{
  connstr->setPlainText (conn.join ("\n"));
}

void dlgEditProfile::accept ()
{
  QString s = name ().simplified ();
  bool failed = false;
  if (s == "")
  {
    failed = true;
    KMessageBox::error (this, i18n ("Profile name cannot be empty."), i18n ("Edit profile"));
  }
  //looks like filenames can contain '/' char, but who knows?
  if (s.contains ('/'))
  {
    KMessageBox::error (this, i18n ("Profile name cannot contain '/' character."), i18n ("Edit profile"));
    failed = true;
  }
    
  //if name is okay, accept the dialog...
  if (failed) return;

  QDialog::accept ();
}

#include "moc_dlgeditprofile.cpp"
