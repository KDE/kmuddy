//
// C++ Interface: dlgwindows
//
// Description: 
//
//
// Author: Vladimir Lazarenko <vlad@lazarenko.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGWINDOWS_H
#define DLGWINDOWS_H

#include <QLabel>
#include <kdialog.h>

class QLabel;
class QListWidget;
class KPushButton;
class QStringList;

class cWindowList;

/**
@author Vladimir Lazarenko
*/
class dlgWindows : public KDialog
{
Q_OBJECT
public:
  dlgWindows(cWindowList *wlist, QWidget *parent = 0);

  ~dlgWindows();
    
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
  KPushButton *btshow, *bthide, *btdelete;
  

};

#endif
