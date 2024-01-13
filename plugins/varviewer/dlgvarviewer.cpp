//
// C++ Implementation: dlgvarviewer
//
// Description: 
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

#include "dlgvarviewer.h"

#include "cactionmanager.h"
#include "cvariablelist.h"

#include <KLocalizedString>

#include <QAbstractTableModel>
#include <QTreeView>

#include <map>

// this model holds and presents the variables and their values
class VariableModel : public QAbstractTableModel {
 public:
  VariableModel () {
    list = nullptr;
    count = 0;
  }

  int columnCount (const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return 2;
  }

  int rowCount (const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid()) return 0;
    return count;
  }

  QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override
  {
    if (role != Qt::DisplayRole) return QVariant();
    if (index.parent().isValid()) return QVariant();
    int row = index.row();
    int col = index.column();
    if ((row < 0) || (row >= count)) return QVariant();
    if ((col < 0) || (col >= 2)) return QVariant();
    QString name = vars[row];
    if (col == 0) return name;
    return list->getValue (name);
  }

  QVariant headerData (int section, Qt::Orientation, int role = Qt::DisplayRole) const override
  {
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
      case 0: return i18n ("Variable");
      case 1: return i18n ("Value");
      default: return QVariant();
    }
  }

  void listChanged (cVariableList *newList) {
    pos.clear ();
    count = 0;
    list = newList;
    if (list) {
      // load variable data
      vars = list->getList ();
      for (QStringList::iterator it = vars.begin(); it != vars.end(); ++it)
        pos[*it] = count++;
    }
    reset ();
  }

  void variableChanged (const QString &name) {
    if (!list) return;
    // check possible situations
    if (!list->exists (name)) {
      // variable just got deleted
      // TODO: only remove the correct row instead of resetting everything
      listChanged (list);
    } else if (!pos.count (name)) {
      // variable just got created
      // TODO: only add the correct row instead of resetting everything
      listChanged (list);
    } else {
      // an existing variable was changed
      int row = pos[name];
      emit dataChanged (index (row, 0, QModelIndex()), index (row, 1, QModelIndex()));
    }

  }

 private:
  cVariableList *list;
  std::map<QString, int> pos;  // positions of variables
  QStringList vars;
  int count;
};


dlgVarViewer::dlgVarViewer (QWidget *parent) : QDockWidget (parent), cActionBase ("variable-viewer", 0)
{
  model = new VariableModel;
  createDialog ();

  addEventHandler ("connected", 200, PT_NOTHING);
  addEventHandler ("disconnected", 200, PT_NOTHING);
  addEventHandler ("session-activated", 200, PT_INT);
  addEventHandler ("var-changed", 200, PT_STRING);
}

dlgVarViewer::~dlgVarViewer ()
{
  removeEventHandler ("connected");
  removeEventHandler ("disconnected");
  removeEventHandler ("session-activated");
  removeEventHandler ("var-changed");

  delete model;
}

void dlgVarViewer::createDialog ()
{
  // setInitialSize (QSize (300, 200));
  setWindowTitle (i18n ("Variables"));

  viewer = new QTreeView (this);
  viewer->setAllColumnsShowFocus (true);
  viewer->setRootIsDecorated (false);
  viewer->setUniformRowHeights (true);
  viewer->setModel (model);

  setWidget (viewer);
  
  //no focus - we don't want this dialog to get focus
  setFocusPolicy (Qt::NoFocus);
  viewer->setFocusPolicy (Qt::NoFocus);
}

void dlgVarViewer::eventNothingHandler (QString event, int sess)
{
  if (!isVisible()) return;  // nothing if we aren't shown
  cActionManager *am = cActionManager::self();
  if (am->activeSession() != sess) return;  // nothing if it's not the active session

  if (event == "connected") {
    cActionManager *am = cActionManager::self();
    cVariableList *vars = dynamic_cast<cVariableList *>(am->object ("variables", sess));
    model->listChanged (vars);
  }
  if (event == "disconnected") {
    model->listChanged (nullptr);
  }
}

void dlgVarViewer::eventIntHandler (QString event, int, int val, int)
{
  if (!isVisible()) return;  // nothing if we aren't shown

  if (event == "session-activated") {
    cActionManager *am = cActionManager::self();
    cVariableList *vars = dynamic_cast<cVariableList *>(am->object ("variables", val));
    model->listChanged (vars);
  }
}

void dlgVarViewer::eventStringHandler (QString event, int sess, QString &par1, const QString &)
{
  if (!isVisible()) return;  // nothing if we aren't shown
  cActionManager *am = cActionManager::self();
  if (am->activeSession() != sess) return;  // nothing if it's not the active session

  if (event == "var-changed") {
    model->variableChanged (par1);
  }
}

void dlgVarViewer::showEvent (QShowEvent *)
{
  // when the dialog is shown, we need to reset the data
  cActionManager *am = cActionManager::self();
  int sess = am->activeSession ();
  cVariableList *vars = dynamic_cast<cVariableList *>(am->object ("variables", sess));
  model->listChanged (vars);
}

#include "moc_dlgvarviewer.cpp"
