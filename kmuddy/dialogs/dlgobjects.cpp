//
// C++ Interface: dlgobjects
//
// Description: Object Manager
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

#include "dlgobjects.h"

#include "cactionmanager.h"
#include "clist.h"
#include "clisteditor.h"
#include "clistgroupeditor.h"
#include "clistmanager.h"
#include "clistviewer.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kdebug.h>

#include <QComboBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardItemModel>

struct dlgObjects::Private
{
  QComboBox *lists;
  KToolBar *toolbar;
  QStackedWidget *editorStack;
  cListEditor *objEditor, *groupEditor;
  QWidget *emptyEditor;
  cListViewer *viewer;
  QString currentList;
  int currentSession;

  KAction *aUp, *aDown, *aLeft, *aRight;
};

dlgObjects::dlgObjects (QWidget *parent)
  : QDialog (parent), cActionBase ("dialog-objects", 0)
{
  d = new Private;

  //initial dialog size
  setWindowTitle (i18n ("Object Manager"));
  setButtons (KDialog::Close);

  //create main dialog's widget
  QSplitter *page = new QSplitter (this);

  setMainWidget (page);

  QWidget *left = new QWidget (page);
  d->editorStack = new QStackedWidget (page);

  QVBoxLayout *layout = new QVBoxLayout (left);

  layout->setSpacing (5);

  d->toolbar = new KToolBar (left, false, false);
  d->toolbar->setToolButtonStyle (Qt::ToolButtonIconOnly);

  d->lists = new QComboBox (left);
  d->viewer = new cListViewer (left);

  d->emptyEditor = new QWidget (d->editorStack);
  d->objEditor = 0;
  d->groupEditor = new cListGroupEditor (d->editorStack);
  d->editorStack->addWidget (d->emptyEditor);
  d->editorStack->addWidget (d->groupEditor);

  d->aUp = new KAction (this);
  d->aUp->setText (i18n ("Up"));
  d->aUp->setIcon (KIcon ("arrow-up"));
  connect (d->aUp, SIGNAL (triggered()), d->viewer, SLOT (moveUp()));
  d->aDown = new KAction(this);
  d->aDown->setText (i18n ("Down"));
  d->aDown->setIcon (KIcon ("arrow-down"));
  connect (d->aDown, SIGNAL (triggered()), d->viewer, SLOT (moveDown()));
  d->aLeft = new KAction (this);
  d->aLeft->setText (i18n ("Left"));
  d->aLeft->setIcon (KIcon ("arrow-left"));
  connect (d->aLeft, SIGNAL (triggered()), d->viewer, SLOT (moveLeft()));
  d->aRight = new KAction (this);
  d->aRight->setText (i18n ("Right"));
  d->aRight->setIcon (KIcon ("arrow-right"));
  connect (d->aRight, SIGNAL (triggered()), d->viewer, SLOT (moveRight()));

  KActionCollection *col = d->viewer->actionCollection ();
  d->toolbar->addAction (col->action ("AddGroup"));
  d->toolbar->addAction (col->action ("AddObject"));
  d->toolbar->addAction (col->action ("DeleteObject"));

  d->toolbar->addSeparator ();
  d->toolbar->addAction (d->aUp);
  d->toolbar->addAction (d->aDown);
  d->toolbar->addAction (d->aLeft);
  d->toolbar->addAction (d->aRight);

  layout->addWidget (d->lists);
  layout->addWidget (d->toolbar);
  layout->addWidget (d->viewer);

  page->addWidget (left);
  page->addWidget (d->editorStack);

  connect (d->lists, SIGNAL (currentIndexChanged (int)),
      this, SLOT (listChanged (int)));
  connect (d->viewer, SIGNAL (itemActivated (cListObject *)), this, SLOT (activeObjectChanged (cListObject *)));
  connect (this, SIGNAL (closeClicked ()), this, SLOT (saveChanges ()));

  d->currentSession = cActionManager::self()->activeSession ();

  d->lists->setModel (cListManager::self()->typeModel());

  addEventHandler ("connected", 50, PT_NOTHING);
  addEventHandler ("disconnected", 50, PT_NOTHING);
  addEventHandler ("session-activated", 50, PT_INT);
}

dlgObjects::~dlgObjects ()
{
  removeEventHandler ("session-activated");
  removeEventHandler ("connected");
  removeEventHandler ("disconnected");

  delete d;
}

QSize dlgObjects::sizeHint() const
{
  return QSize (600, 400);
}

void dlgObjects::eventNothingHandler (QString event, int session)
{
  if (event == "connected") {
    if (d->currentSession == session)
      switchList ();
  }
  else if (event == "disconnected") {
    if (d->currentSession == session)
      switchList ();
  }
}

void dlgObjects::eventIntHandler (QString event, int, int par1, int)
{
  if (event == "session-activated") {
    d->currentSession = par1;
    switchList ();
  }
}

void dlgObjects::listChanged (int index)
{
  // save changes in the current object, if any
  saveChanges ();

  QStandardItemModel *model = cListManager::self()->typeModel();
  QString listName = model->item (index)->data().toString();
  d->currentList = listName;
  switchList ();
}

void dlgObjects::switchList ()
{
  cList *list = cListManager::self()->getList (d->currentSession, d->currentList);

  if (d->objEditor) {
    d->editorStack->removeWidget (d->objEditor);
    delete d->objEditor;
    d->objEditor = 0;
  }
  if (list) {
    d->objEditor = list->editor (this);
    d->editorStack->addWidget (d->objEditor);
  }
  d->editorStack->setCurrentWidget (d->emptyEditor);
  
  d->viewer->setList (list);

  kDebug() << "Switching active list: " << d->currentList << " in session " << d->currentSession << endl;
}

void dlgObjects::saveChanges ()
{
  cListEditor *editor = dynamic_cast<cListEditor *>(d->editorStack->currentWidget ());
  if (!editor) return;  // no object editor is active

  if (editor->changed ()) {
    // TODO: ask the user ?
    editor->saveChanges ();
  }
}

void dlgObjects::activeObjectChanged (cListObject *obj)
{
  // save changes in the current object, if any
  saveChanges ();

  // clear the current object from editors
  d->groupEditor->setObject (0);
  d->objEditor->setObject (0);

  if ((!obj) || (obj == (cListObject *) obj->list()->rootGroup())) {
    // no object - present the empty widget
    d->editorStack->setCurrentWidget (d->emptyEditor);
    return;
  }

  // fill in the proper editor
  cListEditor *editor = obj->isGroup() ? d->groupEditor : d->objEditor;
  editor->setObject (obj);

  // activate the proper editor
  d->editorStack->setCurrentWidget (editor);

  kDebug () << "Switched active object to " << obj->name() << "." << endl;
}

