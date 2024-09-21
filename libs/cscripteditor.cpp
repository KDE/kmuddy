//
// C++ Implementation: cscripteditor
//
// Description: Script editor - used in dialogs
/*
Copyright 2010-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cscripteditor.h"

#include "cscripteval.h"

#include <KTextEdit>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <KLocalizedString>

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
  d->script->setToolTip (i18n ("Enter your script here."));
  d->script->setWhatsThis (i18n ("Enter a Javascript script that will be executed when the object is triggered.<br /><br />The following routines are available in the script in addition to standard Javascript tools:<br /><br /><b>window.message(text)</b> - display a message<br /><b>window.send(command)</b> - send a command<br /><b>window.sendraw(command)</b> - send a command with no processing<br /><b>window.get(name)</b> - retrieve the value of a variable<br /><b>window.set(name, value)</b> - set the value of a variable<br /><br />You can also use the following variables: <b>prefix, prefixfull, suffix, suffixfull, matched, line, matches[]</b>"));
  d->scriptError = new QLabel (this);
  d->scriptChecker = new QTimer (this);
  d->scriptChecker->setSingleShot (true);
  connect (d->script, &KTextEdit::textChanged, this, &cScriptEditor::timedCheckScript);
  connect (d->scriptChecker, &QTimer::timeout, this, &cScriptEditor::checkScript);

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

#include "moc_cscripteditor.cpp"
