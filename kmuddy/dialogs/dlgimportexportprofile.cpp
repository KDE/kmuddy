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

#include <kfiledialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <QGridLayout>

dlgImportExportProfile::dlgImportExportProfile (bool isImport,
    QWidget *parent) : KDialog (parent)
{
  import = isImport;
  
  //initial dialog size
  setInitialSize (QSize (300, 150));
  setCaption (isImport ? i18n ("Import profile") : i18n ("Export profile"));
  setButtons (KDialog::Ok | KDialog::Cancel);

  //create main dialog's widget
  QWidget *page = new QWidget (this);
  QGridLayout *layout = new QGridLayout (page);

  setMainWidget (page);

  QLabel *l1 = new QLabel (i18n ("&Profile name"), page);
  if (isImport)
  {
    edprofile = new KLineEdit (page);
    l1->setBuddy (edprofile);
  }
  else
  {
    cbprofile = new QComboBox (page);
    //cProfiles p;
    //cbprofile->addItems (p.getprofiles());
    l1->setBuddy (cbprofile);
  }
  QLabel *l2 = new QLabel (isImport ? i18n ("&Import from") :
      i18n ("&Export to"), page);
  edfname = new KLineEdit (page);
  l2->setBuddy (edfname);
  KPushButton *button = new KPushButton (i18n ("Browse..."), page);
  layout->setSpacing (10);
  layout->addWidget (l1, 0, 0);
  layout->addWidget (l2, 1, 0);
  if (isImport)
    layout->addWidget (edprofile, 0, 1);
  else
    layout->addWidget (cbprofile, 0, 1);
  layout->addWidget (edfname, 1, 1);
  layout->addWidget (button, 1, 2);
  layout->setRowStretch (2, 10);

  connect (button, SIGNAL (clicked()), this, SLOT (browse ()));
}                                                                       

dlgImportExportProfile::~dlgImportExportProfile ()
{

}

void dlgImportExportProfile::doThings ()
{
  KMessageBox::sorry (this, i18n ("I am sorry, but import and export is currently disabled. The functionality should be re-enabled before the final release."));
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
      KMessageBox::sorry (this, msg);
  }
  */
}

void dlgImportExportProfile::browse ()
{
  if (import)
  {
    QString n = KFileDialog::getOpenFileName (QString(),
        QString(), this, i18n ("Choose file with profile"));
    if (!(n.isEmpty()))
      edfname->setText (n);
  }
  else
  {
    QString n = KFileDialog::getSaveFileName (QString(),
        QString(), this, i18n ("Enter target file"));
    if (!(n.isEmpty()))
      edfname->setText (n);
  }
}

#include "dlgimportexportprofile.moc"

