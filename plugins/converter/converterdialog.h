//
// C++ Interface: converterdialog
//
// Description: Converter Dialog.
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

#ifndef CONVERTERDIALOG_H
#define CONVERTERDIALOG_H

#include <QDialog>
#include <list>

/** Defined one item of the conversion dialog. */
struct ConversionDialogItem {
  QString name, path;
  bool convert;
};

/** Converter dialog - displays the dialog and lets the user pick profiles to convert */

class cConverterDialog: public QDialog {
Q_OBJECT
public:
  cConverterDialog (QWidget *parent);
  ~cConverterDialog () override;
  bool display (std::list<ConversionDialogItem *> items);
private:
  struct Private;
  Private *d;
};

#endif  // CONVERTERDIALOG_H
