//
// C++ Interface: cvartrigger
//
// Description: One variable trigger.
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

#ifndef CVARTRIGGER_H
#define CVARTRIGGER_H

#include "csaveablefield.h"

/**
This class represents one variable trigger.


@author Tomas Mecir
*/
class cVarTrigger : public cSaveableField
{
public:
  /** constructor */
  cVarTrigger (int _sess);
  /** destructor */
  ~cVarTrigger ();

  /** creates a new instance of the class */
  virtual cSaveableField *newInstance ();

  /** load data from a config file */
  virtual void load (KConfig *config, const QString &group);

  /** abstract; returns type of item (light-weight RTTI) */
  virtual int itemType () { return TYPE_VARTRIG; };

  void setVarName (const QString &varname);
  QString varName () { return var; };

  //commands are implemented via newText/setNewText

protected:
  QString var;

  int sess;
};

#endif
