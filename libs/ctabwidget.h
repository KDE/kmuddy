//
// C++ Interface: ctabwidget
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CTABWIDGET_H
#define CTABWIDGET_H

#include <QTabWidget>

#include <kmuddy_export.h>

/** cTabWidget - QTabWidget with the ability to show/hide the tab bar and add
 context menus (TODO) */

class KMUDDY_EXPORT cTabWidget : public QTabWidget
{
 public:
  cTabWidget (QWidget *parent = 0);
  virtual ~cTabWidget () {};
  void showTabBar ();
  void hideTabBar ();
};

#endif  // CTABWIDGET_H
