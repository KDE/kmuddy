//
// C++ Interface: cscripteval
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSCRIPTEVAL_H
#define CSCRIPTEVAL_H

#include <cactionbase.h>
#include <kmuddy_export.h>
#include <QString>
#include <QMap>
#include <QVariant>

class QEvent;

/** cScriptEval - evaluates QtScript scripts. */
class KMUDDY_EXPORT cScriptEval : public QObject, public cActionBase
{
Q_OBJECT
public:
  cScriptEval (int sess);
  ~cScriptEval();

  /** Add object to the global script namespace. */
  void addObject (QString name, QObject *object);

  /* Evaluate */
  void eval (QString script, QMap<QString, QVariant> variables = QMap<QString, QVariant>());
  /** Abort evaluation. */
  void abort ();

  /* Validate. Returns an empty string if it's ok, error message if not. */
  static QString validate (QString script);

private:
  virtual bool event(QEvent *e);
  class Private;
  Private *d;
};

#endif  // CSCRIPTEVAL_H
