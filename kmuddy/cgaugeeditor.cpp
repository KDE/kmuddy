//
// C++ Implementation: cgaugeeditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cgaugeeditor.h"

#include "cactionmanager.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QRegExpValidator>

#include <kcolorbutton.h>
#include <klineedit.h>
#include <klocale.h>

struct cGaugeEditor::Private {
  KLineEdit *var, *maxvar, *caption;
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
  d->var = new KLineEdit (parent);
  d->var->setValidator (new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this));
  lbl1->setBuddy (d->var);
  d->var->setWhatsThis( i18n ("Variable displayed by this gauge."));
  
  //max variable
  QLabel *lbl2 = new QLabel (i18n ("&Max. variable (optional)"), parent);
  d->maxvar = new KLineEdit (parent);
  d->maxvar->setValidator (new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this));
  lbl2->setBuddy (d->maxvar);
  d->maxvar->setWhatsThis( i18n ("Variable holding the maximum value (default=100)"));
  
  //caption
  QLabel *lbl3 = new QLabel (i18n ("&Caption"), parent);
  d->caption = new KLineEdit (parent);
  lbl3->setBuddy (d->caption);
  d->caption->setWhatsThis( i18n ("Caption shown next to variable value."));

  // color
  QLabel *lbl4 = new QLabel (i18n ("&Caption"), parent);
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

#include "cgaugeeditor.moc"
