//
// C++ Interface: clisteditor
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

#ifndef CLISTEDITOR_H
#define CLISTEDITOR_H

#include <kmuddy_export.h>
#include <clistobject.h>
#include <map>

#include <QWidget>

/** cListEditor - base class for the single object editor */

class KMUDDY_EXPORT cListEditor : public QWidget {
  Q_OBJECT
 public:
  /** constructor */
  cListEditor (QWidget *parent);
  /** destructor */
  ~cListEditor () override;

  /** Change the edited object. */
  void setObject (cListObject *obj);
  /** Returns the currently edited object. */
  cListObject *object ();

  /** Do we hold a valid object ? */
  bool objectValid ();

  /** Are there unsaved changes ? */
  bool changed ();
  void saveChanges ();
 protected slots:
  void saveClicked ();
  void undoClicked ();
  void objectChanged (cListObject *obj);
 protected:

  /** Create the GUI, with the given object as parent.
    IMPORTANT: this must also create a layout for the parent widget, as it has none !!! */
  virtual void createGUI(QWidget *parent) = 0;
  QWidget *createCommonAttribEditor (QWidget *parent);
  void fillCommonAttribEditor (const cListObjectData &data);
  void getDataFromCommonAttribEditor (cListObjectData *data);
  
  /** Load data from the object. */
  void loadDataFromObject ();
  /** Save the changes back to the object. */
  void saveDataToObject ();

  /** prepares the structure and calls getDataFromGUI */
  void getGUIData ();

  /** Fill in the GUI. */
  virtual void fillGUI (const cListObjectData &data) = 0;
  /** Read the data from the GUI, fill in the data structure. */
  virtual void getDataFromGUI (cListObjectData *data) = 0;

private:
  struct Private;
  Private *d;

};


#endif  // CLISTEDITOR_H
