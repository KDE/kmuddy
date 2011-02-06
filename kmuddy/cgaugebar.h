//
// C++ Interface: cgaugebar
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGAUGEBAR_H
#define CGAUGEBAR_H

#include <qwidget.h>

#include "cactionbase.h"

class cGauge;
class cGaugeList;

class QHBoxLayout;

/**
The gauge bar.

@author Tomas Mecir
*/
class cGaugeBar : public QWidget, public cActionBase
{
Q_OBJECT
 public:
  /** constructor */
  cGaugeBar (int sess, QWidget *parent = 0);
  /** destructor */
  ~cGaugeBar ();

  void addGauge (cGauge *g);
  void removeGauge (cGauge *g);
  int gauges() { return count; }
 protected:
  QHBoxLayout *layout;
  int count;
};

#endif
