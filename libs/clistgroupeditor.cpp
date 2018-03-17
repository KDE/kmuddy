//
// C++ Implementation: clistgroupeditor
//
// Description: list group editor widget
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "clistgroupeditor.h"

#include <QHBoxLayout>

cListGroupEditor::cListGroupEditor (QWidget *parent)
  : cListEditor (parent)
{
}

cListGroupEditor::~cListGroupEditor ()
{
}

void cListGroupEditor::createGUI (QWidget *parent)
{
  QHBoxLayout *layout = new QHBoxLayout (parent);
  QWidget *commonEditor = createCommonAttribEditor (parent);
  layout->addWidget (commonEditor);
}

void cListGroupEditor::fillGUI (const cListObjectData &data)
{
  fillCommonAttribEditor (data);
}

void cListGroupEditor::getDataFromGUI (cListObjectData *data)
{
  getDataFromCommonAttribEditor (data);
}

