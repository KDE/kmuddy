//
// C++ Interface: clistgroupeditor
//
// Description: 
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

#ifndef CLISTGROUPEDITOR_H
#define CLISTGROUPEDITOR_H

#include <kmuddy_export.h>
#include <clisteditor.h>

/** cListEditor - base class for the single object editor */

class KMUDDY_EXPORT cListGroupEditor : public cListEditor {
  Q_OBJECT
 public:
  /** constructor */
  cListGroupEditor (QWidget *parent);
  /** destructor */
  virtual ~cListGroupEditor ();
protected:
  /** Create the GUI. */
  virtual void createGUI(QWidget *parent);
  /** Fill in the GUI. */
  virtual void fillGUI (const cListObjectData &data);
  /** Read the data from the GUI, fill in the data structure. */
  virtual void getDataFromGUI (cListObjectData *data);
};


#endif  // CLISTGROUPEDITOR_H
