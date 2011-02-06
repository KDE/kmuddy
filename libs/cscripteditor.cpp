//
// C++ Implementation: cscripteditor
//
// Description: Script editor - used in dialogs
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//

#include "cscripteditor.h"

#include "cscripteval.h"

#include <KTextEdit>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

struct cScriptEditor::Private {
  KTextEdit *script;
  QLabel *scriptError;
  QTimer *scriptChecker;
};


cScriptEditor::cScriptEditor (QWidget *parent) : QWidget (parent)
{
  d = new Private;

  QVBoxLayout *scriptlayout = new QVBoxLayout (this);

  d->script = new KTextEdit (this);
  d->scriptError = new QLabel (this);
  d->scriptChecker = new QTimer (this);
  d->scriptChecker->setSingleShot (true);
  connect (d->script, SIGNAL (textChanged()), this, SLOT (timedCheckScript()));
  connect (d->scriptChecker, SIGNAL (timeout()), this, SLOT (checkScript()));

  scriptlayout->setSpacing (10);
  scriptlayout->addWidget (d->script);
  scriptlayout->addWidget (d->scriptError);
  scriptlayout->setStretchFactor (d->script, 5);
}

cScriptEditor::~cScriptEditor ()
{
  delete d;
}

void cScriptEditor::setText (const QString &text)
{
  d->script->setText (text);
}

QString cScriptEditor::text() const
{
  return d->script->toPlainText();
}

void cScriptEditor::timedCheckScript ()
{
  d->scriptChecker->stop();
  d->scriptChecker->start (500);  // check the script if they don't type anything for 500 ms
}

void cScriptEditor::checkScript ()
{
  QString script = d->script->toPlainText();
  QString err = cScriptEval::validate (script);
  if (err.isEmpty()) err = "Syntax OK";
  d->scriptError->setText (err);
}


#include "cscripteditor.moc"
