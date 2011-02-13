//
// C++ Implementation: cbuttoneditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cbuttoneditor.h"
#include "cscripteditor.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>

#include <kicondialog.h>
#include <klineedit.h>
#include <klocale.h>

struct cButtonEditor::Private {
  KLineEdit *caption, *command, *command2;
  KIconButton *icon;
  QGroupBox *pushdown;
  cScriptEditor *script, *scriptrel;
};

cButtonEditor::cButtonEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cButtonEditor::~cButtonEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cButtonEditor::createGUI(QWidget *parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout (parent);
  QTabWidget *tabs = new QTabWidget (parent);
  mainLayout->addWidget (tabs);

  QFrame *basicPage = new QFrame (tabs);
  QGridLayout *basicLayout = new QGridLayout (basicPage);
  
  // caption
  QLabel *lbl1 = new QLabel (i18n ("&Caption"), basicPage);
  d->caption = new KLineEdit (basicPage);
  lbl1->setBuddy (d->caption);
  d->caption->setWhatsThis( i18n ("Caption of this button."));

  //command
  QLabel *lblc1 = new QLabel (i18n ("Co&mmand"), basicPage);
  d->command = new KLineEdit (basicPage);
  lblc1->setBuddy (d->command);
  d->command->setWhatsThis( i18n ("Command that will be executed if you click on the button. "
      "Note that you can enter multiple commands here (separated by semi-colons, "
      "or another character defined in <b>global settings</b>)."));
  
 //icon
  QLabel *il = new QLabel ("&Icon", basicPage);
  d->icon = new KIconButton (basicPage);
  d->icon->setIconSize (32);
  il->setBuddy (d->icon);
  d->icon->setWhatsThis (i18n ("Icon displayed on the button toolbar."));

  // pushdown
  d->pushdown = new QGroupBox ("&Push-down button", basicPage);
  d->pushdown->setCheckable (true);
  QHBoxLayout *pushdownlayout = new QHBoxLayout (d->pushdown);

  //command2
  QLabel *lblc2 = new QLabel (i18n ("&Un-press command"), d->pushdown);
  d->command2 = new KLineEdit (d->pushdown);
  lblc2->setBuddy (d->command2);
  d->command2->setWhatsThis (i18n ("Command that will be executed if you click on the push-down "
        "button while it's pushed down."));
  pushdownlayout->addWidget (lblc2);
  pushdownlayout->addWidget (d->command2);
 
  QWidget *commonEditor = createCommonAttribEditor (basicPage);

  //place'em there!
  basicLayout->setSpacing (5);
  basicLayout->addWidget (lbl1, 0, 0);
  basicLayout->addWidget (d->caption, 0, 1);
  basicLayout->addWidget (lblc1, 1, 0);
  basicLayout->addWidget (d->command, 1, 1);
  basicLayout->addWidget (il, 2, 0);
  basicLayout->addWidget (d->icon, 2, 1);
  basicLayout->addWidget (d->pushdown, 3, 0, 1, 2);
  basicLayout->addWidget (commonEditor, 4, 0, 1, 2);

  // the Script tabs
  QFrame *scriptPage = new QFrame (tabs);
  QVBoxLayout *scriptlayout = new QVBoxLayout (scriptPage);
  d->script = new cScriptEditor (scriptPage);
  scriptlayout->addWidget (d->script);

  QFrame *scriptPage2 = new QFrame (tabs);
  QVBoxLayout *scriptlayout2 = new QVBoxLayout (scriptPage2);
  d->scriptrel = new cScriptEditor (scriptPage2);
  scriptlayout2->addWidget (d->scriptrel);

  tabs->addTab (basicPage, i18n ("&General"));
  tabs->addTab (scriptPage, i18n ("&Script"));
  tabs->addTab (scriptPage2, i18n ("Script when &released"));
}

void cButtonEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->command->setText (data.strValue ("command"));
  d->command2->setText (data.strValue ("command-released"));
  d->caption->setText (data.strValue ("caption"));
  d->icon->setIcon (data.strValue ("icon"));
  d->pushdown->setChecked (data.boolValue ("pushdown"));

  // Script
  d->script->setText (data.strValue ("script"));
  d->scriptrel->setText (data.strValue ("script-release"));
}

void cButtonEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["command"] = d->command->text();
  data->strValues["command-released"] = d->command2->text();
  data->strValues["caption"] = d->caption->text();
  data->strValues["icon"] = d->icon->icon();
  data->boolValues["pushdown"] = d->pushdown->isChecked();

  // Script
  data->strValues["script"] = d->script->text();
  data->strValues["script-release"] = d->scriptrel->text();
}

#include "cbuttoneditor.moc"
