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

#include "csaveablefield.h"

#include <qcolor.h>

class cGaugeBarItem;

/**
This class represents one gauge. It stores necessary information, but doesn't paint anything.

@author Tomas Mecir
*/

class cGauge : public cSaveableField
{
 public:
  /** constructor */
  cGauge (int _sess);
  /** destructor */
  ~cGauge();
  
  /** creates a new instance of the class */
  virtual cSaveableField *newInstance ();

    /** load data from a config file*/
  virtual void load (KConfig *config, const QString &group);

  /** returns type of item (light-weight RTTI) */
  virtual int itemType () { return TYPE_GAUGE; };
  
  const QString &variable () { return _variable; };
  const QString &maxVariable () { return _maxvariable; };
  const QString &caption () { return _caption; };
  const QColor &color () { return _color; };
  bool hidden () { return _hidden; };
  
  void setVariable (const QString &val);
  void setMaxVariable (const QString &val);
  void setCaption (const QString &val);
  void setColor (const QColor &c);
  void setHidden (bool h);

  int varValue () { return varvalue; };
  int maxVarValue () { return maxvarvalue; };
  
 protected:
  QString _variable, _maxvariable, _caption;
  int varvalue, maxvarvalue;
  QColor _color;
  bool _hidden;
  int sess;
};

#endif
