//
// C++ Implementation: cscripteval
//
// Description: 
//
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

#include "cscripteval.h"
#include <QCoreApplication>
#include <QScriptContext>
#include <QScriptEngine>
#include <QScriptSyntaxCheckResult>

class TooLongEvent : public QEvent { public: TooLongEvent() : QEvent(QEvent::User) {}; };

class cScriptEval::Private {
public:

  QScriptEngine *engine;

  void doEval (QString script, QMap<QString, QVariant> variables, cScriptEval *obj)
  {
    QScriptContext *context = engine->pushContext();

    QMapIterator<QString, QVariant> it(variables);
    while (it.hasNext()) {
      it.next();
      QScriptValue val;
      if (it.value().type() == QVariant::StringList)
        val = engine->toScriptValue (it.value().toStringList());
      else
        val = engine->newVariant (it.value());
      context->activationObject().setProperty (it.key(), val);
    }

    QCoreApplication::postEvent (obj, new TooLongEvent, 50);  // a super high-priority event
    engine->evaluate (script);
    engine->popContext();

    if (engine->hasUncaughtException()) {
      obj->invokeEvent ("message", obj->sess(), "Error in script: " + engine->uncaughtException().toString());
      engine->clearExceptions ();
    }
  }
};

cScriptEval::cScriptEval (int sess) : cActionBase ("scripteval", sess)
{
  d = new Private;
  d->engine = new QScriptEngine;
  d->engine->setProcessEventsInterval (4000);
}

cScriptEval::~cScriptEval ()
{
  abort ();

  delete d->engine;
  delete d;
}

void cScriptEval::addObject (QString name, QObject *object)
{
  d->engine->globalObject().setProperty (name, d->engine->newQObject (object));
}

void cScriptEval::eval (QString script, QMap<QString, QVariant> variables)
{
  d->doEval (script, variables, this);
}

QString cScriptEval::validate (QString script)
{
  QScriptSyntaxCheckResult res = QScriptEngine::checkSyntax (script);
  if (res.state() == QScriptSyntaxCheckResult::Valid) return QString();
  if (res.state() != QScriptSyntaxCheckResult::Error) return QString("(unfinished)");
  // errorMessage() only returns an empty string, so I can't use it. No idea why.
  // return "At line "+QString::number(res.errorLineNumber())+": " + res.errorMessage();
  return "Error at line "+QString::number(res.errorLineNumber());
}

void cScriptEval::abort ()
{
  if (d->engine->isEvaluating())
    d->engine->abortEvaluation();
}

bool cScriptEval::event (QEvent *e)
{
  if (!dynamic_cast<TooLongEvent *>(e)) return QObject::event (e);

  if (!d->engine->isEvaluating()) return true;

  invokeEvent ("message", sess(), "Script execution is taking too long, aborting.");
  abort();
  return true;
}

#include "moc_cscripteval.cpp"
