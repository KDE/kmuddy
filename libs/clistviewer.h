//
// C++ Interface: clistviewer
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  virtual void currentChanged (const QModelIndex &current, const QModelIndex &previous);
 protected:
  /** Context menu handler. */
  virtual void contextMenuEvent (QContextMenuEvent *event);
 private:
  struct Private;
  Private *d;
};


#endif  // CLISTVIEWER_H
