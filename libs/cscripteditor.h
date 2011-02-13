//
// C++ Interface: cscripteditor
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

#ifndef CSCRIPTEDITOR_H
#define CSCRIPTEDITOR_H

#include <QWidget>
#include <kmuddy_export.h>

class KMUDDY_EXPORT cScriptEditor : public QWidget {
Q_OBJECT
 public:
  cScriptEditor (QWidget *parent);
  ~cScriptEditor ();
  void setText (const QString &text);
  QString text() const;
 protected slots:
  void timedCheckScript ();
  void checkScript ();
 private:
  struct Private;
  Private *d;
};

#endif   // CSCRIPTEDITOR_H
