/***************************************************************************
                          dlgimportexportprofile.cpp  -  description
                             -------------------
    begin                : Ne júl 20 2003
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

#include "dlgimportexportprofile.h"

#include <KLocalizedString>
#include <kmessagebox.h>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

dlgImportExportProfile::dlgImportExportProfile (bool isImport,
    QWidget *parent) : QDialog (parent)
{
  import = isImport;
  
  //initial dialog size
  setWindowTitle (isImport ? i18n ("Import profile") : i18n ("Export profile"));

  QGridLayout *layout = new QGridLayout (this);

  QLabel *l1 = new QLabel (i18n ("&Profile name"), this);
  if (isImport)
  {
    edprofile = new QLineEdit (this);
    l1->setBuddy (edprofile);
  }
  else
  {
    cbprofile = new QComboBox (this);
    //cProfiles p;
    //cbprofile->addItems (p.getprofiles());
    l1->setBuddy (cbprofile);
  }
  QLabel *l2 = new QLabel (isImport ? i18n ("&Import from") :
      i18n ("&Export to"), this);
  edfname = new QLineEdit (this);
  l2->setBuddy (edfname);
  QPushButton *button = new QPushButton (i18n ("Browse..."), this);

  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->setSpacing (10);
  layout->addWidget (l1, 0, 0);
  layout->addWidget (l2, 1, 0);
  if (isImport)
    layout->addWidget (edprofile, 0, 1);
  else
    layout->addWidget (cbprofile, 0, 1);
  layout->addWidget (edfname, 1, 1);
  layout->addWidget (button, 1, 2);
  layout->addWidget (buttons, 0, 3, 0, 2);
  layout->setRowStretch (2, 10);

  connect (button, &QPushButton::clicked, this, &dlgImportExportProfile::browse);
}                                                                       

dlgImportExportProfile::~dlgImportExportProfile ()
{

}

QSize dlgImportExportProfile::sizeHint() const
{
  return QSize (300, 150);
}

void dlgImportExportProfile::doThings ()
{
  KMessageBox::error (this, i18n ("I am sorry, but import and export is currently disabled. The functionality should be re-enabled before the final release."));
  /*
  if (exec() == QDialog::Accepted)
  {
    QString fName = edfname->text();
    QString pName;
    if (import)
      pName = edprofile->text ();
    else
      pName = cbprofile->currentText ();
    QString msg;
    cProfiles p;
    if (import)
    {
      if (p.importProfile (pName, fName))
        msg = QString();
      else
        msg = p.lastError();
    }
    else
    {
      if (p.exportProfile (pName, fName))
        msg = QString();
      else
        msg = p.lastError();
    }
    if (msg == QString())
      KMessageBox::information (this, import ?
        i18n ("Profile has been successfully imported.") :
        i18n ("Profile has been successfully exported."));
    else
      KMessageBox::error (this, msg);
  }
  */
}

void dlgImportExportProfile::browse ()
{
  if (import)
  {
    QString n = QFileDialog::getOpenFileName (this, i18n ("Choose file with profile"));
    if (!(n.isEmpty()))
      edfname->setText (n);
  }
  else
  {
    QString n = QFileDialog::getSaveFileName (this, i18n ("Enter target file"));
    if (!(n.isEmpty()))
      edfname->setText (n);
  }
}

#include "moc_dlgimportexportprofile.cpp"
