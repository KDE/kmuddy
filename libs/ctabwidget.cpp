//
// C++ Implementetion: ctabwidget
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ctabwidget.h"

#include <QTabBar>

cTabWidget::cTabWidget (QWidget *parent) : QTabWidget (parent)
{
}

void cTabWidget::showTabBar ()
{
  tabBar()->show();
}

void cTabWidget::hideTabBar ()
{
  tabBar()->hide();
}


