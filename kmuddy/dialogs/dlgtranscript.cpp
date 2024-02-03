/***************************************************************************
                          dlgtranscript.cpp  -  Session Transcript dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pá nov 1 2002
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

#include "dlgtranscript.h"

#include <qcheckbox.h>
#include <qcombobox.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLineEdit>

#include <KLocalizedString>

dlgTranscript::dlgTranscript (QWidget *parent) :
    KPageDialog (parent)
{
  //initial dialog size
  setWindowTitle (i18n ("Session Transcript"));
  setFaceType( KPageDialog::Tabbed );
  setStandardButtons (QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

  //create main dialog's widget
  QFrame *page = new QFrame (this);
  QFrame *advpage = new QFrame (this);
  addPage (page, i18n ("Basic"));
  addPage (advpage, i18n ("Advanced"));
  
  QVBoxLayout *layout = new QVBoxLayout (page);
  QVBoxLayout *advlayout = new QVBoxLayout (advpage);

  //setMainWidget (page);

  //enable transcript checkbox
  enabletranscript = new QCheckBox (i18n ("&Enable basic transcript"), page);
  connect (enabletranscript, &QCheckBox::toggled, this, &dlgTranscript::updateDialog);
  enabletranscript->setWhatsThis( i18n ("Enables or disabled the session transcript. "
      "Note that session transcript is automatically disabled when you disconnect."));
  
  //transcript settings
  transbox = new QGroupBox (i18n ("Transcript settings"), page);
  QVBoxLayout *transboxlayout = new QVBoxLayout (transbox);
  
  QFrame *typebox = new QFrame (transbox);
  QHBoxLayout *typeboxLayout = new QHBoxLayout (typebox);
  typeboxLayout->setSpacing (5);
  QLabel *lbltype = new QLabel (i18n ("Transcript &format"), typebox);
  ttype = new QComboBox (typebox);
  ttype->addItem ("HTML");
  ttype->addItem ("Plain text");
  ttype->addItem ("Text with ANSI colors");
  lbltype->setBuddy (ttype);
  typeboxLayout->addWidget (lbltype);
  typeboxLayout->addWidget (ttype);
  
  overwrite = new QCheckBox (i18n ("&Overwrite existing file"), transbox);
  overwrite->setWhatsThis( i18n ("If you enable this option, existing "
      "transcript file will be overwritten. Otherwise, new transcript will "
      "be appended to the end of an existing file (if it exists :))."));
  includedump = new QCheckBox (i18n ("Include buffer &dump"), transbox);
  includedump->setWhatsThis( i18n ("This will cause KMuddy to add "
      "complete buffer dump to the beginning of the transcript file."));
  
  //the file name
  QFrame *fileframe = new QFrame (transbox);
  QVBoxLayout *filelayout = new QVBoxLayout (fileframe);
  lblname = new QLabel (i18n ("&File:"), fileframe);
  fname = new QLineEdit (fileframe);
  lblname->setBuddy (fname);
  filebutton = new QPushButton (i18n ("&Browse..."), fileframe);

  filelayout->setSpacing (5);
  filelayout->addWidget (lblname);
  filelayout->addWidget (fname);
  filelayout->addWidget (filebutton);

  //connect the Browse button!
  connect (filebutton, &QPushButton::clicked, this, &dlgTranscript::browseFiles);
  
  transboxlayout->setSpacing (5);
  transboxlayout->addWidget (typebox);
  transboxlayout->addWidget (overwrite);
  transboxlayout->addWidget (includedump);
  transboxlayout->addWidget (fileframe);

  layout->setSpacing (5);
  layout->addWidget (enabletranscript);
  layout->addWidget (transbox);

  enableadvtranscript = new QCheckBox (i18n ("Enable &advanced transcript"), advpage);
  enableadvtranscript->setWhatsThis( i18n ("This will enable advanced transcript, "
        "which allows you to have transcript running all the time, rotates it at "
        "midnight and so. Note that normal and advanced transcripts can both be "
        "running at the same time, if you want."));
  connect (enableadvtranscript, &QCheckBox::toggled, this, &dlgTranscript::updateAdvDialog);
  advtrans = new QGroupBox(i18n ("Advanced transcript settings"), advpage);
  QVBoxLayout *advtranslayout = new QVBoxLayout (advtrans);
  
  QFrame *advtypebox = new QFrame (advtrans);
  QHBoxLayout *advtypeboxlayout = new QHBoxLayout (advtypebox);
  advtypeboxlayout->setSpacing (5);
  QLabel *lbladvtype = new QLabel (i18n ("Transcript &format"), advtypebox);
  advttype = new QComboBox (advtypebox);
  advttype->addItem ("HTML");
  advttype->addItem ("Plain text");
  advttype->addItem ("Text with ANSI colors");
  lbladvtype->setBuddy (advttype);
  advtypeboxlayout->addWidget (lbladvtype);
  advtypeboxlayout->addWidget (advttype);

  rotatedaily = new QCheckBox (i18n ("&Rotate log at midnight every day"), advtrans);
  rotatedaily->setWhatsThis( i18n ("This will cause KMuddy to rotate session transcript "
                                      "every day at midnight"));
  
  includetimestamp = new QCheckBox (i18n ("Prepend lines with &timestamp"), advtrans);
  includetimestamp->setWhatsThis( i18n ("This option will cause KMuddy to insert local "
                                           "timestamp in front of every line of logging"));

  QFrame *afileframe = new QFrame (advtrans);
  QHBoxLayout *afilelayout = new QHBoxLayout (afileframe);
  
  QLabel *logformat = new QLabel (i18n ("Log name format"), afileframe);
  logformat->setWhatsThis( i18n ("Here you can specify log format. You can use the following "
      "options: $W - world name, $h - hour, $m - minute, $D - day, $M - month, $Y - year, and "
      "any combination of alpha-numeric characters."));
  
  afname = new QLineEdit (afileframe);
  logformat->setBuddy(afname);
  
  afilelayout->setSpacing (5);
  afilelayout->addWidget (logformat);
  afilelayout->addWidget (afname);
  
  advtranslayout->addWidget (advtypebox);
  advtranslayout->addWidget (rotatedaily);
  advtranslayout->addWidget (includetimestamp);
  advtranslayout->addWidget (afileframe);

  advlayout->addWidget (enableadvtranscript);
  advlayout->addWidget (advtrans);
  
  //initial dialog settings...
  enabletranscript->setChecked (false);
  enableadvtranscript->setChecked (false);
  ttype->setCurrentIndex (0);
  advttype->setCurrentIndex (0);
  updateDialog (false);
  updateAdvDialog (false);
}

dlgTranscript::~dlgTranscript ()
{

}

QSize dlgTranscript::sizeHint() const
{
  return QSize (400, 200);
}

void dlgTranscript::updateDialog (bool how)
{
  overwrite->setEnabled (how);
  fname->setEnabled (how);
  transbox->setEnabled (how);
  lblname->setEnabled (how);
  filebutton->setEnabled (how);
  ttype->setEnabled (how);
}

void dlgTranscript::updateAdvDialog (bool how)
{
  rotatedaily->setEnabled(how);
  includetimestamp->setEnabled(how);
  advtrans->setEnabled(how);
  afname->setEnabled(how);
  advttype->setEnabled (how);
}

void dlgTranscript::browseFiles ()
{
  fname->setText (QFileDialog::getSaveFileName ());
}

QString dlgTranscript::getFName ()
{
  return fname->text ();
}

QString dlgTranscript::getAFName ()
{
  return afname->text ();
}

void dlgTranscript::setFName (const QString &filename)
{
  fname->setText (filename);
}

void dlgTranscript::setAFName (const QString &filename)
{
  afname->setText (filename);
}

bool dlgTranscript::isEnabled ()
{
  return enabletranscript->isChecked ();
}

void dlgTranscript::setEnabled (bool val)
{
  enabletranscript->setChecked (val);
}

bool dlgTranscript::isOverwrite ()
{
  return overwrite->isChecked ();
}

void dlgTranscript::setOverwrite (bool val)
{
  overwrite->setChecked (val);
}

bool dlgTranscript::includeDump ()
{
  return includedump->isChecked ();
}

void dlgTranscript::setIncludeDump (bool val)
{
  includedump->setChecked (val);
}

void dlgTranscript::setAdvTranscript(bool val)
{
  enableadvtranscript->setChecked(val);
}

bool dlgTranscript::advTranscript()
{
  return enableadvtranscript->isChecked();
}

void dlgTranscript::setRotateDaily(bool val)
{
  rotatedaily->setChecked(val);
}

bool dlgTranscript::rotateDaily()
{
  return rotatedaily->isChecked();
}

void dlgTranscript::setIncludeTimestamp(bool val)
{
  includetimestamp->setChecked(val);
}

bool dlgTranscript::includeTimestamp()
{
  return includetimestamp->isChecked();
}

void dlgTranscript::setTranscriptType (char type)
{
  ttype->setCurrentIndex (type - 1);
}

char dlgTranscript::transcriptType ()
{
  return ttype->currentIndex() + 1;
}

void dlgTranscript::setAdvTranscriptType (char type)
{
  advttype->setCurrentIndex (type - 1);
}

char dlgTranscript::advTranscriptType ()
{
  return advttype->currentIndex() + 1;
}

#include "moc_dlgtranscript.cpp"
