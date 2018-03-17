/***************************************************************************
                          dlgdumpbuffer.cpp  -  Dump Buffer dialog
                             -------------------
    begin                : Pi feb 28 2003
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

#include "dlgdumpbuffer.h"

#include "cconsole.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <kfiledialog.h>
#include <klineedit.h>
#include <klocale.h>

dlgDumpBuffer::dlgDumpBuffer (QWidget *parent) : QDialog (parent)
{
  //initial dialog size
  setWindowTitle (i18n ("Dump output buffer"));
  setButtons (KDialog::Ok | KDialog::Cancel);

  //create main dialog's widget
  QVBoxLayout *layout = new QVBoxLayout (this);

  //where to start?
  chkcurpos = new QCheckBox (i18n ("&Start at current position"), this);
  chkcurpos->setWhatsThis( i18n ("When this is enabled, only the "
      "part of history buffer from currently displayed part up to the end of "
      "buffer will be dumped. Otherwise entire buffer will be dumped (default)."));

  //the file name
  QFrame *fileframe = new QFrame (this);
  QHBoxLayout *filelayout = new QHBoxLayout (fileframe);
  QLabel *lblname = new QLabel (i18n ("&File:"), fileframe);
  fname = new KLineEdit (fileframe);
  lblname->setBuddy (fname);
  QPushButton *filebutton = new QPushButton (i18n ("&Browse..."), fileframe);

  filelayout->setSpacing (5);
  filelayout->addWidget (lblname);
  filelayout->addWidget (fname);
  filelayout->addWidget (filebutton);

  //type
  QFrame *typeframe = new QFrame (this);
  QHBoxLayout *typelayout = new QHBoxLayout (typeframe);
  QLabel *lbltype = new QLabel (i18n ("&Dump format:"), typeframe);
  ttype = new QComboBox (typeframe);
  ttype->addItem (i18n ("HTML"));
  ttype->addItem (i18n ("Plain text"));
  ttype->addItem (i18n ("Text with ANSI colors"));
  lbltype->setBuddy (ttype);

  typelayout->setSpacing (5);
  typelayout->addWidget (lbltype);
  typelayout->addWidget (ttype);

  //connect the Browse button!
  connect (filebutton, SIGNAL (clicked ()), this, SLOT (browseFiles ()));

  //connect the qcombobox to our function
  connect (ttype, SIGNAL (activated (int)), this, SLOT (updateFname(int)));
  
  //    connect( pathCombo, SIGNAL( activated( const QString & ) ), this, SLOT ( changePath( const QString & ) ) );
  
  layout->setSpacing (5);
  layout->addWidget (chkcurpos);
  layout->addWidget (fileframe);
  layout->addWidget (typeframe);

  //initial dialog settings...
  chkcurpos->setChecked (false);
  fname->setText (QDir::homePath() + "/buffer_dump.html");
}

dlgDumpBuffer::~dlgDumpBuffer ()
{

}

QSize dlgDumpBuffer::sizeHint() const
{
  return QSize (300, 100);
}

void dlgDumpBuffer::browseFiles ()
{
  fname->setText (KFileDialog::getSaveFileName ());
}

bool dlgDumpBuffer::curPos ()
{
  return chkcurpos->isChecked ();
}

QString dlgDumpBuffer::fileName ()
{
  return fname->text ();
}

void dlgDumpBuffer::setFileName (const QString &fName)
{
  fname->setText (fName);
}

void dlgDumpBuffer::updateFname (int option)
{
  //If we set it to html...
  //and we don't end with a .txt and a .html?
  //add a .html
  //or we dont end with a .html, but with a .txt?
  //replace .txt with .html
  //or we want it to end with a .txt? Same thing again..
  
  QString tempString = fname->text();
  bool hasHtml = tempString.endsWith (".html", Qt::CaseInsensitive);
  bool hasTxt = tempString.endsWith (".txt", Qt::CaseInsensitive);
  if (option + 1 == TRANSCRIPT_HTML)
  {
    if (hasHtml) return; // we already have the right suffix
    if (hasTxt) tempString.remove (tempString.length() - 4, 4);
    tempString.append(".html");
  } else {
    if (hasTxt) return; // we already have the right suffix
    if (hasHtml) tempString.remove (tempString.length() - 5, 5);
    tempString.append(".txt");
  }
  
  fname->setText(tempString);
}


int dlgDumpBuffer::type ()
{
  return ttype->currentIndex() + 1;
}

void dlgDumpBuffer::setType (int t)
{
  ttype->setCurrentIndex (t - 1);
}

