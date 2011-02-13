//
// C++ Implementation: caliaseditor
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

#include "caliaseditor.h"

#include "cpattern.h"
#include "cscripteditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <klineedit.h>
#include <klocale.h>
#include <ktextedit.h>

#include <kregexpeditorinterface.h>
#include <kparts/componentfactory.h>

struct cAliasEditor::Private {
  // Basic
  KLineEdit *cmd;
  QComboBox *type;
  KLineEdit *condition;
  KTextEdit *rcmd;
  QPushButton *editregexp;
  QDialog *regExpDialog;

  // Basic - testarea
  KLineEdit *text;
  QLabel *matched, *replacement;
  QTreeWidget *variables;

  // Script
  cScriptEditor *script;

  // Options
  QCheckBox *check1, *check2, *check3, *check4, *check5;
};

cAliasEditor::cAliasEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
  d->regExpDialog = 0;
}

cAliasEditor::~cAliasEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cAliasEditor::createGUI(QWidget *parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout (parent);
  QTabWidget *tabs = new QTabWidget (parent);
  mainLayout->addWidget (tabs);

  // the Basic tab
  QSplitter *basicTab = new QSplitter (tabs);
  basicTab->setOrientation (Qt::Vertical);
  QFrame *basicPage = new QFrame (basicTab);
  QGridLayout *basiclayout = new QGridLayout (basicPage);

  // command
  QLabel *cl = new QLabel (i18n ("&Alias text"), basicPage);
  d->cmd = new KLineEdit (basicPage);
  cl->setBuddy (d->cmd);
  d->cmd->setWhatsThis (i18n ("Command that will be replaced if you enter it."));
  
  //comparison type
  QWidget *hbComboEdit = new QWidget (basicPage);
  QHBoxLayout *comboEditLayout = new QHBoxLayout (hbComboEdit);;
  comboEditLayout->setSpacing(3);
  QLabel *ctl = new QLabel ("&Comparison type", basicPage);
  d->type = new QComboBox (hbComboEdit);
  ctl->setBuddy (d->type);
  d->type->clear ();
  d->type->addItem (i18n ("Exact match"));
  d->type->addItem (i18n ("Sub-string"));
  d->type->addItem (i18n ("Begins with"));
  d->type->addItem (i18n ("Ends with"));
  d->type->addItem (i18n ("Regular expression"));
  d->type->setWhatsThis( i18n ("Type of matching. Alias will only be activated "
      "if this passes this type of test. Note that aliases are always matched as whole words."));
  comboEditLayout->addWidget (d->type);

  //Check if kdeutils kregeditor is available...
  if (!KServiceTypeTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty()) {
  
    d->editregexp = new QPushButton (i18n ("&Edit..."), hbComboEdit);
    connect (d->editregexp, SIGNAL (clicked ()), this, SLOT (editRegExp()));  
    //hide / show button depending on what is activated in the 'type' combobox
    connect (d->type, SIGNAL (activated (const QString &)), this, SLOT (updateEditButton (const QString &)));
    comboEditLayout->addWidget (d->editregexp);
  }
  
  //condition
  QLabel *cndl = new QLabel ("Con&dition", basicPage);
  d->condition = new KLineEdit (basicPage);
  cndl->setBuddy (d->condition);
  d->condition->setWhatsThis( i18n ("Conditional triggering. If this is set, the actions "
      "will only fire if the condition is true (that means, if it evaluates as non-zero)."));

   //replacement command(s)
  QLabel *rcl = new QLabel (i18n ("&Replacement text(s)"), basicPage);
  d->rcmd = new KTextEdit (basicPage);
  d->rcmd->setWordWrapMode (QTextOption::NoWrap);
  rcl->setBuddy (d->rcmd);
  d->rcmd->setWhatsThis( i18n ("Command(s) that will replace command you've entered."));

  //test area
  QGroupBox *testarea = new QGroupBox (i18n ("Test area"), basicTab);
  QGridLayout *testlayout = new QGridLayout (testarea);
  QLabel *textlabel = new QLabel (i18n ("&Text: "), testarea);
  d->text = new KLineEdit (testarea);
  textlabel->setBuddy (d->text);
  d->matched = new QLabel ("", testarea);
  d->replacement = new QLabel ("", testarea);
  d->variables = new QTreeWidget (testarea);
  d->variables->setHeaderLabels (QStringList() << i18n ("Name") << i18n ("Value"));
  d->variables->setAllColumnsShowFocus (true);
  d->variables->setUniformRowHeights (true);
  d->variables->setRootIsDecorated (true);
  testarea->setWhatsThis( i18n("This is testing area. You can test your alias "
        "here to see if it does what you want it to do. Simply "
        "type in some text and see what happens."));
  testlayout->setSpacing (5);
  testlayout->addWidget (textlabel, 0, 0);
  testlayout->addWidget (d->text, 0, 1);
  testlayout->addWidget (d->matched, 1, 0, 1, 2);
  testlayout->addWidget (d->replacement, 2, 0, 1, 2);
  testlayout->addWidget (d->variables, 3, 0, 1, 2);

  basiclayout->setSpacing (5);
  basiclayout->addWidget (cl, 0, 0);
  basiclayout->addWidget (d->cmd, 0, 1);
  basiclayout->addWidget (ctl, 1, 0);
  basiclayout->addWidget (hbComboEdit, 1, 1);
  basiclayout->addWidget (cndl, 2, 0);
  basiclayout->addWidget (d->condition, 2, 1);
  basiclayout->addWidget (rcl, 3, 0);
  basiclayout->addWidget (d->rcmd, 4, 0, 1, 2);
  basiclayout->setRowStretch (4, 10);

  basicTab->addWidget (basicPage);
  basicTab->addWidget (testarea);

  // the Script tab
  QFrame *scriptPage = new QFrame (tabs);
  QVBoxLayout *scriptlayout = new QVBoxLayout (scriptPage);
  d->script = new cScriptEditor (scriptPage);
  scriptlayout->addWidget (d->script);

  // the Options tab
  QFrame *optionsPage = new QFrame (tabs);
  QVBoxLayout *optionslayout = new QVBoxLayout (optionsPage);

  QGroupBox *options = new QGroupBox (i18n ("&Options"), optionsPage);
  QGridLayout *optionsBoxLayout = new QGridLayout (options);

  d->check1 = new QCheckBox (i18n ("Send original command"), options);
  d->check1->setWhatsThis( i18n ("If this alias matches your command, "
        "a replacement command will be sent. If you enable this option, "
        "both original and replacement commands are sent (original command "
        "is sent first)."));

  d->check2 = new QCheckBox (i18n ("Whole words only"), options);
  d->check2->setWhatsThis( i18n("When enabled, this alias will only match "
    "the text if there are spaces or nothing before/after the matched string."));

  d->check3 = new QCheckBox (i18n ("Case sensitive"), options);
  d->check3->setWhatsThis( i18n ("When this option is on, upper case and "
      "lower case letters are treated as different characters, otherwise "
      "they're considered to be the same."));

  d->check4 = new QCheckBox (i18n ("Include prefix/suffix"), options);
  d->check4->setWhatsThis (i18n ("Prefix/suffix of matched text "
      "will be automatically appended to the expanded text."));

  d->check5 = new QCheckBox (i18n ("Global matching"), options);
  d->check5->setWhatsThis( i18n ("<p>With global matching, one alias/trigger can match multiple "
      "times, if it contains the pattern more than once. For example, if pattern is abc, "
      "it matches only once on abcdabcd of global matching is off, but twice if it's on. "
      "For each match, the requested actions are performed - so the commands can be sent "
      "multiple times, once per match.</p>"
      "<p>Note that failing the condition doesn't terminate scanning, so you can use "
      "this to highlight names from a list (using the condition to check if a match is in the "
      "list), or something like that."));

  optionsBoxLayout->addWidget (d->check1, 0, 1);
  optionsBoxLayout->addWidget (d->check2, 0, 0);
  optionsBoxLayout->addWidget (d->check3, 1, 0);
  optionsBoxLayout->addWidget (d->check4, 1, 1);
  optionsBoxLayout->addWidget (d->check5, 2, 0);

  QWidget *commonEditor = createCommonAttribEditor (optionsPage);

  optionslayout->setSpacing (10);
  optionslayout->addWidget (options);
  optionslayout->addWidget (commonEditor);


  //make testarea work!
  connect (d->text, SIGNAL (textChanged (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->cmd, SIGNAL (textChanged (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->rcmd, SIGNAL (textChanged ()), this, SLOT (updateTest ()));
  connect (d->type, SIGNAL (activated (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->check3, SIGNAL (toggled(bool)), this, SLOT (updateTest (bool)));
  connect (d->check2, SIGNAL (toggled(bool)), this, SLOT (updateTest (bool)));
  connect (d->check4, SIGNAL (toggled(bool)), this, SLOT (updateTest (bool)));


  tabs->addTab (basicTab, i18n ("&Basic"));
  tabs->addTab (scriptPage, i18n ("&Script"));
  tabs->addTab (optionsPage, i18n ("&Options"));
}

void cAliasEditor::updateEditButton (const QString &comboText)
{
  (comboText == "Regular expression") ? d->editregexp->show() : d->editregexp->hide();
}

void cAliasEditor::editRegExp ()
{
  if (!d->regExpDialog)
    d->regExpDialog = KServiceTypeTrader::createInstanceFromQuery<QDialog>("KRegExpEditor/KRegExpEditor");

  KRegExpEditorInterface *regExpEditor = dynamic_cast<KRegExpEditorInterface *>(d->regExpDialog);
  if (!regExpEditor) return;

  //get text from cmd and put it into regExp GUI
  regExpEditor->setRegExp (d->cmd->text());

  // execute the dialog
  bool result = d->regExpDialog->exec();

  //if ok, set the cmd to the regExp created
  if (result) d->cmd->setText (regExpEditor->regExp());
}

void cAliasEditor::updateTest ()
{
  updateTest (QString());
}

void cAliasEditor::updateTest (bool)
{
  //this slot is activated when WholeWords status changed in alias mode
  updateTest (QString());
}

void cAliasEditor::updateTest (const QString &)
{
  QString txt = d->text->text();
  // pattern used to test
  cPattern pattern;

  //set it up with dialog data
  pattern.setPattern (d->cmd->text ());
  cPattern::PatternType pt;
  switch (d->type->currentIndex ()) {
    case 0: pt = cPattern::exact; break;
    case 1: pt = cPattern::substring; break;
    case 2: pt = cPattern::begin; break;
    case 3: pt = cPattern::end; break;
    case 4: pt = cPattern::regexp; break;
    default: pt = cPattern::begin;
  }
  pattern.setMatching (pt);
  pattern.setCaseSensitive (d->check3->isChecked ());

  pattern.setWholeWords (d->check2->isChecked ());

  bool prefixsuffix = d->check4->isChecked ();

  bool ismatch = pattern.match (txt);

  //write results of the matching
  d->variables->clear ();
  if (!ismatch) {
    d->matched->setText (i18n ("This text did not match your alias."));
    d->replacement->setText ("");
    return;
  }

  d->matched->setText (i18n ("This text matches your alias."));
  QStringList ntext = d->rcmd->toPlainText().split ("\n");
  QString nt;
  QStringList::iterator it;
  //we cannot use QStringList::join because of prefix/suffix stuff
  for (it = ntext.begin(); it != ntext.end(); ++it)
  {
    QString text = *it;
    pattern.expandPseudoVariables (text);
    if (prefixsuffix)
      text = pattern.getPrefix() + text + pattern.getSuffix();
    nt += text + "\n  ";
  }
  
  d->replacement->setText (i18n ("Command: ") + nt);
  QList<QTreeWidgetItem *> items;
  items << new QTreeWidgetItem (d->variables, QStringList() << "$matched" << pattern.getVariable ("matched"));
  items << new QTreeWidgetItem (d->variables, QStringList() << "$matched" << pattern.getVariable ("matched"));
  items << new QTreeWidgetItem (d->variables, QStringList() << "$prefix" << pattern.getVariable ("prefix"));
  items << new QTreeWidgetItem (d->variables, QStringList() << "$suffix" << pattern.getVariable ("suffix"));
  items << new QTreeWidgetItem (d->variables, QStringList() << "$prefixfull" << pattern.getVariable ("prefixfull"));
  items << new QTreeWidgetItem (d->variables, QStringList() << "$suffixfull" << pattern.getVariable ("suffixfull"));
  if (pattern.matching() == cPattern::regexp)
    for (int i = 0; i < pattern.getBackRefList().count(); i++)
      items << new QTreeWidgetItem (d->variables, QStringList() << "$"+QString::number(i) << pattern.getBackRefList()[i]);
  d->variables->addTopLevelItems (items);
}


void cAliasEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  // Basic
  d->cmd->setText (data.strValue ("pattern"));
  d->type->setCurrentIndex (data.intValue ("matching"));
  d->condition->setText (data.strValue ("condition"));
  QStringList newtext;
  for (int i = 1; i <= data.intValue ("newtext-count"); ++i)
    newtext << data.strValue ("newtext-" + QString::number (i));
  d->rcmd->setPlainText (newtext.join("\n"));

  // Script
  d->script->setText (data.strValue ("script"));

  // Options

  d->check1->setChecked (data.boolValue ("orig"));
  d->check2->setChecked (data.boolValue ("whole-words"));
  d->check3->setChecked (data.boolValue ("cs"));
  d->check4->setChecked (data.boolValue ("prefix-suffix"));
  d->check5->setChecked (data.boolValue ("global"));
}

void cAliasEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  // Basic
  data->strValues["pattern"] = d->cmd->text();
  data->intValues["matching"] = d->type->currentIndex();
  data->strValues["condition"] = d->condition->text();

  QStringList ntext = d->rcmd->toPlainText().split ("\n");
  data->intValues["newtext-count"] = ntext.size();
  QStringList::iterator it; int i;
  for (i = 1, it = ntext.begin(); it != ntext.end(); ++i, ++it)
    data->strValues["newtext-" + QString::number (i)] = *it;

  // Script
  data->strValues["script"] = d->script->text();

  // Options
  data->boolValues["orig"] = d->check1->isChecked();
  data->boolValues["whole-words"] = d->check2->isChecked();
  data->boolValues["cs"] = d->check3->isChecked();
  data->boolValues["prefix-suffix"] = d->check4->isChecked();
  data->boolValues["global"] = d->check5->isChecked();
}

#include "caliaseditor.moc"
