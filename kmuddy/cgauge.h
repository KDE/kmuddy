//
// C++ Interface: cgauge
//
// Description: One gauge (no GUI). It maintains the GUI item though.
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

#ifndef CGAUGE_H
#define CGAUGE_H

#include "clistobject.h"

#include <qcolor.h>

class cGaugeBarItem;

/**
This class represents one gauge. It stores necessary information, but doesn't paint anything.

@author Tomas Mecir
*/

class cGauge : public cListObject
{
 public:
  /** destructor */
  ~cGauge();
  
#define GAUGE_MATCH 1
#define GAUGE_UPDATE 2
  
  virtual cList::TraverseAction traverse (int traversalType);

  virtual void attribChanged (const QString &name);

  virtual void updateVisibleName();

  /** variable value has been changed - called by cGaugeList */
  void varValueChanged (const QString &var, int newval);

  cGaugeBarItem *gaugeItem ();
 protected:
  friend class cGaugeList;
  cGauge (cList *list);

  struct Private;
  Private *d;

  /** Update this gauge. */
  void updateGauge ();

  /** React on the fact that the object has moved. */
  virtual void objectMoved ();
  virtual void objectEnabled ();
  virtual void objectDisabled ();
};

#endif
