//
// C++ Interface: cshortcuteditor
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

#ifndef CSHORTCUTEDITOR_H
#define CSHORTCUTEDITOR_H

#include <clisteditor.h>


class cShortcutEditor : public cListEditor {
 Q_OBJECT
 public:
  cShortcutEditor (QWidget *parent);
  ~cShortcutEditor ();

  virtual void createGUI(QWidget *parent) override;
  virtual void fillGUI (const cListObjectData &data) override;
  virtual void getDataFromGUI (cListObjectData *data) override;
 protected slots:
  void grabKey ();
 protected:
  struct Private;
  Private *d;
};

#endif  // CSHORTCUTEDITOR_H
