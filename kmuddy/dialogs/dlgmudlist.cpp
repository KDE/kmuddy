//
// C++ Implementation: dlgmudlist
//
// Description: A dialog displaying the list of MUDs from the MUD Connector database.
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

#include "dlgmudlist.h"

#include "cmudlist.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QTextDocument>
#include <QTreeView>

#include <klineedit.h>
#include <klocale.h>
#include <ktextbrowser.h>

cMUDList *dlgMudList::lst = nullptr;

struct dlgMudList::Private {
  KLineEdit *filter;
  QTreeView *view;
  KTextBrowser *details;
  // model used to handle sorting
  QSortFilterProxyModel *proxy;
};

dlgMudList::dlgMudList (QWidget *parent) : QDialog (parent)
{
  d = new Private;
  if (!lst)
    // This is a SLOW operation, as it loads the big XML.
    lst = new cMUDList;

  // initialize the dialog
  setWindowTitle (i18n ("MUD Listing"));

  //create main dialog's widget
  QVBoxLayout *mainLayout = new QVBoxLayout (this);

  QSplitter *page = new QSplitter (this);
  page->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
  QWidget *left = new QWidget (page);
  QGridLayout *layout = new QGridLayout (left);
  d->filter = new KLineEdit (left);
  QLabel *lbl = new QLabel (i18n ("&Filter:"), left);
  lbl->setBuddy (d->filter);
  d->view = new QTreeView (left);
  layout->addWidget (lbl, 0, 0);
  layout->addWidget (d->filter, 0, 1);
  layout->addWidget (d->view, 1, 0, 1, 2);
  d->details = new KTextBrowser (page);
  page->addWidget (left);
  page->addWidget (d->details);
  QLabel *source = new QLabel ("The list is provided courtesy of <a href=\"http://www.mudconnector.com/\">The MUD Connector</a>.<br/>If you are unsure which game to pick, you may also want to check out <a href=\"http://www.topmudsites.com\">www.topmudsites.com</a>.", this);
  source->setOpenExternalLinks (true);
  source->setWordWrap (true);

  QDialogButtonBox *buttons = new QDialogButtonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect (buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

  mainLayout->addWidget (page);
  mainLayout->addWidget (source);
  mainLayout->addWidget (buttons);

  d->view->setRootIsDecorated (false);
  d->view->setItemsExpandable (false);
  d->proxy = new QSortFilterProxyModel (this);
  d->proxy->setSourceModel (lst->model ());
  d->proxy->setFilterCaseSensitivity (Qt::CaseInsensitive);
  d->proxy->setSortCaseSensitivity (Qt::CaseInsensitive);
  d->view->setModel (d->proxy);
  d->view->setSortingEnabled (true);
  d->view->sortByColumn (0, Qt::AscendingOrder);
  d->view->resizeColumnToContents (0);

  connect (d->filter, SIGNAL (textChanged (const QString &)), d->proxy, SLOT (setFilterFixedString (const QString &)));
  connect (d->view->selectionModel(), SIGNAL (currentChanged (const QModelIndex &, const QModelIndex &)), this, SLOT (currentChanged (const QModelIndex &)));
  connect (d->view, SIGNAL (doubleClicked (const QModelIndex &)), this, SLOT (accept ()));
}

dlgMudList::~dlgMudList ()
{
  delete d;
}

QSize dlgMudList::sizeHint() const
{
  return QSize (750, 500);
}

const cMUDEntry *dlgMudList::getEntry (QWidget *parent)
{
  dlgMudList *dlg = new dlgMudList (parent);
  if (dlg->exec() != QDialog::Accepted) return nullptr;
  // obtain the selected entry
  const cMUDEntry *e = dlg->selectedEntry ();
  delete dlg;
  return e;
}

void dlgMudList::currentChanged (const QModelIndex &index)
{
  QModelIndex idx = d->proxy->mapToSource (index);
  const cMUDEntry *e = lst->entry (idx.row());
  if (!e) return;

  // place the data into the details viewer
  QTextDocument *doc = d->details->document();
  QString html;
  html = "<html><body><table border=0 cellpadding=10 cellspacing=1 width=100%>";
  html += "<tr bgcolor=#E0E0E0><td><b>Name</b></td><td>"+Qt::escape(e->name)+"</td></tr>";
  html += "<tr bgcolor=#D7D7D7><td><b>Host</b></td><td>"+Qt::escape(e->host)+":"+QString::number(e->port);
  if (!e->ip.isEmpty())
    html += " ("+Qt::escape (e->ip)+")";
  html += "</td></tr>";
  if (!e->www.isEmpty())
    html += "<tr bgcolor=#E0E0E0><td><b>URL</b></td><td><a href=\""+Qt::escape(e->www)+"\">"+Qt::escape(e->www)+"</a></td></tr>";
  html += "<tr bgcolor=#D7D7D7><td><b>Codebase</b></td><td>"+Qt::escape(e->codebase)+"</td></tr>";
  html += "</table>";
  QStringList::const_iterator it;
  for (it = e->desc.begin(); it != e->desc.end(); ++it)
    html += "<div style=\"margin-bottom:5px\">"+Qt::escape (*it)+"</div>";
  html += "</body></html>";
  doc->setHtml (html);
}

const cMUDEntry *dlgMudList::selectedEntry ()
{
  QItemSelectionModel *sm = d->view->selectionModel();
  if (!sm->hasSelection())
    return nullptr;
  int row = sm->selectedRows().first().row();
  // we have the row in the view now, so now we need to map it to the row in the original model
  QModelIndex idx = d->proxy->index (row, 0);
  return lst->entry (d->proxy->mapToSource (idx).row());
}

#include "moc_dlgmudlist.cpp"
