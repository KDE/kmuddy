//
// C++ Interface: cgaugelist
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
  virtual cListObject *newObject () override;
  virtual QString objName () override { return "Gauge"; }
  virtual cListEditor *editor (QWidget *parent) override;
  
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

  virtual void listLoaded () override;
  
  void addGauge (cGauge *g);
  void removeGauge (cGauge *g);

  void updateGauges ();
  
  QString variableName () const;
};

#endif  //CGAUGELIST_H
