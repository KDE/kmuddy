//
// C++ Implementation: cgaugebar
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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

#include "cgaugebar.moc"
