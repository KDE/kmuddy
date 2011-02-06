//
// C++ Interface: clisteditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  virtual ~cListEditor ();

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
