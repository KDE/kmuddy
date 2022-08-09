//
// C++ Interface: dlgwindows
//
// Description: 
//
/*
Copyright 2004 Vladimir Lazarenko <vlad@lazarenko.net>

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

#ifndef DLGWINDOWS_H
#define DLGWINDOWS_H

#include <QDialog>
#include <QLabel>

class QListWidget;
class QPushButton;
class QStringList;

class cWindowList;

/**
@author Vladimir Lazarenko
*/
class dlgWindows : public QDialog
{
Q_OBJECT
public:
  dlgWindows(cWindowList *wlist, QWidget *parent = nullptr);
  ~dlgWindows();

  virtual QSize sizeHint() const override;
    
protected slots:
  void wshow();
  void whide();
  void remove();
  
protected:
  void createDialog();
  void updateMe();
  
  QStringList wl;
  
  cWindowList *winlist;
  
  QListWidget *box;
  QPushButton *btshow, *bthide, *btdelete;
  

};

#endif
