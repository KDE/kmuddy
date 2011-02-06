//
// C++ Interface: cstatusvar
//
// Description: One variable entry in the status bar
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CSTATUSVAR_H
#define CSTATUSVAR_H

#include "csaveablefield.h"

/**
This class represents one variable entry in the status bar.

@author Tomas Mecir
*/
class cStatusVar : public cSaveableField
{
 public:
  /** constructor */
  cStatusVar (int _sess);
  /** destructor */
  ~cStatusVar();

  /** creates a new instance of the class */
  virtual cSaveableField *newInstance ();

    /** load data from a config file*/
  virtual void load (KConfig *config, const QString &group);

  /** returns type of item (light-weight RTTI) */
  virtual int itemType () { return TYPE_STATUSVAR; };
 
  const QString &variable () { return _variable; };
  const QString &maxVariable () { return _maxvariable; };
  const QString &caption () { return _caption; };
  bool percentage () { return _percentage; };
  bool hidden () { return _hidden; };

  void setVariable (const QString &val);
  void setMaxVariable (const QString &val);
  void setCaption (const QString &val);
  void setPercentage (bool percent);
  void setHidden (bool h);

  int varValue () { return varvalue; };
  int maxVarValue () { return maxvarvalue; };
  
 protected:
  QString _variable, _maxvariable, _caption;
  int varvalue, maxvarvalue;
  bool _percentage, _hidden;

  int sess;
};

#endif
