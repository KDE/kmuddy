//
// C++ Interface: cgauge
//
// Description: One gauge (no GUI). It maintains the GUI item though.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
