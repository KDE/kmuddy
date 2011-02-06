//
// C++ Implementation: cshortcuteditor
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2002-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cshortcuteditor.h"

#include "dialogs/dlggrabkey.h"
#include "cshortcut.h"
#include "cscripteditor.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QTabWidget>

#include <klineedit.h>
#include <klocale.h>
#include "kpushbutton.h"

struct cShortcutEditor::Private {
  KLineEdit *cmd;
  QCheckBox *chksendit, *chkoverwrite;
  QLabel *lblkey;
  int key, modifiers;
  cScriptEditor *script;
};

cShortcutEditor::cShortcutEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cShortcutEditor::~cShortcutEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cShortcutEditor::createGUI(QWidget *parent)
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
  d->cmd->setWhatsThis( i18n ("Command that will be executed when you press the"
      "defined key combination.\n"
      "Command can include aliases, script calls and similar stuff."));

  //key
  QLabel *lbl2 = new QLabel (i18n ("Shortcut"), basicPage);
  d->lblkey = new QLabel (basicPage);
  d->lblkey->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  d->lblkey->setLineWidth (2);
  KPushButton *btgrab = new KPushButton (i18n ("Grab shortcut..."), basicPage);
  connect (btgrab, SIGNAL (clicked ()), this, SLOT (grabKey ()));

  d->chksendit = new QCheckBox (i18n ("&Send command"), basicPage);
  d->chksendit->setWhatsThis( i18n ("If checked, the command will be sent "
      "to the MUD. Otherwise, it will be put in the inputline."));

  d->chkoverwrite = new QCheckBox (i18n ("&Overwrite existing text"), basicPage);
  d->chkoverwrite->setWhatsThis( i18n ("If there already is some text in "
      "the inputline, should it be overwritten?"));
  connect (d->chksendit, SIGNAL (toggled (bool)), d->chkoverwrite, SLOT (setDisabled (bool)));
   
  QWidget *commonEditor = createCommonAttribEditor (basicPage);

  //place'em there!
  basicLayout->setSpacing (5);
  basicLayout->addWidget (lbl1, 0, 0);
  basicLayout->addWidget (d->cmd, 0, 1, 1, 2);
  basicLayout->addWidget (lbl2, 1, 0);
  basicLayout->addWidget (d->lblkey, 1, 1);
  basicLayout->addWidget (btgrab, 1, 2);
  basicLayout->addWidget (d->chksendit, 2, 0, 1, 3);
  basicLayout->addWidget (d->chkoverwrite, 3, 0, 1, 3);
  basicLayout->addWidget (commonEditor, 4, 0, 1, 3);

  // initial state
  d->chksendit->setChecked (true);

  // the Script tab
  QFrame *scriptPage = new QFrame (tabs);
  QVBoxLayout *scriptlayout = new QVBoxLayout (scriptPage);
  d->script = new cScriptEditor (scriptPage);
  scriptlayout->addWidget (d->script);

  tabs->addTab (basicPage, i18n ("&General"));
  tabs->addTab (scriptPage, i18n ("&Script"));
}

void cShortcutEditor::fillGUI (const cListObjectData &data)
{
  // Common attributes
  fillCommonAttribEditor (data);

  d->cmd->setText (data.strValue ("command"));
  d->key = data.intValue ("key");
  d->modifiers = data.intValue ("modifiers");
  d->lblkey->setText (cShortcut::keyToString (d->key, d->modifiers));
  d->chksendit->setChecked (data.boolValue ("send"));
  d->chkoverwrite->setChecked (data.boolValue ("overwrite"));
  // Script
  d->script->setText (data.strValue ("script"));
}

void cShortcutEditor::getDataFromGUI (cListObjectData *data)
{
  // Comon attributes
  getDataFromCommonAttribEditor (data);

  data->strValues["command"] = d->cmd->text();
  data->intValues["key"] = d->key;
  data->intValues["modifiers"] = d->modifiers;
  data->boolValues["send"] = d->chksendit->isChecked();
  data->boolValues["overwrite"] = d->chkoverwrite->isChecked();
  // Script
  data->strValues["script"] = d->script->text();
}

void cShortcutEditor::grabKey ()
{
  dlgGrabKey *gk = new dlgGrabKey (this);
  if (gk->exec ())
  {
    d->key = gk->key();
    d->modifiers = gk->state();
    d->modifiers = d->modifiers & (Qt::KeyboardModifierMask | Qt::KeypadModifier);
    d->lblkey->setText (cShortcut::keyToString (d->key, d->modifiers));
  }
  delete gk;
  d->cmd->setFocus ();
}

#include "cshortcuteditor.moc"
