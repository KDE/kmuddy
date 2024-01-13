/***************************************************************************
                          dlgtranscript.h  -  Session Transcript dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pa nov 1 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#ifndef DLGTRANSCRIPT_H
#define DLGTRANSCRIPT_H

#include <kpagedialog.h>

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QLineEdit;

/**
Configures session transcript.
  *@author Tomas Mecir
  */

class dlgTranscript : public KPageDialog {
   Q_OBJECT
public: 
  dlgTranscript (QWidget *parent=nullptr);
  ~dlgTranscript () override;

  QSize sizeHint() const override;
  QString getFName ();
  QString getAFName ();
  void setFName (const QString &filename);
  void setAFName (const QString &filename);
  bool isEnabled ();
  void setEnabled (bool val);
  bool isOverwrite ();
  void setOverwrite (bool val);
  bool includeDump ();
  void setIncludeDump (bool val);
  void setAdvTranscript (bool val);
  bool advTranscript ();
  void setRotateDaily (bool val);
  bool rotateDaily ();
  void setIncludeTimestamp (bool val);
  bool includeTimestamp ();
  void setTranscriptType (char type);
  char transcriptType ();
  void setAdvTranscriptType (char type);
  char advTranscriptType ();
protected slots:
  void updateDialog (bool how);
  void updateAdvDialog (bool how);
  void browseFiles ();
protected:
  QCheckBox *enabletranscript;
  QCheckBox *enableadvtranscript;
  QCheckBox *rotatedaily;
  QCheckBox *includetimestamp;
  QCheckBox *overwrite, *allowansi;
  QCheckBox *includedump;
  QLineEdit *fname, *afname;

  QGroupBox *transbox;
  QGroupBox *advtrans;
  QLabel *lblname;
  QPushButton *filebutton;
  
  QComboBox *ttype, *advttype;
};

#endif
