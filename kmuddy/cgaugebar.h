//
// C++ Interface: cgaugebar
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
