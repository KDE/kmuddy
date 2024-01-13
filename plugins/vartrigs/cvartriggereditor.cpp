//
// C++ Implementation: cvartriggereditor
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

#include "cvartriggereditor.h"

#include "cactionmanager.h"

#include <QGridLayout>
#include <QLabel>
#include <QRegExpValidator>

#include <klineedit.h>
#include <KLocalizedString>
#include <ktextedit.h>

struct cVarTriggerEditor::Private {
  KLineEdit *var;
  KTextEdit *cmds;
};

cVarTriggerEditor::cVarTriggerEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cVarTriggerEditor::~cVarTriggerEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cVarTriggerEditor::createGUI(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout (parent);
  
  //variable
  QLabel *lbl1 = new QLabel (i18n ("&Variable name"), parent);
  d->var = new KLineEdit (parent);
  d->var->setValidator (new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this));
  lbl1->setBuddy (d->var);
  d->var->setWhatsThis( i18n ("Variable name that this trigger will be reacting on."));
  
  QLabel *lbl2 = new QLabel (i18n ("&Commands"), parent);
  d->cmds = new KTextEdit (parent);
  lbl2->setBuddy (d->cmds);
  d->cmds->setWhatsThis( i18n ("Commands that will be executed when this trigger matches."));

  QWidget *commonEditor = createCommonAttribEditor (parent);

  //place'em there!
  layout->setSpacing (5);
  layout->addWidget (lbl1, 0, 0);
  layout->addWidget (d->var, 0, 1);
  layout->addWidget (lbl2, 1, 0);
  layout->addWidget (d->cmds, 1, 1);
  layout->addWidget (commonEditor, 2, 0, 1, 2);
}

void cVarTriggerEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->var->setText (data.strValue ("variable"));
  QStringList command;
  for (int i = 1; i <= data.intValue ("command-count"); ++i)
    command << data.strValue ("command-" + QString::number (i));
  d->cmds->setPlainText (command.join("\n"));
}

void cVarTriggerEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["variable"] = d->var->text();
  QStringList cmds = d->cmds->toPlainText().split ("\n");
  data->intValues["command-count"] = cmds.size();
  QStringList::iterator it;
  int i;
  for (it = cmds.begin(), i = 1; it != cmds.end(); ++i, ++it)
    data->strValues["command-" + QString::number (i)] = *it;
}

#include "moc_cvartriggereditor.cpp"
