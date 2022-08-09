/***************************************************************************
                          dlgimportexportprofile.h  -  description
                             -------------------
    begin                : Ne júl 20 2003
    copyright            : (C) 2003 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DLGIMPORTEXPORTPROFILE_H
#define DLGIMPORTEXPORTPROFILE_H

#include <QDialog>

class QLineEdit;
class QComboBox;

/**
Dialog where you choose what to import/export.
  *@author Tomas Mecir
  */

class dlgImportExportProfile : public QDialog {
   Q_OBJECT
public: 
  dlgImportExportProfile (bool isImport, QWidget *parent=nullptr);
  ~dlgImportExportProfile () override;
  void doThings ();
  QSize sizeHint() const override;
protected slots:
  void browse ();
protected:
  bool import;
  QLineEdit *edfname;
  QLineEdit *edprofile;
  QComboBox *cbprofile;
};

#endif
