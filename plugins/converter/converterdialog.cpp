//
// C++ Implementation: converterdialog
//
// Description: Converter Dialog.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "converterdialog.h"

#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

struct cConverterDialog::Private {
  QTreeWidget *tree;
};

cConverterDialog::cConverterDialog (QWidget *parent) : QDialog (parent)
{
  d = new Private;

  resize (QSize (500, 400));
  setWindowTitle (i18n ("Profile Converter"));

  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

  //create main dialog's widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);

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
  layout->addWidget (buttons);
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

#include "moc_converterdialog.cpp"
