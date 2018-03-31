/***************************************************************************
                          dlgmultiline.cpp  -  multi-line input dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Aug 26 2002
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

#include "dlgmultiline.h"

#include <qlabel.h>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <kpushbutton.h>
#include <klineedit.h>
#include <klocale.h>

dlgMultiLine::dlgMultiLine (QWidget *parent) : QDockWidget (parent)
{
  //initial size
  setWindowTitle (i18n ("Multi-line input"));

  //main widget
  QWidget *page = new QWidget (this);
  QVBoxLayout *layout = new QVBoxLayout (page);

  setWidget (page);
  setFocusPolicy (Qt::StrongFocus);
  
  //editor
  editor = new QTextEdit (page);
  editor->setAcceptRichText (false);
  editor->setWhatsThis( i18n ("Enter commands that are to be sent to the MUD. "
  "Each line represents one command."));
  editor->setWordWrapMode (QTextOption::NoWrap);
    
  layout->setSpacing (5);
  layout->addWidget (editor);

  //prefix/suffix
  QFrame *ps = new QFrame (page);
  layout->addWidget (ps);
  QHBoxLayout *pslayout = new QHBoxLayout (ps);
  QLabel *l1 = new QLabel (i18n ("&Prefix:"), ps);
  prefix = new KLineEdit (ps);
  l1->setBuddy (prefix);
  QLabel *l2 = new QLabel (i18n ("&Suffix:"), ps);
  suffix = new KLineEdit (ps);
  l2->setBuddy (suffix);
  pslayout->setSpacing (5);
  pslayout->addWidget (l1);
  pslayout->addWidget (prefix);
  pslayout->addSpacing (5);
  pslayout->addWidget (l2);
  pslayout->addWidget (suffix);

  QString hlptext = i18n ("Prefix and suffix may contain text, that will "
      "be added to the beginning (prefix) or ending (suffix) of every "
      "line written to the multi-line editor.");
  
  prefix->setWhatsThis (hlptext);
  suffix->setWhatsThis (hlptext);
  
  QFrame *bt = new QFrame (page);
  layout->addWidget (bt);
  QHBoxLayout *btlayout = new QHBoxLayout (bt);
  KPushButton *bt1 = new KPushButton (i18n ("&Send"), bt);
  bt1->setToolTip ( i18n ("Sends current text to the MUD."));
  bt1->setWhatsThis (i18n ("This will send all the commands you've "
    "entered to the MUD. If you've entered prefix and/or suffix, they will be added "
    "to the beginning and ending of each command."));
  KPushButton *bt2 = new KPushButton (i18n ("&Clear"), bt);
  bt2->setToolTip ( i18n ("Clears window contents."));
  btlayout->addWidget (bt1);
  btlayout->addWidget (bt2);
  connect (bt1, SIGNAL (clicked ()), this, SLOT (sendClicked ()));
  connect (bt2, SIGNAL (clicked ()), editor, SLOT (clear ()));
}

dlgMultiLine::~dlgMultiLine ()
{
}

QSize dlgMultiLine::sizeHint() const
{
  return QSize (400, 250);
}

void dlgMultiLine::setFont (const QFont &font)
{
  editor->setFont (font);
}

void dlgMultiLine::sendClicked ()
{
  QStringList text = editor->toPlainText().split ("\n");
  QString s1 = prefix->text();
  QString s2 = suffix->text();
  if ((!s1.isEmpty()) && (!s1.endsWith (' '))) s1 += " ";
  if ((!s2.isEmpty()) && (!s2.endsWith (' '))) s2 = " " + s2;
  QStringList cmds;
  // append prefix/suffix
  QStringList::iterator it;
  for (it = text.begin(); it != text.end(); ++it) {
    cmds << (s1 + *it + s2);
  }
  emit commands (cmds);
}

