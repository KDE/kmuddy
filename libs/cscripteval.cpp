//
// C++ Implementation: cscripteval
//
// Description: 
//
/*
Copyright 2010-2024 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cscripteval.h"
#include <QCoreApplication>
#include <QJSEngine>

class cScriptEval::Private {
public:

  QJSEngine engine;

  void doEval (QString script, QMap<QString, QVariant> variables, cScriptEval *obj)
  {
    QMapIterator<QString, QVariant> it(variables);
    while (it.hasNext()) {
      it.next();
      QJSValue val = engine.toScriptValue(it.value());
      engine.globalObject().setProperty(it.key(), val);
    }

    engine.evaluate (script);

    if (engine.hasError()) {
      QJSValue error = engine.catchError();
      obj->invokeEvent ("message", obj->sess(), "Error in script: " + error.toString());
    }
  }
};

cScriptEval::cScriptEval (int sess) : cActionBase ("scripteval", sess)
{
  d = new Private;
}

cScriptEval::~cScriptEval ()
{
  abort ();

  delete d;
}

void cScriptEval::addObject (QString name, QObject *object)
{
  QJSValue objectValue = d->engine.newQObject(object);
  d->engine.globalObject().setProperty (name, objectValue);
}

void cScriptEval::eval (QString script, QMap<QString, QVariant> variables)
{
  d->doEval (script, variables, this);
}

QString cScriptEval::validate (QString script)
{
  // TODO - QJSEngine does not support syntax checking, need to find a way to do it
  return QString();
/*
  QScriptSyntaxCheckResult res = QScriptEngine::checkSyntax (script);
  if (res.state() == QScriptSyntaxCheckResult::Valid) return QString();
  if (res.state() != QScriptSyntaxCheckResult::Error) return QString("(unfinished)");
  // errorMessage() only returns an empty string, so I can't use it. No idea why.
  // return "At line "+QString::number(res.errorLineNumber())+": " + res.errorMessage();
  return "Error at line "+QString::number(res.errorLineNumber());
*/
}

void cScriptEval::abort ()
{
  if (!d->engine.isInterrupted())
    d->engine.setInterrupted(true);
}

#include "moc_cscripteval.cpp"
