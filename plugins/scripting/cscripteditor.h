//
// C++ Interface: cscripteditor
//
// Description: 
//
/*
Copyright 2002-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include <clisteditor.h>


class cScriptEditor : public cListEditor {
 Q_OBJECT
 public:
  cScriptEditor (QWidget *parent);
  ~cScriptEditor ();

  virtual void createGUI(QWidget *parent);
  virtual void fillGUI (const cListObjectData &data);
  virtual void getDataFromGUI (cListObjectData *data);
 protected slots:
  void browse1 ();
  void browse2 ();
 protected:
  struct Private;
  Private *d;
};

#endif  // CSCRIPTEDITOR_H
