//
// C++ Interface: clistgroupeditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
