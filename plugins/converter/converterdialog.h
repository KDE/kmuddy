//
// C++ Interface: converterdialog
//
// Description: Converter Dialog.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CONVERTERDIALOG_H
#define CONVERTERDIALOG_H

#include <kdialog.h>
#include <list>

/** Defined one item of the conversion dialog. */
struct ConversionDialogItem {
  QString name, path;
  bool convert;
};

/** Converter dialog - displays the dialog and lets the user pick profiles to convert */

class cConverterDialog: public KDialog {
Q_OBJECT
public:
  cConverterDialog (QWidget *parent);
  ~cConverterDialog ();
  bool display (std::list<ConversionDialogItem *> items);
private:
  struct Private;
  Private *d;
};

#endif  // CONVERTERDIALOG_H
