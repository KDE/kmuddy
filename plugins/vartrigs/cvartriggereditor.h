//
// C++ Interface: cvartriggereditor
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

#ifndef CVARTRIGGEREDITOR_H
#define CVARTRIGGEREDITOR_H

#include <clisteditor.h>


class cVarTriggerEditor : public cListEditor {
 Q_OBJECT
 public:
  cVarTriggerEditor (QWidget *parent);
  ~cVarTriggerEditor () override;

  void createGUI(QWidget *parent) override;
  void fillGUI (const cListObjectData &data) override;
  void getDataFromGUI (cListObjectData *data) override;
 protected:
  struct Private;
  Private *d;
};

#endif  // CVARTRIGGEREDITOR_H
