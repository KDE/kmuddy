//
// C++ Implementation: ctimereditor
//
// Description: timer editor
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

#include "ctimereditor.h"

#include "cactionmanager.h"
#include "cscripteditor.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>

#include <klineedit.h>
#include <klocale.h>
#include <knuminput.h>

struct cTimerEditor::Private {
  KLineEdit *cmd, *advcmd;
  KIntNumInput *interval, *advinterval;
  QCheckBox *chksingleshot, *chkadvance;
  cScriptEditor *script;
};

cTimerEditor::cTimerEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cTimerEditor::~cTimerEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cTimerEditor::createGUI(QWidget *parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout (parent);
  QTabWidget *tabs = new QTabWidget (parent);
  mainLayout->addWidget (tabs);

  QFrame *basicPage = new QFrame (tabs);
  QGridLayout *basicLayout = new QGridLayout (basicPage);
  
  //command
  QLabel *lbl1 = new QLabel (i18n ("&Command"), basicPage);
  d->cmd = new KLineEdit (basicPage);
  lbl1->setBuddy (d->cmd);
  d->cmd->setWhatsThis( i18n ("Command that will be executed on each tick.\n"
      "Command can include aliases, script calls and similar stuff."));

  //interval
  QLabel *lbl2 = new QLabel (i18n ("&Interval"), basicPage);
  d->interval = new KIntNumInput (basicPage);
  d->interval->setMinimum (1);
  d->interval->setMaximum (3600);  //max = 1 hod.
  d->interval->setSuffix (i18n (" secs"));
  lbl2->setBuddy (d->interval);

  //single-shot
  d->chksingleshot = new QCheckBox (i18n ("&Single-shot"), basicPage);
  d->chksingleshot->setWhatsThis( i18n ("When enabled, the timer will only be executed once and then disabled."));
  
  d->chkadvance = new QCheckBox (i18n ("&Run a command in advance"), basicPage);
  d->chkadvance->setWhatsThis( i18n ("Run a command a given number of seconds before each timer tick."));

  QGroupBox *grpadvance = new QGroupBox (i18n ("Advance command"), basicPage);
  QGridLayout *advancelayout = new QGridLayout (grpadvance);

  QLabel *lbl3 = new QLabel (i18n ("&Seconds before tick"), grpadvance);
  d->advinterval = new KIntNumInput (grpadvance);
  d->advinterval->setMinimum (1);
  d->advinterval->setMaximum (180);
  d->advinterval->setSuffix (i18n (" secs"));
  lbl3->setBuddy (d->interval);

  QLabel *lbl4 = new QLabel (i18n ("C&ommand"), basicPage);
  d->advcmd = new KLineEdit (basicPage);
  lbl4->setBuddy (d->advcmd);
  d->cmd->setWhatsThis( i18n ("Command that will be executed the specified time before each tick.\n"
      "Command can include aliases, script calls and similar stuff."));

  advancelayout->addWidget (lbl3, 0, 0);
  advancelayout->addWidget (d->advinterval, 0, 1);
  advancelayout->addWidget (lbl4, 1, 0);
  advancelayout->addWidget (d->advcmd, 1, 1);

  QWidget *commonEditor = createCommonAttribEditor (basicPage);

  //place'em there!
  basicLayout->setSpacing (5);
  basicLayout->addWidget (lbl1, 0, 0);
  basicLayout->addWidget (d->cmd, 0, 1);
  basicLayout->addWidget (lbl2, 1, 0);
  basicLayout->addWidget (d->interval, 1, 1);
  basicLayout->addWidget (d->chksingleshot, 2, 0, 1, 2);
  basicLayout->addWidget (d->chkadvance, 3, 0, 1, 2);
  basicLayout->addWidget (grpadvance, 4, 0, 1, 2);
  basicLayout->addWidget (commonEditor, 5, 0, 1, 2);

  connect (d->chkadvance, SIGNAL (toggled(bool)), grpadvance, SLOT (setEnabled (bool)));
  // initial state
  d->chkadvance->setChecked (false);
  grpadvance->setEnabled (false);

  // the Script tab
  QFrame *scriptPage = new QFrame (tabs);
  QVBoxLayout *scriptlayout = new QVBoxLayout (scriptPage);
  d->script = new cScriptEditor (scriptPage);
  scriptlayout->addWidget (d->script);

  tabs->addTab (basicPage, i18n ("&General"));
  tabs->addTab (scriptPage, i18n ("&Script"));
}

void cTimerEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->cmd->setText (data.strValue ("command"));
  d->interval->setValue (data.intValue ("interval"));
  d->chksingleshot->setChecked (data.boolValue ("single-shot"));
  d->chkadvance->setChecked (data.boolValue ("advance"));
  d->advcmd->setText (data.strValue ("command-advance"));
  d->advinterval->setValue (data.intValue ("advance-seconds"));
  // Script
  d->script->setText (data.strValue ("script"));
}

void cTimerEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["command"] = d->cmd->text();
  data->intValues["interval"] = d->interval->value();
  data->boolValues["single-shot"] = d->chksingleshot->isChecked();
  data->boolValues["advance"] = d->chkadvance->isChecked();
  data->strValues["command-advance"] = d->advcmd->text();
  data->intValues["advance-seconds"] = d->advinterval->value();
  // Script
  data->strValues["script"] = d->script->text();
}

#include "moc_ctimereditor.cpp"
