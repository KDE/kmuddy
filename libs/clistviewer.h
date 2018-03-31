//
// C++ Interface: clistviewer
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

#ifndef CLISTVIEWER_H
#define CLISTVIEWER_H

#include <kmuddy_export.h>

#include <QTreeView>

class cList;
class cListObject;

class KActionCollection;

class KMUDDY_EXPORT cListViewer: public QTreeView {
 Q_OBJECT
 public:
  /** constructor */
  cListViewer (QWidget *parent);
  /** destructor */
  virtual ~cListViewer ();

  void setList (cList *l);
  void selectObject (cListObject *obj);

  KActionCollection *actionCollection ();

 signals:
  /** An object has been selected. */
  void itemActivated (cListObject *obj);
 public slots:
  void deleteObject ();
  void addGroup ();
  void addObject ();
  void moveDown ();
  void moveUp ();
  void moveLeft ();
  void moveRight ();
 protected slots:
  virtual void currentChanged (const QModelIndex &current, const QModelIndex &previous) override;
 protected:
  /** Context menu handler. */
  virtual void contextMenuEvent (QContextMenuEvent *event) override;
 private:
  struct Private;
  Private *d;
};


#endif  // CLISTVIEWER_H
