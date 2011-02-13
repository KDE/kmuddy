//
// C++ Interface: cpattern
//
// Description: 
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CPATTERN_H
#define CPATTERN_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <kmuddy_export.h>

/**
This class performs all types of pattern matching.

	@author Tomas Mecir <kmuddy@kmuddy.com>
*/
class KMUDDY_EXPORT cPattern {
 public:
  enum PatternType {
    exact = 0,
    substring = 1,
    begin = 2,
    end = 3,
    regexp = 4
  };

  cPattern (const QString &pattern = QString(), PatternType pt = exact);
  ~cPattern ();

  void setPattern (const QString &pattern);
  QString pattern () const;

  void setMatching (PatternType pt);
  PatternType matching () const;

  void setCaseSensitive (bool cs);
  bool caseSensitive () const;

  void setWholeWords (bool ww);
  bool wholeWords () const;

  /** perform matching */
  bool match (const QString &text, int startpos = 0);

  /** returns last text being successfully compared */
  QString getLastText () const;
  /** returns last prefix */
  QString getPrefix () const;
  /** returns last suffix */
  QString getSuffix () const;
  int getLastPos () const;
  int getLastLength () const;
  /** returns a list of back-references (if doing regexp compare) */
  QStringList getBackRefList () const;
  int getBackRefPos (int which) const;

  /** Returns pseudo-variable position. */
  void variablePosition (const QString &varname, int *start, int *len);
  /** Expands a pseudo-variable; used by expandPseudoVariables() and in test areas */
  QString getVariable (const QString &varname, const QString &def = QString()) const;

  /** Expands pseudo-variables in the string. Pseudo-variables are the following:
  <br><b>$prefix</b> - trimmed part of string BEFORE the matched part
  <br><b>$suffix</b> - trimmed part of string AFTER the matched part
  <br><b>$prefixfull</b> - prefix including leading/trailing spaces
  <br><b>$suffixfull</b> - suffix including leading/trailing spaces
  <br><b>$matched</b> - matched part of the string
  <br><b>$1</b>, <b>$2</b>, ... - back-references; regexp expansion only
   */
  void expandPseudoVariables (QString &string) const;

  /** Return the variables in a form suitable for scripting (cScriptEval). */
  QMap<QString, QVariant> scriptVariables ();
 protected:
  struct Private;
  Private *d;
};

#endif
