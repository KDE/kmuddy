/***************************************************************************
                          dlgconnect.cpp  -  Connect dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ut Jul 23 2002
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

#include "dlgconnect.h"

#include "dlgeditprofile.h"

#include "cprofilemanager.h"
#include "cprofilesettings.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QVBoxLayout>

#include <kinputdialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

dlgConnect::dlgConnect(QWidget *parent) : QDialog (parent)
{
  setWindowTitle (i18n ("Connect"));

  //create main dialog's widget
  QGridLayout *layout = new QGridLayout (this);

  //put some widgets there
  lw = new QTreeView (this);
  QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel (this);
  proxyModel->setSourceModel (cProfileManager::self()->model());
  proxyModel->setSortCaseSensitivity (Qt::CaseInsensitive);
  lw->setModel (proxyModel);
  lw->setUniformRowHeights (true);
  lw->setRootIsDecorated (false);
  lw->setItemsExpandable (false);
  lw->setSortingEnabled (true);
  lw->sortByColumn (0, Qt::AscendingOrder);
  lw->setWhatsThis(
    i18n ("This list shows currently defined profiles.<p><b>Profiles</b> "
      "allow you to speed up connecting to your MUD, as well as to use "
      "more advanced features like <i>aliases</i> or <i>triggers</i>."));

  QWidget *vb = new QWidget (this);
  QVBoxLayout *vblayout = new QVBoxLayout (vb);
  vblayout->setSpacing (5);
  KPushButton *addButton = new KPushButton (i18n ("&New profile"), vb);
  KPushButton *modifyButton = new KPushButton (i18n ("&Modify profile"), vb);
  KPushButton *deleteButton = new KPushButton (i18n ("&Delete profile"), vb);
  KPushButton *duplicateButton = new KPushButton (i18n ("&Duplicate profile"), vb);
  vblayout->addWidget (addButton);
  vblayout->addWidget (modifyButton);
  vblayout->addWidget (deleteButton);
  vblayout->addWidget (duplicateButton);

  chkSendNothing = new QCheckBox (i18n ("Do not &send login sequence"), this);
  chkSendNothing->setWhatsThis( i18n ("Login sequence won't be sent for "
      "this connect. Useful when you're creating a new character and you "
      "want to use QuickConnect for some reason."));
  chkSendNothing->setChecked (false);
  
  chkOffline = new QCheckBox (i18n ("&Offline editing"), this);
  chkOffline->setWhatsThis( i18n ("This allows offline editing of "
      "profiles."));
  chkOffline->setChecked(false);

  buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  QPushButton *button = buttons->button (QDialogButtonBox::Ok);
  button->setText (i18n ("&Connect"));
  button->setToolTip (i18n ("Establishes connection with specified parameters."));
  button = buttons->button (QDialogButtonBox::Cancel);
  button->setText (i18n ("C&lose"));
  button->setToolTip (i18n ("Closes this dialog box without connecting."));
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->setRowStretch (0, 0);
  layout->setRowStretch (1, 5);
  layout->setSpacing (5);
  
  layout->addWidget (lw, 0, 0, 2, 1);
  layout->addWidget (vb, 0, 1);
  layout->addWidget (chkSendNothing, 2, 0);
  layout->addWidget (chkOffline, 3, 0);
  layout->addWidget (buttons, 4, 0, 1, 2);
  
  //connect signals
  connect (addButton, SIGNAL(clicked()), this, SLOT(addPressed()));
  connect (modifyButton, SIGNAL(clicked()), this, SLOT(modifyPressed()));
  connect (deleteButton, SIGNAL(clicked()), this, SLOT(deletePressed()));
  connect (duplicateButton, SIGNAL(clicked()), this, SLOT(duplicatePressed()));
  
  connect (lw->selectionModel(), SIGNAL (selectionChanged (const QItemSelection &, const QItemSelection &)), this, SLOT (selectionChanged (const QItemSelection &)));
  connect (lw, SIGNAL (doubleClicked (const QModelIndex &)), this, SLOT (doubleClicked (const QModelIndex &)));

  lw->setFocus ();
}

dlgConnect::~dlgConnect()
{
}

QSize dlgConnect::sizeHint() const
{
  return QSize (500, 400);
}

QString dlgConnect::selectedProfile ()
{
  QItemSelection sel = lw->selectionModel()->selection();
  if (sel.empty()) return QString();
  QSortFilterProxyModel *model = dynamic_cast<QSortFilterProxyModel *> (lw->model());
  if (!model) return QString();
  sel = model->mapSelectionToSource (sel);
  int idx = sel.indexes().first().row();
  return cProfileManager::self()->profileList()[idx];
}

bool dlgConnect::sendNothing ()
{
  return chkSendNothing->isChecked ();
}

bool dlgConnect::isOffLine()
{
  return chkOffline->isChecked ();
}

void dlgConnect::selectionChanged (const QItemSelection &index)
{
  //enable/disable Connect button
  QPushButton *button = buttons->button (QDialogButtonBox::Ok);
  button->setEnabled (index.indexes().empty() ? false : true);
}

void dlgConnect::doubleClicked (const QModelIndex &index)
{
  if (index.isValid ())
    accept();
}

void dlgConnect::addPressed ()
{
  //so first we have to create the dialog...
  mdlg = new dlgEditProfile (this);

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (mdlg, SIGNAL (accepted()), this, SLOT (doAdd ()));

  //fill in default login sequence
  QStringList ls;
  ls.append ("$name");
  ls.append ("$password");
  mdlg->setConnectionString (ls);

  //dialog is ready - show it!
  mdlg->exec ();
  delete mdlg;
}

void dlgConnect::modifyPressed ()
{
  cProfileManager *mgr = cProfileManager::self();
  QString profile = selectedProfile();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;   // no profile selected

  //so first we have to create the dialog...
  mdlg = new dlgEditProfile (this);

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (mdlg, SIGNAL (accepted()), this, SLOT (doModify ()));

  mdlg->setName (mgr->visibleProfileName (profile));
  mdlg->setServer (sett->getString ("server"));
  mdlg->setPort (sett->getInt ("port"));
  mdlg->setLogin (sett->getString ("login"));
  mdlg->setPassword (sett->getString ("password"));
  QStringList conn;
  int cnt = sett->getInt ("on-connect-count");
  for (int i = 0; i < cnt; ++i)
    conn << sett->getString ("on-connect-"+QString::number(i));
  mdlg->setConnectionString (conn);

  //dialog is ready - show it!
  mdlg->exec ();

  delete mdlg;
}

void dlgConnect::updateProfileFromDialog (const QString &profile)
{
  cProfileManager *mgr = cProfileManager::self();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;
  sett->setString ("server", mdlg->server());
  sett->setInt ("port", mdlg->port());
  sett->setString ("login", mdlg->login());
  sett->setString ("password", mdlg->password());
  QStringList con = mdlg->connectionString();
  sett->setInt ("on-connect-count", con.size());
  for (int i = 0; i < con.size(); ++i)
    sett->setString ("on-connect-"+QString::number(i), con[i]);
  sett->save();

  mgr->profileInfoChanged (profile);
}

void dlgConnect::deletePressed ()
{
  cProfileManager *mgr = cProfileManager::self();
  QString profile = selectedProfile();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;

  // can we do that ?
  if (mgr->hasSessionAssigned (profile)) {
    KMessageBox::error (this, i18n ("This profile cannot be deleted, because you have a connection open using this profile."), i18n ("Unable to delete"));
    return;
  }

  if (KMessageBox::questionYesNoCancel (this,
        i18n ("Do you really want to delete profile %1?", mgr->visibleProfileName (profile)),
        i18n ("Delete profile")) != KMessageBox::Yes)
    return;

  // wants to delete
  // TODO: offer the option to also delete the files
  mgr->deleteProfile (profile, false);
}

void dlgConnect::duplicatePressed ()
{
  cProfileManager *mgr = cProfileManager::self();
  QString profile = selectedProfile();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;
  
  bool ok;
  QString newName = KInputDialog::getText (i18n ("Duplicate Profile"), i18n ("Please enter name for the duplicated profile"), mgr->visibleProfileName (profile), &ok, this);
  if (!mgr->duplicateProfile (profile, newName))
    KMessageBox::error (this, i18n ("There was an error trying to duplicate the profile. Please ensure that you have write access to the profile directory."), i18n ("Unable to duplicate"));
}

void dlgConnect::doAdd ()
{
  cProfileManager *mgr = cProfileManager::self();
  QString profile = mgr->newProfile (mdlg->name());
  updateProfileFromDialog (profile);
}

void dlgConnect::doModify ()
{
  cProfileManager *mgr = cProfileManager::self();
  QString profile = selectedProfile();
  mgr->renameProfile (profile, mdlg->name());
  updateProfileFromDialog (profile);
}

