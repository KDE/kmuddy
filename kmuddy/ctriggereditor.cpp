//
// C++ Implementation: ctriggereditor
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

#include "ctriggereditor.h"

#include "cactionmanager.h"
#include "cpattern.h"
#include "cscripteditor.h"
#include "cwindowlist.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <kcolorbutton.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <ktextedit.h>

#include <kregexpeditorinterface.h>
#include <kparts/componentfactory.h>

struct cTriggerEditor::Private {
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
  QCheckBox *check1, *check2, *check3, *check4;
  QComboBox *actMatched, *actNotMatched;

  // Colors
  QCheckBox *chkiscolor;
  QComboBox *clrtype, *fgselect, *bgselect;
  KColorButton *fgcolsel, *bgcolsel;
  KIntNumInput *backrefno;
  QTreeWidget *colorizations;
#define MAX_COLORIZATIONS 10
  int cnum;
  int fgcolor[MAX_COLORIZATIONS], bgcolor[MAX_COLORIZATIONS];
  QColor fgc[MAX_COLORIZATIONS], bgc[MAX_COLORIZATIONS];
  QString ctype[MAX_COLORIZATIONS];

  // Rewrite
  QCheckBox *chkrewrite;
  QComboBox *cbrewritevar;
  KIntNumInput *rewritebackrefno;
  KLineEdit *edrewritetext;

  // Special
  QCheckBox *chkgag, *chknotify, *chkprompt;
  QCheckBox *chksound;
  KLineEdit *edsoundname;

  // Windows
  QCheckBox *chkwindow;
  QComboBox *windowlist;
  QCheckBox *chkgagoutput;
  KLineEdit *wname;
};

//color list
QString *cltextk;
// type list
QString *ltype;

cTriggerEditor::cTriggerEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
  d->regExpDialog = 0;

  if (!cltextk) {
    cltextk = new QString[18];
    int i = 0;
    cltextk[i++] = i18n("Keep");
    cltextk[i++] = i18n("Black");
    cltextk[i++] = i18n("Red");
    cltextk[i++] = i18n("Green");
    cltextk[i++] = i18n("Yellow");
    cltextk[i++] = i18n("Blue");
    cltextk[i++] = i18n("Magenta");
    cltextk[i++] = i18n("Cyan");
    cltextk[i++] = i18n("Gray");
    cltextk[i++] = i18n("Dark gray");
    cltextk[i++] = i18n("Bright red");
    cltextk[i++] = i18n("Bright green");
    cltextk[i++] = i18n("Bright yellow");
    cltextk[i++] = i18n("Bright blue");
    cltextk[i++] = i18n("Bright magenta");
    cltextk[i++] = i18n("Bright cyan");
    cltextk[i++] = i18n("White");
    cltextk[i++] = i18n("Other");
  }
  if (!ltype) {
    ltype = new QString[7];
    int i = 0;
    ltype[i++] = i18n("Whole line");
    ltype[i++] = i18n("Matching text");
    ltype[i++] = i18n("Prefix");
    ltype[i++] = i18n("Suffix");
    ltype[i++] = i18n("Full prefix");
    ltype[i++] = i18n("Full suffix");
    ltype[i++] = i18n ("Backreference");
  }
}

cTriggerEditor::~cTriggerEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cTriggerEditor::createGUI(QWidget *parent)
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
  QLabel *cl = new QLabel (i18n ("&Trigger text"), basicPage);
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
  d->type->setWhatsThis( i18n ("Type of matching. Trigger will only be activated "
      "if this passes this type of test. Note that triggers are always matched as whole words."));
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
  QLabel *rcl = new QLabel (i18n ("&Command(s) to send:"), basicPage);
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
  d->variables->setRootIsDecorated (false);
  testarea->setWhatsThis( i18n("This is testing area. You can test your trigger "
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

  d->check1 = new QCheckBox (i18n ("Whole words only"), options);
  d->check1->setWhatsThis( i18n("When enabled, this trigger will only match "
    "the text if there are spaces or nothing before/after the matched string."));

  d->check2 = new QCheckBox (i18n ("Case sensitive"), options);
  d->check2->setWhatsThis( i18n ("When this option is on, upper case and "
      "lower case letters are treated as different characters, otherwise "
      "they're considered to be the same."));

  d->check3 = new QCheckBox (i18n ("Show but do not send"), options);
  d->check3->setWhatsThis (i18n ("When enabled, the text will not be sent to the MUD, it will only be "
      "displayed on the screen."));

  d->check4 =  new QCheckBox (i18n ("Global matching"), options);
  d->check4->setWhatsThis( i18n ("<p>With global matching, one trigger/trigger can match multiple "
      "times, if it contains the pattern more than once. For example, if pattern is abc, "
      "it matches only once on abcdabcd of global matching is off, but twice if it's on. "
      "For each match, the requested actions are performed - so the commands can be sent "
      "multiple times, once per match.</p>"
      "<p>Note that failing the condition does not terminate scanning, so you can use "
      "this to highlight names from a list (using the condition to check if a match is in the "
      "list), or something like that."));

  optionsBoxLayout->addWidget (d->check1, 0, 0);
  optionsBoxLayout->addWidget (d->check2, 0, 1);
  optionsBoxLayout->addWidget (d->check3, 1, 0);
  optionsBoxLayout->addWidget (d->check4, 1, 1);

  QWidget *commonEditor = createCommonAttribEditor (optionsPage);

  QWidget *matcheds = new QWidget (optionsPage);
  QGridLayout *matchedslayout = new QGridLayout (matcheds);
  
  QLabel *m1l = new QLabel ("Action if &matched", matcheds);
  d->actMatched = new QComboBox (matcheds);
  m1l->setBuddy (d->actMatched);
  d->actMatched->addItem (i18n ("Continue matching"));
  d->actMatched->addItem (i18n ("Stop matching"));
  d->actMatched->addItem (i18n ("Leave group"));

  QLabel *m2l = new QLabel ("Action if &not matched", matcheds);
  d->actNotMatched = new QComboBox (matcheds);
  m2l->setBuddy (d->actMatched);
  d->actNotMatched->addItem (i18n ("Continue matching"));
  d->actNotMatched->addItem (i18n ("Stop matching"));
  d->actNotMatched->addItem (i18n ("Leave group"));

  matchedslayout->setSpacing (10);
  matchedslayout->addWidget (m1l, 0, 0);
  matchedslayout->addWidget (d->actMatched, 0, 1);
  matchedslayout->addWidget (m2l, 1, 0);
  matchedslayout->addWidget (d->actNotMatched, 1, 1);
  matchedslayout->setColumnStretch (1, 1);

  optionslayout->setSpacing (10);
  optionslayout->addWidget (options);
  optionslayout->addWidget (matcheds);
  optionslayout->addWidget (commonEditor);

  // the Color replacement tab
  // TODO: this page is very user unfriendly, improve it !!!
  QFrame *colorPage = new QFrame (tabs);
  QVBoxLayout *colorlayout = new QVBoxLayout (colorPage);
  
  d->chkiscolor = new QCheckBox (i18n ("&Enable color replacement"), colorPage);
  QGroupBox *colorgroup = new QGroupBox (i18n ("Color replacement"), colorPage);
  QVBoxLayout *colorgroupLayout = new QVBoxLayout (colorgroup);

  QStringList types;
  for (int i = 0; i < 7; i++)
    types.append (ltype[i]);
  QStringList colorlist;
  for (int i = 0; i < 18; i++)
    colorlist.append (cltextk[i]);

  QWidget *onecolor = new QWidget (colorgroup);
  QGridLayout *onecolorlayout = new QGridLayout (onecolor);
  onecolorlayout->setSpacing (10);
  d->clrtype = new QComboBox (onecolor);
  d->clrtype->addItems (types);
  d->fgselect = new QComboBox (onecolor);
  d->fgselect->addItems (colorlist);
  d->bgselect = new QComboBox (onecolor);
  d->bgselect->addItems (colorlist);
  d->fgcolsel = new KColorButton (Qt::white, onecolor);
  d->bgcolsel = new KColorButton (Qt::black, onecolor);
  d->backrefno = new KIntNumInput (onecolor);
  d->backrefno->setLabel (i18n ("&Back-reference"), Qt::AlignLeft);
  d->backrefno->setRange (0, 100, 1);
  d->backrefno->setSliderEnabled (false);
  QPushButton *btadd = new QPushButton (i18n ("&Add"), onecolor);
  QPushButton *btremove = new QPushButton (i18n ("&Remove"), onecolor);
  QLabel *l1 = new QLabel (i18n ("&Colorize"), onecolor);
  l1->setBuddy (d->clrtype);
  QLabel *l2 = new QLabel (i18n ("&Foreground"), onecolor);
  l2->setBuddy (d->fgselect);
  QLabel *l3 = new QLabel (i18n ("&Background"), onecolor);
  l3->setBuddy (d->bgselect);

  onecolorlayout->addWidget (l1, 0, 0);
  onecolorlayout->addWidget (l2, 0, 1);
  onecolorlayout->addWidget (l3, 0, 2);
  onecolorlayout->addWidget (d->clrtype, 1, 0);
  onecolorlayout->addWidget (d->fgselect, 1, 1);
  onecolorlayout->addWidget (d->bgselect, 1, 2);
  onecolorlayout->addWidget (d->fgcolsel, 2, 1);
  onecolorlayout->addWidget (d->bgcolsel, 2, 2);
  onecolorlayout->addWidget (d->backrefno, 3, 0);
  onecolorlayout->addWidget (btadd, 3, 1);
  onecolorlayout->addWidget (btremove, 3, 2);

  d->colorizations = new QTreeWidget (colorgroup);
  d->colorizations->setHeaderLabels (QStringList() << i18n ("Type") << i18n ("Foreground") << i18n ("Background"));
  d->colorizations->setAllColumnsShowFocus (true);
  d->colorizations->setUniformRowHeights (true);
  d->colorizations->setRootIsDecorated (false);

  colorgroupLayout->addWidget (onecolor);
  colorgroupLayout->addWidget (d->colorizations);

  colorlayout->setSpacing (10);
  colorlayout->addWidget (d->chkiscolor);
  colorlayout->addWidget (colorgroup);

  connect (d->chkiscolor, SIGNAL (toggled (bool)), colorgroup, SLOT (setEnabled (bool)));
  connect (btadd, SIGNAL (clicked ()), this, SLOT (addColorization ()));
  connect (btremove, SIGNAL (clicked ()), this, SLOT (removeColorization ()));
  d->chkiscolor->setChecked (false);
  colorgroup->setEnabled (false);

  // the Rewrite text tab
  QFrame *rewritePage = new QFrame (tabs);
  QVBoxLayout *rewritelayout = new QVBoxLayout (rewritePage);
  
  d->chkrewrite = new QCheckBox (i18n ("Text &rewrite trigger"), rewritePage);
  connect (d->chkrewrite, SIGNAL (toggled (bool)), this, SLOT (rewriteChanged (bool)));

  QGroupBox *rewritegroup = new QGroupBox (i18n ("Rewrite text"), rewritePage);
  QGridLayout *rewritegrouplayout = new QGridLayout (rewritegroup);
  rewritegrouplayout->setSpacing (10);

  QLabel *l4 = new QLabel (i18n ("&Change this part of line:"), rewritegroup);
  d->cbrewritevar = new QComboBox (rewritegroup);
  d->cbrewritevar->addItems (types);
  l4->setBuddy (d->cbrewritevar);
  d->rewritebackrefno = new KIntNumInput (rewritegroup);
  d->rewritebackrefno->setLabel (i18n ("&Back-reference"), Qt::AlignLeft);
  d->rewritebackrefno->setRange (0, 100, 1);
  d->rewritebackrefno->setSliderEnabled (false);
  QLabel *l5 = new QLabel (i18n ("&To this value:"), rewritegroup);
  d->edrewritetext = new KLineEdit (rewritegroup);
  l5->setBuddy (d->edrewritetext);

  rewritegrouplayout->addWidget (l4, 0, 0);
  rewritegrouplayout->addWidget (l5, 2, 0);
  rewritegrouplayout->addWidget (d->cbrewritevar, 0, 1);
  rewritegrouplayout->addWidget (d->edrewritetext, 2, 1);
  rewritegrouplayout->addWidget (d->rewritebackrefno, 1, 0, 1, 2);
  rewritegrouplayout->addWidget (new QWidget (rewritegroup), 3, 0);
  rewritegrouplayout->setRowStretch (3, 0);
  
  connect (d->chkrewrite, SIGNAL (toggled (bool)), rewritegroup, SLOT (setEnabled (bool)));
  d->chkrewrite->setChecked (false);
  rewritegroup->setEnabled (false);
  
  rewritelayout->setSpacing (10);
  rewritelayout->addWidget (d->chkrewrite);
  rewritelayout->addWidget (rewritegroup);


  // the Special trigger tab
  QFrame *specialPage = new QFrame (tabs);
  QGridLayout *speciallayout = new QGridLayout (specialPage);
  d->chkgag = new QCheckBox (i18n ("Do not show (&gag) the line"), specialPage);
  d->chkgag->setWhatsThis( i18n ("This will prevent the matched line "
      "from being displayed in the output window."));
  d->chknotify = new QCheckBox (i18n ("&Notify"), specialPage);
  d->chknotify->setWhatsThis( i18n ("This setting only takes effect if you "
    "have disabled the \"Always notify\" option in Global settings and "
    "at least one of global/local notification is enabled. It will "
    "activate notification if the KMuddy window and/or this session "
    "is not active at the moment.\nUseful if you only want to be notified "
    "of some events (tells, combat, ...)."));

  d->chkprompt = new QCheckBox (i18n ("&Prompt-detection trigger"), specialPage);
  d->chkprompt->setWhatsThis( i18n ("Enabling this will turn this trigger "
    "into a prompt detection trigger, that is, text will be passed to these "
    "triggers even if no newline was received, the text will be displayed "
    "as a prompt if the trigger gets matched, and most other options will "
    "NOT be taken into effect - prompt-detect trigger cannot send out "
    "text, gag lines, do notification. Colorization is still possible."));
  d->chksound = new QCheckBox (i18n ("Play s&ound file"), specialPage);
  d->chksound->setWhatsThis( i18n ("This trigger will play a sound. Useful "
    "as a notification of important events, or to assign sounds to some "
    "events."));
  d->edsoundname = new KLineEdit (specialPage);
  QPushButton *button = new QPushButton ("&Browse...", specialPage);
  connect (button, SIGNAL (clicked ()), this, SLOT (browseForSoundFile ()));

  speciallayout->setSpacing (10);
  speciallayout->addWidget (d->chkgag, 0, 0);
  speciallayout->addWidget (d->chknotify, 1, 0);
  speciallayout->addWidget (d->chkprompt, 2, 0);
  speciallayout->addWidget (d->chksound, 3, 0);
  speciallayout->addWidget (d->edsoundname, 3, 1);
  speciallayout->addWidget (button, 3, 2);
  speciallayout->setRowStretch (4, 5);

  // the Output windows tab
  QFrame *windowPage = new QFrame (tabs);
  QGridLayout *windowlayout = new QGridLayout (windowPage);
  d->chkwindow = new QCheckBox (i18n ("Send output to a separate window"), windowPage);
  d->chkwindow->setWhatsThis( i18n ("This will send the matched text to a separate output window"));
  
  QGroupBox *outwindow = new QGroupBox(i18n ("Output Windows"), windowPage);
  QVBoxLayout *owlayout = new QVBoxLayout (outwindow);
  connect (d->chkwindow, SIGNAL (toggled (bool)), outwindow, SLOT (setEnabled (bool)));
  d->chkwindow->setChecked (false);
  outwindow->setEnabled (false);
  
  QLabel *wll = new QLabel("&Select window", outwindow);
  d->windowlist = new QComboBox(outwindow);
  wll->setBuddy(d->windowlist);
  d->windowlist->clear();
  
  d->chkgagoutput = new QCheckBox (i18n ("&Gag output in main window"), outwindow);
  d->chkgagoutput->setWhatsThis( i18n ("This will cause output sent to a separate window not to "
      "be displayed in main KMuddy session window"));

  QLabel *wnl = new QLabel (i18n ("&Window name"), outwindow);
  d->wname = new KLineEdit (outwindow);
  wnl->setBuddy(d->wname);

  QPushButton *createwindow = new QPushButton (i18n ("&Create window"), outwindow);
  connect (createwindow, SIGNAL (clicked ()), this, SLOT (createOutputWindow ()));

  owlayout->addWidget (wll);
  owlayout->addWidget (d->windowlist);
  owlayout->addWidget (d->chkgagoutput);
  owlayout->addWidget (wnl);
  owlayout->addWidget (d->wname);
  owlayout->addWidget (createwindow);
  owlayout->addStretch ();

  windowlayout->setSpacing(10);
  windowlayout->addWidget(d->chkwindow, 0, 0);
  windowlayout->addWidget(outwindow, 1, 0, 1, 3);


  //make testarea work!
  connect (d->text, SIGNAL (textChanged (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->cmd, SIGNAL (textChanged (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->rcmd, SIGNAL (textChanged ()), this, SLOT (updateTest ()));
  connect (d->type, SIGNAL (activated (const QString &)), this, SLOT (updateTest (const QString &)));
  connect (d->check1, SIGNAL (toggled(bool)), this, SLOT (updateTest (bool)));
  connect (d->check2, SIGNAL (toggled(bool)), this, SLOT (updateTest (bool)));


  tabs->addTab (basicTab, i18n ("&Basic"));
  tabs->addTab (scriptPage, i18n ("&Script"));
  tabs->addTab (optionsPage, i18n ("&Options"));
  tabs->addTab (colorPage, i18n ("Color replacement"));
  tabs->addTab (rewritePage, i18n ("Rewrite text"));
  tabs->addTab (specialPage, i18n ("Special trigger"));
  tabs->addTab (windowPage, i18n ("Output windows"));
}

void cTriggerEditor::updateEditButton (const QString &comboText)
{
  (comboText == "Regular expression") ? d->editregexp->show() : d->editregexp->hide();
}

void cTriggerEditor::editRegExp ()
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

void cTriggerEditor::updateTest ()
{
  updateTest (QString());
}

void cTriggerEditor::updateTest (bool)
{
  //this slot is activated when WholeWords status changed in trigger mode
  updateTest (QString());
}

void cTriggerEditor::updateTest (const QString &)
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
  pattern.setCaseSensitive (d->check2->isChecked ());

  pattern.setWholeWords (d->check1->isChecked ());

  bool ismatch = pattern.match (txt);

  //write results of the matching
  d->variables->clear ();
  if (!ismatch) {
    d->matched->setText (i18n ("This text did not match your trigger."));
    d->replacement->setText ("");
    return;
  }

  d->matched->setText (i18n ("This text matches your trigger."));
  QStringList ntext = d->rcmd->toPlainText().split ("\n");
  QStringList::iterator it;
  QString nt;
  for (it = ntext.begin(); it != ntext.end(); ++it)
  {
    QString text = *it;
    pattern.expandPseudoVariables (text);
    nt += text + "\n  ";
  }
  
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

void cTriggerEditor::addColorization ()
{
  if (d->cnum == MAX_COLORIZATIONS)  //limit reached!
  {
    KMessageBox::sorry (this, i18n ("Maximum number of colorizations reached."));
    return;
  }
  int fg = d->fgselect->currentIndex ();
  int bg = d->bgselect->currentIndex ();
  QString varname;
  int t = d->clrtype->currentIndex ();

  switch (t) {
    case 0: varname = "$line"; break;
    case 1: varname = "$matched"; break;
    case 2: varname = "$prefix"; break;
    case 3: varname = "$suffix"; break;
    case 4: varname = "$prefixfull"; break;
    case 5: varname = "$suffixfull"; break;
    case 6: varname = "$"; break;
  };
  if (t == 6)
    varname += QString::number (d->backrefno->value ());

  //values are ready, fill'em in!
  d->fgcolor[d->cnum] = fg;
  d->bgcolor[d->cnum] = bg;
  d->fgc[d->cnum] = d->fgcolsel->color();
  d->bgc[d->cnum] = d->bgcolsel->color();
  d->ctype[d->cnum] = varname;
  d->cnum++;
  updateColorizationsList ();
}

void cTriggerEditor::removeColorization ()
{
  QTreeWidgetItem *item = d->colorizations->currentItem();
  if (!item) return;
  int num = d->colorizations->indexOfTopLevelItem (item);
  if (num == -1) return;
  for (int i = num; i < d->cnum - 1; i++)
  {
    d->ctype[i] = d->ctype[i + 1];
    d->fgcolor[i] = d->fgcolor[i + 1];
    d->bgcolor[i] = d->bgcolor[i + 1];
    d->fgc[i] = d->fgc[i + 1];
    d->bgc[i] = d->bgc[i + 1];
  }
  d->cnum--;
  //this will also fix item numbers
  updateColorizationsList ();
}

void cTriggerEditor::updateColorizationsList ()
{
  //erase old data
  d->colorizations->clear ();

  //now I fill in data, item by item...
  //for each item:
  for (int i = 0; i < d->cnum; i++)
  {
    QString stype, sfg, sbg;

    //prepare strings
    QString varname = d->ctype[i];
    if (varname == "$line")
      stype = ltype[0];
    else if (varname == "$matched")
      stype = ltype[1];
    else if ((varname == "$prefix") || (varname == "$prefixtrim"))
      stype = ltype[2];
    else if ((varname == "$suffix") || (varname == "$suffixtrim"))
      stype = ltype[3];
    else if (varname == "$prefixfull")
      stype = ltype[4];
    else if (varname == "$suffixfull")
      stype = ltype[5];
    else if (varname[0] == '$')
    {
      //now it *should* be a back-reference
      varname = varname.mid(1);
      bool ok;
      varname.toInt (&ok);
      if (ok) //back-ref -> good :)
        stype = ltype[6] + " " + varname;
      else
        stype = i18n ("Unknown");
    }

    //fg/bg color
    //custom color is displayed in #rrggbb format... we may need to invent something better
    int fg = d->fgcolor[i];
    int bg = d->bgcolor[i];
    sfg = (fg == 17) ? d->fgc[i].name() : cltextk[fg];
    sbg = (bg == 17) ? d->bgc[i].name() : cltextk[bg];
    
    //fill'em in!
    QTreeWidgetItem *item = new QTreeWidgetItem (QStringList() << stype << sfg << sbg);
    d->colorizations->addTopLevelItem (item);
    if (i == d->cnum - 1)  //last item shall be selected
      d->colorizations->setCurrentItem (item);
  }
}

void cTriggerEditor::browseForSoundFile ()
{
  //open some file and place its name to the edit box
  QString fName = KFileDialog::getOpenFileName (QString(),
        "audio/x-wav audio/mpeg audio/ogg", this, i18n ("Choose sound file"));
  if (!(fName.isEmpty()))
    d->edsoundname->setText (fName);
}

void cTriggerEditor::createOutputWindow ()
{
  QString winname;
  
  winname = d->wname->text();
  if(!winname.isEmpty())
  {
    cWindowList *list = dynamic_cast<cWindowList *>(cActionManager::self()->object ("windowlist", object()->list()->session()));
    list->add(winname);
    d->windowlist->clear();
    QStringList wlist = list->windowList();
    d->windowlist->addItems(wlist);
  }
  else
    KMessageBox::sorry(this, i18n("Window name is empty!"));
}

void cTriggerEditor::rewriteChanged (bool val)
{
  if (val) {
    d->check4->setChecked (false);
    d->check4->setEnabled (false);
  }
  else {
    d->check4->setEnabled (true);
  }
}


void cTriggerEditor::fillGUI (const cListObjectData &data)
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
  d->check1->setChecked (data.boolValue ("whole-words"));
  d->check2->setChecked (data.boolValue ("cs"));
  d->check3->setChecked (data.boolValue ("dont-send"));
  d->check4->setChecked (data.boolValue ("global"));
  d->actMatched->setCurrentIndex (data.intValue ("action-matched"));
  d->actNotMatched->setCurrentIndex (data.intValue ("action-not-matched"));

  // Colors
  d->chkiscolor->setChecked (data.boolValue ("colorize"));
  d->cnum = 0;
  if (d->chkiscolor->isChecked()) {
    d->cnum = data.intValue ("colorize-count");
    for (int i = 0; i < d->cnum; i++) {
      if (i >= MAX_COLORIZATIONS) break;  // FIXME: limits are bad
      d->ctype[i] = data.strValue ("colorize-variable-"+QString::number(i+1));
      int fg = data.intValue ("colorize-fg-"+QString::number(i+1));
      int bg = data.intValue ("colorize-bg-"+QString::number(i+1));
      if (fg == 0)
        d->fgcolor[i] = 0;
      else if (fg > 0) {  // RGB
        d->fgcolor[i] = 17;
        fg--;
        d->fgc[i].setBlue (fg % 256);
        fg /= 256;
        d->fgc[i].setGreen (fg % 256);
        fg /= 256;
        d->fgc[i].setRed (fg % 256);
      } else  // ANSI
        d->fgcolor[i] = fg + 17;
      if (bg == 0)
        d->bgcolor[i] = 0;
      else if (bg > 0) {  // RGB
        d->bgcolor[i] = 17;
        bg--;
        d->bgc[i].setBlue (bg % 256);
        bg /= 256;
        d->bgc[i].setGreen (bg % 256);
        bg /= 256;
        d->bgc[i].setRed (bg % 256);
      } else  // ANSI
        d->bgcolor[i] = bg + 17;
    }
  }
  updateColorizationsList ();

  // Rewrite
  d->chkrewrite->setChecked (data.boolValue ("rewrite"));
  d->edrewritetext->setText (data.strValue ("rewrite-text"));
  QString varname = data.strValue ("rewrite-var");
  bool ok = false;
  int number = varname.toInt (&ok);
  if (ok) {
    d->cbrewritevar->setCurrentIndex (6);
    d->rewritebackrefno->setValue (number);
  } else {
    int item = -1;
    if (varname == "line") item = 0;
    else if (varname == "matched") item = 1;
    else if ((varname == "prefix") || (varname == "prefixtrim")) item = 2;
    else if ((varname == "suffix") || (varname == "suffixtrim")) item = 3;
    else if (varname == "prefixfull") item = 4;
    else if (varname == "suffixfull") item = 5;
    d->cbrewritevar->setCurrentIndex (item);
    d->rewritebackrefno->setValue (0);
  }

  // Special
  d->chkgag->setChecked (data.boolValue ("gag"));
  d->chknotify->setChecked (data.boolValue ("notify"));
  d->chkprompt->setChecked (data.boolValue ("prompt"));
  d->chksound->setChecked (data.boolValue ("sound"));
  d->edsoundname->setText (data.strValue ("sound-file"));

  // Output windows
  d->chkwindow->setChecked (data.boolValue ("output-window"));
  d->chkgagoutput->setChecked (data.boolValue ("output-gag-in-main"));
  d->windowlist->clear();
  cWindowList *list = dynamic_cast<cWindowList *>(cActionManager::self()->object ("windowlist", object()->list()->session()));
  if (list) {
    QStringList wlist = list->windowList();
    d->windowlist->addItems(wlist);
    d->windowlist->setCurrentIndex (wlist.indexOf (data.strValue ("output-window-name")));
  }

}

void cTriggerEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  // Basic
  data->strValues["pattern"] = d->cmd->text();
  data->intValues["matching"] = d->type->currentIndex();
  data->strValues["condition"] = d->condition->text();

  QStringList ntext = d->rcmd->toPlainText().split ("\n");
  data->intValues["newtext-count"] = ntext.size();
  QStringList::iterator it;
  int i;
  for (it = ntext.begin(), i = 1; it != ntext.end(); ++i, ++it)
    data->strValues["newtext-" + QString::number (i)] = *it;

  // Script
  data->strValues["script"] = d->script->text();

  // Options
  data->boolValues["whole-words"] = d->check1->isChecked();
  data->boolValues["cs"] = d->check2->isChecked();
  data->boolValues["dont-send"] = d->check3->isChecked();
  data->boolValues["global"] = d->check4->isChecked();
  data->intValues["action-matched"] = d->actMatched->currentIndex();
  data->intValues["action-not-matched"] = d->actNotMatched->currentIndex();

  // Colors
  data->boolValues["colorize"] = d->chkiscolor->isChecked();
  if (d->chkiscolor->isChecked()) {
    data->intValues["colorize-count"] = d->cnum;
    for (int i = 0; i < d->cnum; i++) {
      int fg = d->fgcolor[i];
      if (fg == 17) {  // RGB
        fg = d->fgc[i].red() * 256 * 256 + d->fgc[i].green() * 256 + d->fgc[i].blue()+ 1;
      } else if (fg > 0) fg -= 17;
      int bg = d->bgcolor[i];
      if (bg == 17) {  // RGB
        bg = d->bgc[i].red() * 256 * 256 + d->bgc[i].green() * 256 + d->bgc[i].blue()+ 1;
      } else if (bg > 0) bg -= 17;
      data->intValues["colorize-fg-"+QString::number(i+1)] = fg;
      data->intValues["colorize-bg-"+QString::number(i+1)] = bg;
      data->strValues["colorize-variable-"+QString::number(i+1)] = d->ctype[i];
    }
  }

  // Rewrite
  data->boolValues["rewrite"] = d->chkrewrite->isChecked ();
  data->strValues["rewrite-text"] = d->edrewritetext->text();
  QString varname;
  switch (d->cbrewritevar->currentIndex ())
  {
    case 0: varname = "line"; break;
    case 1: varname = "matched"; break;
    case 2: varname = "prefix"; break;
    case 3: varname = "suffix"; break;
    case 4: varname = "prefixfull"; break;
    case 5: varname = "suffixfull"; break;
    case 6: varname = QString::number (d->rewritebackrefno->value()); break;
  }
  data->strValues["rewrite-var"] = varname;

  // Special
  data->boolValues["gag"] = d->chkgag->isChecked ();
  data->boolValues["notify"] = d->chknotify->isChecked ();
  data->boolValues["prompt"] = d->chkprompt->isChecked ();
  data->boolValues["sound"] = d->chksound->isChecked ();
  data->strValues["sound-file"] = d->edsoundname->text ();

  // Output windows
  data->boolValues["output-window"] = d->chkwindow->isChecked ();
  data->boolValues["output-gag-in-main"] = d->chkgagoutput->isChecked();
  data->strValues["output-window-name"] = d->windowlist->currentText();

}

