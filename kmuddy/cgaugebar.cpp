//
// C++ Implementation: cgaugebar
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cgaugebar.h"

#include "cgauge.h"
#include "cgaugebaritem.h"
#include "cgaugelist.h"

#include <QHBoxLayout>

cGaugeBar::cGaugeBar (int sess, QWidget *parent)
 : QWidget(parent), cActionBase ("gaugebar", sess)
{
  layout = new QHBoxLayout (this);
  layout->setSpacing (5);
  layout->setContentsMargins (2, 2, 2, 2);
  setFocusPolicy (Qt::NoFocus);
  count = 0;
  layout->addStretch (1);
  setAutoFillBackground (true);
}


cGaugeBar::~cGaugeBar ()
{
}

void cGaugeBar::addGauge (cGauge *g)
{
  layout->insertWidget (count, g->gaugeItem());
  g->gaugeItem()->show();
  count++;
  update ();
}

void cGaugeBar::removeGauge (cGauge *g)
{
  if (!g->gaugeItem()) return;
  layout->removeWidget (g->gaugeItem());
  g->gaugeItem()->hide();
  if (count) count--;
  update ();
}

#include "moc_cgaugebar.cpp"
