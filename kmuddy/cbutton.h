//
// C++ Interface: cbutton
//
// Description: One button.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CBUTTON_H
#define CBUTTON_H

#include "clistobject.h"

/**
This class represents one button.

@author Tomas Mecir
*/

class cButton : public cListObject
{
Q_OBJECT
 public:
  /** destructor */
  ~cButton();
  
#define BUTTON_UPDATE 1
  
  virtual cList::TraverseAction traverse (int traversalType);

  virtual void attribChanged (const QString &name);

  virtual void updateVisibleName();

 protected slots:
  void execute (bool checked);
 protected:
  friend class cButtonList;
  cButton (cList *list);

  struct Private;
  Private *d;

  /** Insert this button to the button bar. */
  void addButton ();

  /** React on the fact that the object has moved. */
  virtual void objectMoved ();
  virtual void objectEnabled ();
  virtual void objectDisabled ();
};

#endif
