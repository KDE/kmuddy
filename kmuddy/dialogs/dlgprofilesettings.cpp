/***************************************************************************
                          dlgprofilesettings.cpp  -  profile settings
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : oct 18 2002
    copyright            : (C) 2002-2007 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dlgprofilesettings.h"

#include "cdirlist.h"
#include "cdialoglist.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>

#include <kapplication.h>
#include <kcodecaction.h>
#include <klineedit.h>
#include <KLocalizedString>

dlgProfileSettings::dlgProfileSettings (QWidget *parent) : KPageDialog (parent)
{
  cDialogList::self()->addDialog ("profile-prefs", this);

  //initial dialog size
  setWindowTitle (i18n ("MUD preferences"));
  setStandardButtons (QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
  setFaceType (KPageDialog::List);

  //prepare dialog pages
  KPageWidgetItem *item;
  QFrame *frmconn = new QFrame (this);
  item = addPage (frmconn, i18n ("Connection"));
  item->setIcon (QIcon::fromTheme ("connection-established"));
  QFrame *frmcommands = new QFrame (this);
  item = addPage (frmcommands, i18n ("Commands"));
  item->setIcon (QIcon::fromTheme ("gear"));
  QFrame *frmdirs = new QFrame (this);
  item = addPage (frmdirs, i18n ("Folders"));
  item->setIcon (QIcon::fromTheme ("folder"));
  QFrame *frmsound = new QFrame (this);
  item = addPage (frmsound, i18n ("Sound"));
  item->setIcon (QIcon::fromTheme ("media-playback-start"));
#ifdef HAVE_MXP
  QFrame *frmmxp = new QFrame (this);
  item = addPage (frmmxp, i18n ("MXP"));
  item->setIcon (QIcon::fromTheme ("go-jump"));
#endif

  //page 1
  QGridLayout *connlayout = new QGridLayout (frmconn);
  useansi = new QCheckBox (i18n ("&Use colorized output (if available)"), frmconn);
  limitrepeater = new QCheckBox (i18n ("&Do not allow more than 100 repeated commands"), frmconn);
  limitrepeater->setWhatsThis( "<qt>With this option turned on, usage of repeater string "
      "is limited to one hundred repeats. This is here to prevent you from sending "
      "lots of commands accidentally. Note that you can always cancel those "
      "unwanted commands with <b>Edit / Cancel pending commands</b>.</qt>");
  
  startupneg = new QCheckBox (i18n ("Enable telnet &negotiation on startup"), frmconn);
  startupneg->setWhatsThis( i18n ("Enables telnet negotiation for some option. Disabling "
      "it may resolve problems with some servers that do not understand these."));

  // encoding
  QLabel *lblencoding = new QLabel (i18n ("&Encoding:"), frmconn);
  btencoding = new QPushButton (frmconn);
  lblencoding->setBuddy (btencoding);
  btencoding->setWhatsThis( i18n ("Lets you pick the output character encoding used by the server. This usually only needs to be set for non-English servers."));
  codecAction = new KCodecAction (this);
  btencoding->setMenu (codecAction->menu ());
  connect (codecAction, SIGNAL (triggered (const QString &)), this, SLOT (encodingChanged (const QString &)));

  // LPMud prompt handling
  lpmudstyle = new QCheckBox (i18n ("&Auto-append newlines after prompt"), frmconn);
  lpmudstyle->setWhatsThis( i18n ("Enables adding linefeeds after GA-terminated prompts "
             "to have properly aligned output."));
  
  promptlabel = new QCheckBox (i18n ("&Show prompt in a separate field"), frmconn);
  promptlabel->setWhatsThis( "<qt>You will have prompt displayed in a special field "
       "on the left from your primary input line.</qt>");
  
  statusprompt = new QCheckBox (i18n ("Show prompt in the s&tatus bar"), frmconn);
  statusprompt->setWhatsThis( "<qt>Enables prompt display in the status bar.</qt>");
  
  consoleprompt = new QCheckBox (i18n ("Show prompt in the &console"), frmconn);
  consoleprompt->setWhatsThis( "<qt>Enables prompt display in the console.</qt>");
  
  autoadvtranscript = new QCheckBox (i18n ("Start advanced transcript when session starts"), frmconn);
  autoadvtranscript->setWhatsThis( "<qt>Starts advanced logging of the session upon session "
       "startup</qt>");
  
  connlayout->setSpacing (10);
  connlayout->addWidget (useansi, 0, 0, 1, 2);
  connlayout->addWidget (limitrepeater, 1, 0, 1, 2);
  connlayout->addWidget (lblencoding, 2, 0);
  connlayout->addWidget (btencoding, 2, 1);
  connlayout->addWidget (startupneg, 3, 0, 1, 2);
  connlayout->addWidget (lpmudstyle, 4, 0, 1, 2);
  connlayout->addWidget (promptlabel, 5, 0, 1, 2);
  connlayout->addWidget (statusprompt, 6, 0, 1, 2);
  connlayout->addWidget (consoleprompt, 7, 0, 1, 2);
  connlayout->addWidget (autoadvtranscript, 8, 0, 1, 2);
  connlayout->setRowStretch (9, 10);
  connlayout->setColumnStretch (1, 5);

  //page 2
  QGridLayout *commlayout = new QGridLayout (frmcommands);

  QLabel *cl[10];
  QString texts[10] = { i18n ("&North"), i18n ("No&rtheast"), i18n ("&East"),
        i18n ("So&utheast"), i18n ("&South"), i18n ("Sou&thwest"),
        i18n ("&West"), i18n ("Nort&hwest"), i18n ("&Up"), i18n ("&Down")};
  QString mcommandhelp = i18n ("This is a movement command. It is used"
        " by speed-walking and will also be used by the auto-mapper.");
  for (int i = 0; i < 10; i++)
  {
    cl[i] = new QLabel (texts[i], frmcommands);
    edcmd[i] = new KLineEdit ("", frmcommands);
    cl[i]->setBuddy (edcmd[i]);
    edcmd[i]->setWhatsThis( mcommandhelp);
  }

  commlayout->setColumnStretch (1, 10);
  commlayout->setSpacing (5);
  for (int i = 0; i < 10; i++)
  {
    commlayout->addWidget (cl[i], i, 0);
    commlayout->addWidget (edcmd[i], i, 1);
  }
  commlayout->setRowStretch (10, 10);

  //page 3
  QGridLayout *dirslayout = new QGridLayout (frmdirs);

  QLabel *sl1 = new QLabel (i18n ("External &scripts directory"), frmdirs);
  edscriptdir = new KLineEdit (frmdirs);
  sl1->setBuddy (edscriptdir);
  QPushButton *locbutton = new QPushButton (i18n ("Browse..."), frmdirs);
  edscriptdir->setWhatsThis( i18n ("Default directory where we will look for "
      "scripts."));

  QLabel *sl2 = new QLabel (i18n ("&Working directory"), frmdirs);
  edscriptworkdir = new KLineEdit (frmdirs);
  sl2->setBuddy (edscriptworkdir);
  QPushButton *wrkbutton = new QPushButton (i18n ("Browse..."), frmdirs);
  edscriptworkdir->setWhatsThis( i18n ("Default working directory for your "
      "scripts. Making this the same as above is usually a good choice."));

  QLabel *sl3 = new QLabel (i18n ("&Transcripts directory"), frmdirs);
  edtranscriptdir = new KLineEdit (frmdirs);
  sl3->setBuddy (edtranscriptdir);
  QPushButton *trnbutton = new QPushButton (i18n ("Browse..."), frmdirs);
  edtranscriptdir->setWhatsThis( i18n ("Directory where session "
        "transcripts will be saved by default."));

  connect (locbutton, SIGNAL (clicked()), this, SLOT (choosedir1()));
  connect (wrkbutton, SIGNAL (clicked()), this, SLOT (choosedir2()));
  connect (trnbutton, SIGNAL (clicked()), this, SLOT (choosedir3()));
  
  dirslayout->setSpacing (5);
  dirslayout->addWidget (sl1, 0, 0);
  dirslayout->addWidget (edscriptdir, 0, 1);
  dirslayout->addWidget (locbutton, 0, 2);
  dirslayout->addWidget (sl2, 1, 0);
  dirslayout->addWidget (edscriptworkdir, 1, 1);
  dirslayout->addWidget (wrkbutton, 1, 2);
  dirslayout->addWidget (sl3, 2, 0);
  dirslayout->addWidget (edtranscriptdir, 2, 1);
  dirslayout->addWidget (trnbutton, 2, 2);
  dirslayout->setRowStretch (3, 10);

  //page 4
  QVBoxLayout *soundlayout = new QVBoxLayout (frmsound);
  usemsp = new QCheckBox (i18n ("&Enable sounds for this profile"), frmsound);
  usemsp->setWhatsThis( i18n ("Enable playing sounds with this profile.\n"
      "Note that this will only work if you enable sounds in global preferences.\n"
      "Also note that this will only work if your MUD supports <b>MSP</b> "
      "(MUD Sound Protocol)."));
  dirlist = new cDirList (frmsound);
  QLabel *lblmspwarning = new QLabel (i18n ("Only enable these if you need them:"), frmsound);
  alwaysmsp = new QCheckBox (i18n ("&Use MSP even if not negotiated"), frmsound);
  alwaysmsp->setWhatsThis( i18n ("When this option is enabled, KMuddy "
      "will interpret MSP sequences all the time, even if no negotiation "
      "took place. Useful for MUDs that don't support telnet negotiation of "
      "MSP.\nBut beware - only turn this on if your MUD really supports "
      "MSP to prevent other players from sending malicious MSP sequences, "
      "if you allow downloading of sounds."));
  midlinemsp = new QCheckBox (i18n ("Allow &mid-line MSP sequences"), frmsound);
  midlinemsp->setWhatsThis( i18n ("Mid-line MSP sequences are a zMUD's "
      "extension to the MSP spec that some MUDs seem to be using. You can "
      "enable support for this feature here, but ensure that other players "
      "cannot say/tell/whatever these sequences, or disable downloading "
      "support; otherwise players could instruct your client to download "
      "any file on the net..."));
      
  soundlayout->setSpacing (5);
  soundlayout->addWidget (usemsp);
  soundlayout->addWidget (dirlist);
  soundlayout->addWidget (lblmspwarning);
  soundlayout->addWidget (alwaysmsp);
  soundlayout->addWidget (midlinemsp);
#ifdef HAVE_MXP
  QGridLayout *mxplayout = new QGridLayout (frmmxp);
  
  QLabel *lblmxp1 = new QLabel (i18n ("&Use MXP"), frmmxp);
  cbmxpallowed = new QComboBox (frmmxp);
  lblmxp1->setBuddy (cbmxpallowed);
  cbmxpallowed->addItem (i18n ("Never"));
  cbmxpallowed->addItem (i18n ("If negotiated"));
  cbmxpallowed->addItem (i18n ("Auto-detect"));
  cbmxpallowed->addItem (i18n ("Always on"));
  cbmxpallowed->setWhatsThis( i18n ("<qt><p>Here you can choose when will MXP be used.</p>"
      "<ul><li><b>Never</b> MXP tags will never be parsed</li>"
      "<li><b>If negotiated</b> MXP support will depend on successful telnet negotiation.</li>"
      "<li><b>Auto-detect</b> MXP support will be auto-detected.</li>"
      "<li><b>Always on</b> KMuddy will always look for MXP sequences.</li></ul></qt>"));

  QLabel *lblmxp2 = new QLabel (i18n ("MXP &variable prefix"), frmmxp);
  edvarprefix = new KLineEdit (frmmxp);
  lblmxp2->setBuddy (edvarprefix);
  edvarprefix->setWhatsThis( i18n ("Prefix prepended to each MXP variable from the server. "
      "Note that changing the prefix won't affect existing variables with old prefix."));

  mxplayout->setSpacing (5);
  mxplayout->addWidget (lblmxp1, 0, 0);
  mxplayout->addWidget (cbmxpallowed, 0, 1);
  mxplayout->addWidget (lblmxp2, 1, 0);
  mxplayout->addWidget (edvarprefix, 1, 1);
  mxplayout->setRowStretch (3, 10);
#endif  //HAVE_MXP
}

dlgProfileSettings::~dlgProfileSettings ()
{
  cDialogList::self()->removeDialog ("profile-prefs");

}

QSize dlgProfileSettings::sizeHint() const
{
  return QSize (450, 350);
}

bool dlgProfileSettings::useAnsi ()
{
  return useansi->isChecked ();
}

void dlgProfileSettings::setUseAnsi (bool val)
{
  useansi->setChecked (val);
}

bool dlgProfileSettings::limitRepeater ()
{
  return limitrepeater->isChecked ();
}

void dlgProfileSettings::setLimitRepeater (bool val)
{
  limitrepeater->setChecked (val);
}

bool dlgProfileSettings::negotiateOnStartup ()
{
  return startupneg->isChecked();
}

void dlgProfileSettings::setNegotiateOnStartup (bool val)
{
  startupneg->setChecked (val);
}

bool dlgProfileSettings::promptLabel()
{
  return promptlabel->isChecked ();
}

void dlgProfileSettings::setPromptLabel(bool val)
{
  promptlabel->setChecked (val);
}

bool dlgProfileSettings::LPMudStyle()
{
  return lpmudstyle->isChecked();
}

void dlgProfileSettings::setLPMudStyle(bool val)
{
  lpmudstyle->setChecked(val);
}

QString dlgProfileSettings::encoding ()
{
  return codecAction->currentCodecName();
}

void dlgProfileSettings::setEncoding (const QString &encoding)
{
  btencoding->setText (encoding);
  codecAction->setCurrentCodec (encoding);
}


bool dlgProfileSettings::statusPrompt()
{
  return statusprompt->isChecked();
}

void dlgProfileSettings::setStatusPrompt(bool val)
{
  statusprompt->setChecked(val);
}

bool dlgProfileSettings::consolePrompt()
{
  return consoleprompt->isChecked();
}

void dlgProfileSettings::setConsolePrompt(bool val)
{
  consoleprompt->setChecked(val);
}

bool dlgProfileSettings::autoAdvTranscript()
{
  return autoadvtranscript->isChecked();
}

void dlgProfileSettings::setAutoAdvTranscript(bool val)
{
  autoadvtranscript->setChecked(val);
}
QString dlgProfileSettings::movementCmd (int which)
{
  if ((which >= 0) && (which <= 9))
    return edcmd[which]->text ();
  else
    return "";
}

void dlgProfileSettings::setMovementCmd (int which, QString val)
{
  if ((which >= 0) && (which <= 9))
    edcmd[which]->setText (val);
}

QString dlgProfileSettings::scriptDir ()
{
  return edscriptdir->text ();
}

void dlgProfileSettings::setScriptDir (const QString &s)
{
  edscriptdir->setText (s);
}

QString dlgProfileSettings::scriptWorkDir ()
{
  return edscriptworkdir->text ();
}

void dlgProfileSettings::setTranscriptDir (const QString &s)
{
  edtranscriptdir->setText (s);
}

QString dlgProfileSettings::transcriptDir ()
{
  return edtranscriptdir->text ();
}

void dlgProfileSettings::setScriptWorkDir (const QString &s)
{
  edscriptworkdir->setText (s);
}

void dlgProfileSettings::choosedir1 ()
{
  edscriptdir->setText (QFileDialog::getExistingDirectory (this, i18n ("Choose default script location"), edscriptdir->text()));
}

void dlgProfileSettings::choosedir2 ()
{
  edscriptworkdir->setText (QFileDialog::getExistingDirectory (this, i18n ("Choose default script working directory"), edscriptworkdir->text()));
}

void dlgProfileSettings::choosedir3 ()
{
  edtranscriptdir->setText (QFileDialog::getExistingDirectory (this, i18n ("Choose default transcript directory"), edtranscriptdir->text()));
}

void dlgProfileSettings::encodingChanged (const QString &enc)
{
  btencoding->setText (enc);
}

bool dlgProfileSettings::useMSP ()
{
  return usemsp->isChecked ();
}

void dlgProfileSettings::setUseMSP (bool val)
{
  usemsp->setChecked (val);
}

bool dlgProfileSettings::alwaysMSP ()
{
  return alwaysmsp->isChecked ();
}

void dlgProfileSettings::setAlwaysMSP (bool val)
{
  alwaysmsp->setChecked (val);
}

bool dlgProfileSettings::midlineMSP ()
{
  return midlinemsp->isChecked ();
}

void dlgProfileSettings::setMidlineMSP (bool val)
{
  midlinemsp->setChecked (val);
}

const QStringList &dlgProfileSettings::soundDirList ()
{
  return dirlist->getDirList ();
}

void dlgProfileSettings::setSoundDirList (const QStringList &list)
{
  dirlist->setDirList (list);
}

#ifdef HAVE_MXP

int dlgProfileSettings::MXPAllowed ()
{
  return cbmxpallowed->currentIndex() + 1;
}

void dlgProfileSettings::setMXPAllowed (int allow)
{
  cbmxpallowed->setCurrentIndex (allow - 1);
}

void dlgProfileSettings::setVariablePrefix (const QString &prefix)
{
  edvarprefix->setText (prefix);
}

QString dlgProfileSettings::variablePrefix ()
{
  return edvarprefix->text ();
}

#endif  //HAVE_MXP

