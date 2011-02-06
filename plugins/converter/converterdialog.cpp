//
// C++ Implementation: converterdialog
//
// Description: Converter Dialog.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "converterdialog.h"

#include <klocale.h>

#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

struct cConverterDialog::Private {
  QTreeWidget *tree;
};

cConverterDialog::cConverterDialog (QWidget *parent) : KDialog (parent)
{
  d = new Private;

  setInitialSize (QSize (500, 400));
  setCaption (i18n ("Profile Converter"));

  setButtons (KDialog::Ok | KDialog::Cancel);

  //create main dialog's widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);
  setMainWidget (page);

  QLabel *label = new QLabel (i18n ("Pick profiles that you would like to convert and press Ok. This tool will create a new profile containing the information from the chosen profiles. Existing profiles will not be overwritten, even if they have the same name. The old profiles will be kept intact."), this);
  label->setWordWrap (true);
  
  d->tree = new QTreeWidget (page);
  d->tree->setColumnCount (2);
  d->tree->setHeaderLabels (QStringList() << i18n ("Name") << i18n ("Path"));
  d->tree->setRootIsDecorated (false);
  d->tree->setUniformRowHeights (true);
  d->tree->setItemsExpandable (false);

  layout->addWidget (label);
  layout->addWidget (d->tree);
}

cConverterDialog::~cConverterDialog ()
{
  delete d;
}

bool cConverterDialog::display (std::list<ConversionDialogItem *> items)
{
  d->tree->clear ();
  std::list<ConversionDialogItem *>::iterator it;
  for (it = items.begin(); it != items.end(); ++it) {
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText (0, (*it)->name);
    item->setText (1, (*it)->path);
    item->setFlags (Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState (0, Qt::Unchecked);
    d->tree->addTopLevelItem (item);
  }

  // execute the dialog
  if (!exec()) return false;

  // update the checked flags
  int idx = 0;
  QTreeWidgetItem *root = d->tree->invisibleRootItem ();
  for (it = items.begin(); it != items.end(); ++it) {
    QTreeWidgetItem *item = root->child (idx);
    (*it)->convert = (item->checkState(0) == Qt::Checked);
    ++idx;
  }
  return true;
}


#include "converterdialog.moc"
