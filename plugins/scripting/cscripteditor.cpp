//
// C++ Implementation: cscripteditor
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

#include "cscripteditor.h"

#include "cactionmanager.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include "kfiledialog.h"
#include <klineedit.h>
#include <KLocalizedString>

struct cScriptEditor::Private {
  KLineEdit *workdir, *cmd, *sname;
  QCheckBox *sendusercommands, *useadvcomm, *singleinstance, *chkflowcontrol, *chkallowvars;
};

cScriptEditor::cScriptEditor (QWidget *parent)
  : cListEditor (parent)
{
  d = new Private;
}

cScriptEditor::~cScriptEditor ()
{
  // the GUI elements will be destroyed automatically
  delete d;
}

void cScriptEditor::createGUI(QWidget *parent)
{
  QGridLayout *layout = new QGridLayout (parent);

  //name
  QLabel *label1 = new QLabel (i18n ("Script &name"), parent);
  d->sname = new KLineEdit (parent);
  d->sname->setValidator (new QRegularExpressionValidator (QRegularExpression("^[0-9A-Za-z_]+$"), this));
  label1->setBuddy (d->sname);
  d->sname->setWhatsThis (i18n ("Name of this script."));
  d->sname->setFocus();
  
  //command
  QLabel *label2 = new QLabel (i18n ("&Command to execute"), parent);
  d->cmd = new KLineEdit (parent);
  label2->setBuddy (d->cmd);
  QPushButton *cmdbutton = new QPushButton (i18n ("Browse..."), parent);
  d->cmd->setWhatsThis (i18n ("Command to execute, including parameters. "
      "Note that you can specify more parameters when executing the command "
      "from the input line, if you enable the Allow parameters checkbox."));

  //workdir
  QLabel *label3 = new QLabel (i18n ("&Working directory"), parent);
  d->workdir = new KLineEdit (parent);
  label3->setBuddy (d->workdir);
  QPushButton *wrkbutton = new QPushButton (i18n ("Browse..."), parent);
  d->workdir->setWhatsThis (i18n ("Working directory for the script. Script "
      "will behave exactly as if it was run from this directory in console."));

  //options groupbox
  QGroupBox *options = new QGroupBox (i18n ("&Options"), parent);
  QGridLayout *optionsLayout = new QGridLayout (options);

  //send user commands
  d->sendusercommands = new QCheckBox (i18n ("Send user commands"), options);
  d->sendusercommands->setWhatsThis (i18n ("If script input is enabled, "
      "this enables sending of all sent commands to the script."));
  //adv.communication
  d->useadvcomm = new QCheckBox (i18n ("Enable communication"), options);
  d->useadvcomm->setWhatsThis (i18n ("Enables identification of lines of "
      "server output. Refer to the manual for more information."));
  //single-instance
  d->singleinstance = new QCheckBox (i18n ("Single-instance script"), options);
  d->singleinstance->setWhatsThis (i18n ("If an instance of this script "
      "is already running, another one can not be started."));  
  //no flow control
  d->chkflowcontrol = new QCheckBox (i18n ("Use flow control"), options);
  d->chkflowcontrol->setWhatsThis (i18n ("<p>If enabled, flow control will "
      "be used for this script. Flow control ensures that scripts "
      "are kept in sync, that means, that if two lines arrive from the "
      "server, the second one is not sent to the scripts until all running "
      "scripts have received the first one. However, in some situations, "
      "it's better not to use this feature.</p><p>If you don't need to "
      "disable flow control for some reason, leave this on.</p>"));
  //allow variables
  d->chkallowvars = new QCheckBox (i18n ("Communicate variables"), options);
  d->chkallowvars->setWhatsThis (i18n ("If enabled, a UNIX domain "
      "socket will be created, allowing you to obtain and modify actual "
      "variable values. More information in the documentation."));
  optionsLayout->addWidget (d->sendusercommands, 0, 0);
  optionsLayout->addWidget (d->useadvcomm, 1, 0);
  optionsLayout->addWidget (d->singleinstance, 2, 0);
  optionsLayout->addWidget (d->chkflowcontrol, 0, 1);
  optionsLayout->addWidget (d->chkallowvars, 1, 1);

  //activate Browse... buttons
  connect (cmdbutton, SIGNAL (clicked ()), this, SLOT (browse1()));
  connect (wrkbutton, SIGNAL (clicked ()), this, SLOT (browse2()));
    
  //place'em there!
  layout->setSpacing (5);
  layout->addWidget (label1, 0, 0);
  layout->addWidget (d->sname, 0, 1, 1, 2);
  layout->addWidget (label2, 2, 0);
  layout->addWidget (d->cmd, 2, 1);
  layout->addWidget (cmdbutton, 2, 2);
  layout->addWidget (label3, 3, 0);
  layout->addWidget (d->workdir, 3, 1);
  layout->addWidget (wrkbutton, 3, 2);
  layout->addWidget (options, 4, 0, 1, 3);
}

void cScriptEditor::browse1 ()
{
  //open some executable and place its name to the edit box
  if (!object()) return;
  cProfileSettings *sett = cProfileManager::self()->settings (object()->list()->session());
  if (!sett) return;
  QString fName = KFileDialog::getOpenFileName
        (sett->getString ("script-directory"), QString(), this, i18n ("Choose script"));
  if (!(fName.isEmpty()))
    d->cmd->setText (fName);
}

void cScriptEditor::browse2 ()
{
  QString fName = KFileDialog::getExistingDirectory (d->workdir->text(),
        this, i18n ("Choose working directory"));
  d->workdir->setText (fName);
}

void cScriptEditor::fillGUI (const cListObjectData &data)
{
  d->sname->setText (data.name);
  d->cmd->setText (data.strValue ("command"));
  d->workdir->setText (data.strValue ("work-directory"));
  d->sendusercommands->setChecked (data.boolValue ("send-user-commands"));
  d->useadvcomm->setChecked (data.boolValue ("adv-comunication"));
  d->singleinstance->setChecked (data.boolValue ("single-instance"));
  d->chkflowcontrol->setChecked (data.boolValue ("flow-control"));
  d->chkallowvars->setChecked (data.boolValue ("enable-variables"));
}

void cScriptEditor::getDataFromGUI (cListObjectData *data)
{
  data->name = d->sname->text();
  data->strValues["command"] = d->cmd->text();
  data->strValues["work-directory"] = d->workdir->text();
  data->boolValues["send-user-commands"] = d->sendusercommands->isChecked();
  data->boolValues["adv-comunication"] = d->useadvcomm->isChecked();
  data->boolValues["single-instance"] = d->singleinstance->isChecked();
  data->boolValues["flow-control"] = d->chkflowcontrol->isChecked();
  data->boolValues["enable-variables"] = d->chkallowvars->isChecked();
}

#include "moc_cscripteditor.cpp"
