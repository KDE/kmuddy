//
// C++ Implementation: cgaugeeditor
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

#include "cgaugeeditor.h"

#include "cactionmanager.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>

#include <kcolorbutton.h>
#include <KLocalizedString>

struct cGaugeEditor::Private {
  QLineEdit *var, *maxvar, *caption;
  KColorButton *btcolor;
};

cGaugeEditor::cGaugeEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cGaugeEditor::~cGaugeEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cGaugeEditor::createGUI(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout (parent);
  
  //variable
  QLabel *lbl1 = new QLabel (i18n ("&Variable name"), parent);
  d->var = new QLineEdit (parent);
  d->var->setValidator (new QRegularExpressionValidator (QRegularExpression("^[0-9A-Za-z_ ]+$"), this));
  lbl1->setBuddy (d->var);
  d->var->setWhatsThis( i18n ("Variable displayed by this gauge."));
  
  //max variable
  QLabel *lbl2 = new QLabel (i18n ("&Max. variable (optional)"), parent);
  d->maxvar = new QLineEdit (parent);
  d->maxvar->setValidator (new QRegularExpressionValidator (QRegularExpression("^[0-9A-Za-z_ ]+$"), this));
  lbl2->setBuddy (d->maxvar);
  d->maxvar->setWhatsThis( i18n ("Variable holding the maximum value (default=100)"));
  
  //caption
  QLabel *lbl3 = new QLabel (i18n ("&Caption"), parent);
  d->caption = new QLineEdit (parent);
  lbl3->setBuddy (d->caption);
  d->caption->setWhatsThis( i18n ("Caption shown next to variable value."));

  // color
  QLabel *lbl4 = new QLabel (i18n ("&Color"), parent);
  d->btcolor = new KColorButton (Qt::white, parent);
  lbl4->setBuddy (d->btcolor);
  d->btcolor->setWhatsThis( i18n ("Color used to paint this gauge."));

  QWidget *commonEditor = createCommonAttribEditor (parent);

  //place'em there!
  layout->setSpacing (5);
  layout->addWidget (lbl1, 0, 0);
  layout->addWidget (d->var, 0, 1);
  layout->addWidget (lbl2, 1, 0);
  layout->addWidget (d->maxvar, 1, 1);
  layout->addWidget (lbl3, 2, 0);
  layout->addWidget (d->caption, 2, 1);
  layout->addWidget (lbl4, 3, 0);
  layout->addWidget (d->btcolor, 3, 1);
  layout->addWidget (commonEditor, 4, 0, 1, 2);
}

void cGaugeEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->var->setText (data.strValue ("variable"));
  d->maxvar->setText (data.strValue ("max-variable"));
  d->caption->setText (data.strValue ("caption"));
  int color = data.intValue ("color");
  QColor c;
  c.setBlue (color % 256);
  color /= 256;
  c.setGreen (color % 256);
  color /= 256;
  c.setRed (color % 256);
  d->btcolor->setColor (c);
}

void cGaugeEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["variable"] = d->var->text();
  data->strValues["max-variable"] = d->maxvar->text();
  data->strValues["caption"] = d->caption->text();
  QColor c = d->btcolor->color ();
  data->intValues["color"] = c.red() * 256 * 256 + c.green() * 256 + c.blue();
}

#include "moc_cgaugeeditor.cpp"
