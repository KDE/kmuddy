//
// C++ Interface: cvartrigger
//
// Description: One variable trigger.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
