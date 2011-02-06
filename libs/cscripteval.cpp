//
// C++ Implementation: cscripteval
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cscripteval.h"
#include <QCoreApplication>
#include <QScriptContext>
#include <QScriptEngine>
#include <QScriptSyntaxCheckResult>
#include <QStringList>

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

#include "cscripteval.moc"
