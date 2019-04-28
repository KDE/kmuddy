//
// C++ Implementation: clist
//
// Description: Viewer of the object model.
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

#include "clistviewer.h"

#include "clist.h"
#include "clistmanager.h"
#include "clistgroup.h"
#include <kaction.h>
#include <kactioncollection.h>
#include <KLocalizedString>
#include <kinputdialog.h>
#include <kmessagebox.h>

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QIcon>
#include <QMenu>
#include <QStandardItemModel>
#include <QRegExpValidator>

#include <kdebug.h>

struct cListViewer::Private {
  cList *list;
  cListObject *currentItem;
  QStandardItemModel *emptyModel;
  QMenu *objectMenu, *groupMenu, *outsideMenu;
  QRegExpValidator *validator;
  KActionCollection *col;
};

cListViewer::cListViewer (QWidget *parent)
  : QTreeView (parent)
{
  d = new Private;
  d->currentItem = nullptr;
  d->emptyModel = new QStandardItemModel;
  setList (nullptr);
  header()->hide();  // no header
  d->validator = new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this);
  d->col = new KActionCollection (this);

  // create the actions and popup menus
  KAction *actDeleteObj = new KAction (this);
  actDeleteObj->setText (i18n ("Delete object"));
  actDeleteObj->setIcon (QIcon::fromTheme ("list-remove"));
  connect (actDeleteObj, SIGNAL (triggered()), this, SLOT (deleteObject()));
  d->col->addAction ("DeleteObject", actDeleteObj);
  KAction *actDeleteGroup = new KAction (this);
  actDeleteGroup->setText (i18n ("Delete group"));
  connect (actDeleteGroup, SIGNAL (triggered()), this, SLOT (deleteObject()));
  d->col->addAction ("DeleteGroup", actDeleteGroup);
  KAction *actAddSubGroup = new KAction (this);
  actAddSubGroup->setText (i18n ("Add subgroup"));
  actAddSubGroup->setIcon (QIcon::fromTheme ("folder-new"));
  connect (actAddSubGroup, SIGNAL (triggered()), this, SLOT (addGroup()));
  d->col->addAction ("AddSubGroup", actAddSubGroup);
  KAction *actAddGroup = new KAction (this);
  actAddGroup->setText (i18n ("Add group"));
  actAddGroup->setIcon (QIcon::fromTheme ("folder-new"));
  connect (actAddGroup, SIGNAL (triggered()), this, SLOT (addGroup()));
  d->col->addAction ("AddGroup", actAddGroup);
  KAction *actAddObject = new KAction (this);
  actAddObject->setText (i18n ("Add object"));
  actAddObject->setIcon (QIcon::fromTheme ("list-add"));
  connect (actAddObject, SIGNAL (triggered()), this, SLOT (addObject()));
  d->col->addAction ("AddObject", actAddObject);

  d->outsideMenu = new QMenu (this);
  d->outsideMenu->addAction (actAddGroup);
  d->outsideMenu->addAction (actAddObject);
  d->objectMenu = new QMenu (this);
  d->objectMenu->addAction (actDeleteObj);
  d->groupMenu = new QMenu (this);
  d->groupMenu->addAction (actAddObject);
  d->groupMenu->addAction (actAddSubGroup);
  d->groupMenu->addAction (actDeleteGroup);

  setSelectionMode (SingleSelection);

  setDragDropMode (InternalMove);
  setDragEnabled (true);
  setAcceptDrops (true);
  setDropIndicatorShown (true);
}

cListViewer::~cListViewer ()
{
  delete d->col;
  delete d->validator;
  delete d->emptyModel;
  delete d;
}

void cListViewer::setList (cList *l)
{
  d->list = l;
  d->currentItem = nullptr;
  setModel (l ? l->model() : d->emptyModel);
}

void cListViewer::selectObject (cListObject *obj)
{
  QModelIndex index = obj->list()->indexOf (obj);
  scrollTo (index);
  setCurrentIndex (index);
}

KActionCollection *cListViewer::actionCollection ()
{
  return d->col;
}

void cListViewer::currentChanged (const QModelIndex &current, const QModelIndex &)
{
  if (!d->list) return;
  d->currentItem = d->list->objectAt (current);
  emit itemActivated (d->currentItem);
}

void cListViewer::contextMenuEvent (QContextMenuEvent *event)
{
  if (!d->list) return;
  // we need to figure out which object we clicked on, and display the
  // appropriate context menu
  QMenu *menu = d->outsideMenu;
  QModelIndex index = indexAt (event->pos());
  if (index.isValid()) {
    d->currentItem = d->list->objectAt (index);
    menu = d->currentItem->isGroup() ? d->groupMenu : d->objectMenu;
  }
  menu->exec (event->globalPos());
}

void cListViewer::deleteObject () {
  if (!d->list) return;
  // verify that the object still exists
  if (!cListManager::self()->objectId (d->currentItem)) return;
  
  // ask for confirmation
  QString message1;
  QString message2;
  if (d->currentItem->isGroup()) {
    message1 = i18n ("Do you really want to delete this group?");
    message2 = i18n ("Delete group");
  } else {
    message1 = i18n ("Do you really want to delete this %1?", d->list->objName());
    message2 = i18n ("Delete %1", d->list->objName());
  }
  if (KMessageBox::questionYesNo (this, message1, message2) != KMessageBox::Yes) return;

  // verify that the object still exists
  if (!cListManager::self()->objectId (d->currentItem)) return;

  // delete the object
  if (d->currentItem->isGroup())
    d->list->removeGroup ((cListGroup *) d->currentItem);
  else
    d->list->deleteObject (d->currentItem);
}

void cListViewer::addGroup () {
  if (!d->list) return;
  cListObject *obj = d->currentItem;
  // nothing selected ? use root
  if (!obj) obj = d->list->rootGroup();
  // verify that the group still exists
  if (!cListManager::self()->objectId (obj)) return;
  // selected object not a group ? use its parent
  if ((!obj->isGroup())) obj = obj->parentGroup();

  // ask for group name
  bool ok = false;
  QString name = KInputDialog::getText (i18n ("Create Group"), i18n ("Please enter the group name:"), QString(), &ok, this, d->validator);
  if (!ok) return;

  // check if such group exists yet
  cListGroup *g = d->list->group (name);
  if (g) {
    KMessageBox::sorry (this, i18n ("Cannot create the group, as a group with this name already exists."));
    return;
  }

  // verify that the group still exists
  if (!cListManager::self()->objectId (obj)) return;

  // add a subgroup to the given group
  d->list->addGroup ((cListGroup *) obj, name);
  // and select it
  selectObject (d->list->group (name));
}

void cListViewer::addObject () {
  if (!d->list) return;
  cListObject *obj = d->currentItem;
  if (!obj) obj = d->list->rootGroup();  // nothing selected ? use root
  // verify that the group still exists
  if (!cListManager::self()->objectId (obj)) return;
  // selected object not a group ? use its parent
  if ((!obj->isGroup())) obj = obj->parentGroup();

  // create the object
  cListObject *o = d->list->newObject();
  d->list->addToGroup ((cListGroup *) obj, o);
  // and select it
  selectObject (o);
}

void cListViewer::moveDown ()
{
  cListObject *obj = d->currentItem;
  if (!obj) return;
  obj->parentGroup()->moveObjectDown (obj->positionInGroup());
  selectObject (obj);
}

void cListViewer::moveUp ()
{
  cListObject *obj = d->currentItem;
  if (!obj) return;
  obj->parentGroup()->moveObjectUp (obj->positionInGroup());
  selectObject (obj);
}

void cListViewer::moveLeft ()
{
  cListObject *obj = d->currentItem;
  if (!obj) return;
  // find parent group - it must not be the root group
  cListGroup *parent = obj->parentGroup();
  if (parent == d->list->rootGroup()) return;

  // move the object to the parent's parent, under the current parent
  int pos = parent->positionInGroup ();
  cList *list = obj->list();
  list->addToGroup (parent->parentGroup(), obj);
  parent->parentGroup()->moveObjectToPosition (obj, pos + 1);
  selectObject (obj);
}

void cListViewer::moveRight ()
{
  cListObject *obj = d->currentItem;
  if (!obj) return;
  // the object above us must be a group
  // we become the last child of that group
  int pos = obj->positionInGroup ();
  if (pos == 0) return;
  cListObject *g = obj->parentGroup()->objectAt (pos - 1);
  if (!g->isGroup()) return;
  obj->list()->addToGroup ((cListGroup *) g, obj);

  selectObject (obj);
}

