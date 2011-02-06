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
