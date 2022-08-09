//
// C++ Interface: ctabwidget
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

#ifndef CTABWIDGET_H
#define CTABWIDGET_H

#include <QTabWidget>

#include <kmuddy_export.h>

/** cTabWidget - QTabWidget with the ability to show/hide the tab bar and add
 context menus (TODO) */

class KMUDDY_EXPORT cTabWidget : public QTabWidget
{
 public:
  cTabWidget (QWidget *parent = nullptr);
  virtual ~cTabWidget () {};
  void showTabBar ();
  void hideTabBar ();
};

#endif  // CTABWIDGET_H
