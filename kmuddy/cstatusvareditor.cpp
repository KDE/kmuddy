//
// C++ Implementation: cstatusvareditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cstatusvareditor.h"

#include "cactionmanager.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QRegExpValidator>

#include <klineedit.h>
#include <klocale.h>

struct cStatusVarEditor::Private {
  KLineEdit *var, *maxvar, *caption;
  QCheckBox *chkpercent;
};

cStatusVarEditor::cStatusVarEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cStatusVarEditor::~cStatusVarEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cStatusVarEditor::createGUI(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout (parent);
  
  //variable
  QLabel *lbl1 = new QLabel (i18n ("&Variable name"), parent);
  d->var = new KLineEdit (parent);
  d->var->setValidator (new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this));
  lbl1->setBuddy (d->var);
  d->var->setWhatsThis( i18n ("Variable displayed by this status variable."));
  
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

  //percent
  d->chkpercent = new QCheckBox (i18n ("&Show as percentage"), parent);
  d->chkpercent->setWhatsThis( i18n ("When enabled, this variable will be shown as percentage. "
      "(of max.value, or of 100 if no max.value given)"));
  
  QWidget *commonEditor = createCommonAttribEditor (parent);

  //place'em there!
  layout->setSpacing (5);
  layout->addWidget (lbl1, 0, 0);
  layout->addWidget (d->var, 0, 1);
  layout->addWidget (lbl2, 1, 0);
  layout->addWidget (d->maxvar, 1, 1);
  layout->addWidget (lbl3, 2, 0);
  layout->addWidget (d->caption, 2, 1);
  layout->addWidget (d->chkpercent, 3, 0, 1, 2);
  layout->addWidget (commonEditor, 4, 0, 1, 2);
}

void cStatusVarEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->var->setText (data.strValue ("variable"));
  d->maxvar->setText (data.strValue ("max-variable"));
  d->caption->setText (data.strValue ("caption"));
  d->chkpercent->setChecked (data.boolValue ("percentage"));
}

void cStatusVarEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["variable"] = d->var->text();
  data->strValues["max-variable"] = d->maxvar->text();
  data->strValues["caption"] = d->caption->text();
  data->boolValues["percentage"] = d->chkpercent->isChecked();
}

#include "cstatusvareditor.moc"
