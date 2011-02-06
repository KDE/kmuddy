//
// C++ Implementation: clistgroupeditor
//
// Description: list group editor widget
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

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


#include "clistgroupeditor.moc"
