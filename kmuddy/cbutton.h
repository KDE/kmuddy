//
// C++ Interface: cbutton
//
// Description: One button.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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
