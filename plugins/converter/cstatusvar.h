//
// C++ Interface: cstatusvar
//
// Description: One variable entry in the status bar
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
  ~cStatusVar() override;

  /** creates a new instance of the class */
  cSaveableField *newInstance () override;

    /** load data from a config file*/
  void load (KConfig *config, const QString &group) override;

  /** returns type of item (light-weight RTTI) */
  int itemType () override { return TYPE_STATUSVAR; };
 
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
