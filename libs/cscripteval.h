//
// C++ Interface: cscripteval
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
  virtual bool event(QEvent *e) override;
  class Private;
  Private *d;
};

#endif  // CSCRIPTEVAL_H
