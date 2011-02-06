//
// C++ Interface: cgaugelist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGAUGELIST_H
#define CGAUGELIST_H

#include "clist.h"

#include <qcolor.h>
#include <qstring.h>

class cGauge;

/**
List of gauges.

@author Tomas Mecir
*/
class cGaugeList : public cList
{
public:
  cGaugeList ();
  ~cGaugeList ();

  static cList *newList () { return new cGaugeList; };
  virtual cListObject *newObject ();
  virtual QString objName () { return "Gauge"; }
  virtual cListEditor *editor (QWidget *parent);
  
  /** a gauge request, adds a new gauge, if there is no gauge already using the
  same base variable */
  bool requestGauge (const QString &var, const QString &maxvar, const QString &caption,
      const QColor &color, bool hidden);
  /** as above; *DELETES* the gauge if it can't accept it! */
  bool requestGauge (cGauge *gauge);
  
  /** handle the fact that a variable value has been changed */
  void variableChanged (const QString &varname);
 private:
  struct Private;
  Private *d;
  friend class cGauge;

  virtual void listLoaded ();
  
  void addGauge (cGauge *g);
  void removeGauge (cGauge *g);

  void updateGauges ();
  
  QString variableName () const;
};

#endif  //CGAUGELIST_H
