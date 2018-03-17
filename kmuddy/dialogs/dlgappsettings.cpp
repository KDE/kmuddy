/***************************************************************************
                          dlgappsettings.cpp  -  application settings
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : St aug 14 2002
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

#include "dlgappsettings.h"
#include "cactionmanager.h"
#include "cglobalsettings.h"
// needed for auto-connect
#include "cprofilemanager.h"
#include "cdialoglist.h"
#include "cdirlist.h"

//a lot of include files...
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <kapplication.h>
#include <kcolorbutton.h>
#include <kconfig.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <kshortcutsdialog.h>
#include <klineedit.h>
#include <klocale.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

dlgAppSettings::dlgAppSettings (QWidget *parent) : KPageDialog (parent)
{
  cDialogList::self()->addDialog ("app-prefs", this);

  //color list
  QString cltext[16] = { i18n("Black"), i18n("Red"), i18n("Green"),
    i18n("Yellow"), i18n("Blue"), i18n("Magenta"), i18n("Cyan"), i18n("Gray"),
    i18n("Dark gray"), i18n("Bright red"), i18n("Bright green"),
    i18n("Bright yellow"), i18n("Bright blue"), i18n("Bright magenta"),
    i18n("Bright cyan"), i18n("White") };

  //initial dialog size
  setWindowTitle (i18n ("Application settings"));
  setStandardButtons (QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
  setFaceType (KPageDialog::List);

  //prepare dialog pages
  KPageWidgetItem *item;
  QFrame *frmwindow = new QFrame (this);
  item = addPage (frmwindow, i18n ("Window"));
  item->setIcon (KIcon ("window-new"));
  QFrame *frmoutputarea = new QFrame (this);
  item = addPage (frmoutputarea, i18n ("Output area"));
  item->setIcon (KIcon ("format-justify-left"));
  QFrame *frmfont = new QFrame (this);
  item = addPage (frmfont, i18n ("Font"));
  item->setIcon (KIcon ("preferences-desktop-font"));
  QFrame *frminput = new QFrame (this);
  item = addPage (frminput, i18n ("Input line"));
  item->setIcon (KIcon ("edit-clear"));
  QFrame *frmcolors = new QFrame (this);
  item = addPage (frmcolors, i18n ("Colors"));
  item->setIcon (KIcon ("color-picker"));
  QFrame *frmchars = new QFrame (this);
  item = addPage (frmchars, i18n ("Strings"));
  item->setIcon (KIcon ("format-indent-less"));
  QFrame *frmsound = new QFrame (this);
  item = addPage (frmsound, i18n ("Sounds"));
  item->setIcon (KIcon ("media-playback-start"));
  QFrame *frmshortcuts = new QFrame (this);
  item = addPage (frmshortcuts, i18n ("Shortcuts"));
  item->setIcon (KIcon ("configure-shortcuts"));

  //page 1
  QGridLayout *winlayout = new QGridLayout (frmwindow);
   //always tabbar
  chkalwaystabbar = new QCheckBox (i18n ("S&how the tab bar even if not needed"), frmwindow);
  chkalwaystabbar->setWhatsThis( i18n ("If this is checked, the tab bar will "
      "always be shown. Otherwise, it will be hidden if it contains only one tab."));
   //auxiliary input
  chkauxinput = new QCheckBox (i18n ("Show au&xiliary input line"), frmwindow);
  chkauxinput->setWhatsThis( i18n ("You will have two input lines instead of one."));
   //always notify
  chkalwaysnotify = new QCheckBox (i18n ("&Always notify"), frmwindow);
  chkalwaysnotify->setWhatsThis( i18n ("If this is checked, output notification "
      "will be used whenever some text comes from the server. Otherwise, it will "
      "only be used when a notification trigger is activated. Note that this "
      "option only takes effect if global or local notification is enabled."));
   //global notify
  chkglobalnotify = new QCheckBox (i18n ("&Global notification"), frmwindow);
  chkglobalnotify->setWhatsThis( i18n ("When some text comes from the server "
      "and KMuddy is not currently active, window caption and the taskbar will "
      "try to attract your attention (some characters will blink there)."));
   //local notify
  chklocalnotify = new QCheckBox (i18n ("&Local notification"), frmwindow);
  chklocalnotify->setWhatsThis( i18n ("When some text comes from the server "
      "and this session is not currently active, its tab will try to attract "
      "your attention (some characters will blink there)."));
  
  //system tray
  chksystray = new QCheckBox (i18n ("Display system tray icon"), frmwindow);
  chksystray->setWhatsThis( i18n ("This option toggles the presence of the "
      "system tray icon. The icon can be used to hide the application window "
      "without taking up space in the taskbar. You can click on the icon "
      "to hide the window and then click again to restore it. "
      "Your connection will remain active even if the window is hidden "
      "and you will still receive notification messages. "));

  //passive popups
  chkpassivepopup = new QCheckBox (i18n ("Use passive popups"), frmwindow);
  chkpassivepopup->setWhatsThis( i18n ("This option toggles the use of "
      "passive popups. Passive popups appear near the system tray icon when you "
      "are in another window and alert you to the presence of activity in one "
      "of your connections. "));
  
  //auto-connect
  chkautoconnect = new QCheckBox (i18n ("Auto-connect on startup"), frmwindow);
  chkautoconnect->setWhatsThis( i18n ("With this option set, the selected "
      "connection will be automatically initiated immediately after starting "
      "KMuddy. Handy when you connect using one profile most of the time."));
  QLabel *lblac = new QLabel (i18n ("Auto-connect to:"), frmwindow);
  autoconnect = new QComboBox (frmwindow);
  lblac->setBuddy (autoconnect);
  autoconnect->setModel (cProfileManager::self()->model());
  connect (chkautoconnect, SIGNAL (toggled (bool)), lblac, SLOT (setEnabled (bool)));
  connect (chkautoconnect, SIGNAL (toggled (bool)), autoconnect, SLOT (setEnabled (bool)));
  
  // initial state
  chkautoconnect->setChecked (false);
  lblac->setEnabled (false);
  autoconnect->setEnabled (false);
  chksystray->setChecked (false);

   //add widgets to the layout
  winlayout->setSpacing (10);
  winlayout->addWidget (chkalwaystabbar, 0, 0, 1, 2);
  winlayout->addWidget (chkauxinput, 1, 0, 1, 2);
  winlayout->addWidget (chkalwaysnotify, 2, 0, 1, 2);
  winlayout->addWidget (chkglobalnotify, 3, 0, 1, 2);
  winlayout->addWidget (chklocalnotify, 4, 0, 1, 2);
  winlayout->addWidget (chksystray, 5, 0, 1, 2);
  winlayout->addWidget (chkpassivepopup, 6, 0, 1, 2);
  winlayout->addWidget (chkautoconnect, 7, 0, 1, 2);
  winlayout->addWidget (lblac, 8, 0);
  winlayout->addWidget (autoconnect, 8, 1);
  winlayout->setRowStretch (9, 10);

  //page 2
  QVBoxLayout *outlayout = new QVBoxLayout (frmoutputarea);
   //colors
  QWidget *wcolors = new QWidget (frmoutputarea);
  QGridLayout *wincolorlayout = new QGridLayout (wcolors);
  QStringList colorlist;
  for (int i = 0; i < 16; i++)
    colorlist.append (cltext[i]);
  QLabel *wcolorlabel[5];
    //bg color
  wcolorlabel[0] = new QLabel (i18n ("Default &background color"), wcolors);
  combo[0] = new QComboBox (wcolors);
  combo[0]->addItems (colorlist);
  combo[0]->setWhatsThis( i18n ("This color will be used as a standard background color."));
    //fg color
  wcolorlabel[1] = new QLabel (i18n ("Default &text color"), wcolors);
  combo[1] = new QComboBox (wcolors);
  combo[1]->addItems (colorlist);
  combo[1]->setWhatsThis( i18n ("This color will be used to display standard text with no defined color."));
    //echo color
  wcolorlabel[2] = new QLabel (i18n ("Command &echo color"), wcolors);
  combo[2] = new QComboBox (wcolors);
  combo[2]->addItems (colorlist);
  combo[2]->setWhatsThis( i18n ("This color will be used to display commands that you have entered."));
    //system color
  wcolorlabel[3] = new QLabel (i18n ("&System message color"), wcolors);
  combo[3] = new QComboBox (wcolors);
  combo[3]->addItems (colorlist);
  combo[3]->setWhatsThis( i18n ("This color will be used for various system messages."));
    //insert'em!
  wincolorlayout->setSpacing (10);
  for (int i = 0; i < 4; i++)
  {
    wcolorlabel[i]->setBuddy (combo[i]);
    wincolorlayout->addWidget (wcolorlabel[i], i, 0);
    wincolorlayout->addWidget (combo[i], i, 1);
  }
  wincolorlayout->setRowStretch (4, 10);
  wincolorlayout->setColumnStretch (2, 3);

  KHBox *outputs = new KHBox (frmoutputarea);
  //output1 and output2 come in reversed order, because I'm too lazy to rewrite all the stuff ;)
  KVBox *output2 = new KVBox (outputs);
  KVBox *output1 = new KVBox (outputs);
  outputs->setSpacing (10);
  output1->setSpacing (10);
  output2->setSpacing (10);

   //cmd echo and system messages
  chkcmdecho = new QCheckBox (i18n ("Enable co&mmand echo"), output1);
  chkcmdecho->setWhatsThis( i18n ("Enables displaying of commands in the "
      "console.<p><b>Please note:</b> This will also disable command echo "
      "in session transcript."));
  chkmessages = new QCheckBox (i18n ("Enable s&ystem messages"), output1);
  chkmessages->setWhatsThis( i18n ("Enables displaying of system messages "
      "in the console."));

    //blinking
  chkblinking = new QCheckBox (i18n ("Enable b&linking"), output1);
  chkblinking->setWhatsThis( i18n ("Enables support for blinking. "));
  
   //indentation
  edindent = new KIntNumInput (output2);
  edindent->setLabel (i18n ("&Indentation"),
      Qt::AlignLeft | Qt::AlignVCenter);
  edindent->setMinimum (0);
  edindent->setMaximum (10);
  edindent->setSpecialValueText (i18n ("None"));
  //edindent shouldn't be too wide
  edindent->setMaximumSize (edindent->minimumSizeHint ());
  edindent->setWhatsThis( i18n ("When a line is too long, part of it will "
      "be moved to the next line. Indentation says how many spaces will be "
      "put to the beginning of the continued line."));

   //wrap at
  edwrappos = new KIntNumInput (output2);
  edwrappos->setLabel (i18n ("W&rap at position"),
      Qt::AlignLeft | Qt::AlignVCenter);
  edwrappos->setMinimum (0);
  edwrappos->setMaximum (250);
  edwrappos->setSpecialValueText (i18n ("As needed"));
  //edwrappos shouldn't be too wide
  edwrappos->setMaximumSize (edwrappos->minimumSizeHint ());
  edwrappos->setWhatsThis( i18n ("Wrap at a fixed position, or at the rightmost position, "
      "if no value is given."));

   //history size
  edhistory = new KIntNumInput (output2);
  edhistory->setLabel (i18n ("&History buffer size"),
      Qt::AlignLeft | Qt::AlignVCenter);
  edhistory->setRange (100, 10000, 100);
  edhistory->setSliderEnabled (false);
  edhistory->setSuffix (" " + i18n ("lines"));
  //edhistory shouldn't be too wide too
  edhistory->setMaximumSize (edhistory->minimumSizeHint ());
  edhistory->setWhatsThis( i18n ("Size of output scroll-back buffer. "
      "Please note that this setting will only affect new connections, "
      "existing ones will keep their history setting."));

   //force redraw
  edforceredraw = new KIntNumInput (output2);
  edforceredraw->setLabel (i18n ("&Forced redraw after"),
      Qt::AlignLeft | Qt::AlignVCenter);
  edforceredraw->setRange (0, 20, 1);
  edforceredraw->setSliderEnabled (false);
  edforceredraw->setSuffix (" " + i18n ("lines"));
  edforceredraw->setSpecialValueText (i18n ("Never"));
  //edhistory shouldn't be too wide too
  edforceredraw->setMaximumSize (edforceredraw->minimumSizeHint ());
  edforceredraw->setWhatsThis( i18n ("Forced redraw after adding a given amount "
    "of lines. Setting this to a low number may prevent some displaying problems, at a cost "
    "of higher CPU usage. Setting it to Never will disable the feature, redraw will "
    "only occur when needed."));

   //add widgets to main layout
  outlayout->setSpacing (10);
  outlayout->addWidget (wcolors);
  outlayout->addWidget (outputs);
  outlayout->addStretch (1);

  //page 3
  QGridLayout *fontlayout = new QGridLayout (frmfont);
   //row 1
  QLabel *fl1 = new QLabel (i18n ("&Server output"), frmfont);
  fonted1 = new QLabel (frmfont);
  fonted1->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  KPushButton *bfont1 = new KPushButton (i18n ("Choose..."), frmfont);
  fl1->setBuddy (bfont1);
  connect (bfont1, SIGNAL (clicked()), this, SLOT (fontchooser1 ()));
  QString serveroutput = i18n ("Here you can select a font that will be used for main display.");
  fonted1->setWhatsThis( serveroutput);
  bfont1->setWhatsThis( serveroutput);
   //row 2  
  QLabel *fl2 = new QLabel (i18n ("&Input line"), frmfont);
  fonted2 = new QLabel (frmfont);
  fonted2->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  KPushButton *bfont2 = new KPushButton (i18n ("Choose..."), frmfont);
  fl2->setBuddy (bfont2);
  connect (bfont2, SIGNAL (clicked()), this, SLOT (fontchooser2 ()));
  QString inputline = i18n ("Here you can select a font that will be used for the input line.");
  fonted2->setWhatsThis( inputline);
  bfont2->setWhatsThis( inputline);
   //row 3
  QLabel *fl3 = new QLabel (i18n ("&Multi-line input"), frmfont);
  fonted3 = new QLabel (frmfont);
  fonted3->setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);
  KPushButton *bfont3 = new KPushButton (i18n ("Choose..."), frmfont);
  fl3->setBuddy (bfont3);
  connect (bfont3, SIGNAL (clicked()), this, SLOT (fontchooser3 ()));
  QString minputline = i18n ("Here you can select a font that will be used for the multi-line input window.");
  fonted3->setWhatsThis( minputline);
  bfont3->setWhatsThis( minputline);
   //place widgets
  fontlayout->setSpacing (10);
  fontlayout->setColumnStretch (1, 10);
  fontlayout->addWidget (fl1, 0, 0);
  fontlayout->addWidget (fl2, 1, 0);
  fontlayout->addWidget (fl3, 2, 0);
  fontlayout->addWidget (fonted1, 0, 1);
  fontlayout->addWidget (fonted2, 1, 1);
  fontlayout->addWidget (fonted3, 2, 1);
  fontlayout->addWidget (bfont1, 0, 2);
  fontlayout->addWidget (bfont2, 1, 2);
  fontlayout->addWidget (bfont3, 2, 2);
  fontlayout->setRowStretch (3, 10);

  //page 4
  QVBoxLayout *inputlayout = new QVBoxLayout (frminput);
  chkkeeptext = new QCheckBox (i18n ("&Keep sent text"), frminput);
  chkkeeptext->setWhatsThis( i18n ("If you enable this option, commands "
        "will remain in the input line after you send them (by pressing "
        "ENTER); otherwise it will be cleared."));
  chkselectkepttext = new QCheckBox (i18n ("&Select kept text after it's sent"), frminput);
  chkselectkepttext->setWhatsThis( i18n ("If you enable this option, entered "
        "command will be selected. This allows you to immediately type another "
        "command without deleting the old one and still be able to reuse previous "
        "command easily."));
  connect (chkkeeptext, SIGNAL (toggled (bool)), chkselectkepttext, SLOT (setEnabled (bool)));
  chkarrowshistory = new QCheckBox (i18n ("&Cursor keys browse history"), frminput);
  chkarrowshistory->setWhatsThis( i18n ("If you enable this option, you "
        "will be able to choose previously entered commands with Up and Down keys."));
  chktelnetpaste = new QCheckBox (i18n ("Telnet-style &paste"), frminput);
  chktelnetpaste->setWhatsThis( i18n ("Enables telnet-style pasting, i.e. "
      "if multi-line text is pasted, all lines but the last are sent as "
      "commands immediately."));
  chktrimspaces = new QCheckBox (i18n ("T&rim spaces"), frminput);
  chktrimspaces->setWhatsThis( i18n ("Enables trimming of spaces from "
      "commands. Note that with this option disabled, you should avoid "
      "including spaces when typing multiple commands on one line, or you "
      "will encounter problems with special sequences not being detected "
      "properly."));
  chkautocompletion = new QCheckBox (i18n ("Enable &auto-completion"), frminput);
  QString autocompldesc = i18n ("Auto-completion is a feature that offers "
      "you possible completions for commands, as you type them. It can also "
      "finish the command for you.");
  chkautocompletion->setWhatsThis( autocompldesc);
   //combos widget
  QWidget *inputcombos = new QWidget (frminput);
  QGridLayout *inputcombolayout = new QGridLayout (inputcombos);
    //AC type
  QLabel *inputlabel1 = new QLabel (i18n ("T&ype of auto-completion"), inputcombos);
  comboactype = new QComboBox (inputcombos);
  comboactype->addItem (i18n ("Auto-fill"));
  comboactype->addItem (i18n ("Shortest-match auto-fill"));
  comboactype->addItem (i18n ("Popup list-box"));
  comboactype->setWhatsThis( autocompldesc);
  connect (chkautocompletion, SIGNAL (toggled (bool)), comboactype, SLOT (setEnabled (bool)));
    //bg color
  QLabel *inputlabel2 = new QLabel (i18n ("&Background color"), inputcombos);
  comboinputbkcolor = new QComboBox (inputcombos);
  comboinputbkcolor->addItems (colorlist);
  comboinputbkcolor->setWhatsThis( i18n ("Background color of the input line"));
    //fg color
  QLabel *inputlabel3 = new QLabel (i18n ("&Text color"), inputcombos);
  comboinputcolor = new QComboBox (inputcombos);
  comboinputcolor->addItems (colorlist);
  comboinputcolor->setWhatsThis( i18n ("Color of text that you type in the input line."));
    //set buddies...
  inputlabel1->setBuddy (comboactype);
  inputlabel2->setBuddy (comboinputbkcolor);
  inputlabel3->setBuddy (comboinputcolor);
    //place widgets...
  inputcombolayout->addWidget (inputlabel1, 0, 0);
  inputcombolayout->addWidget (inputlabel2, 1, 0);
  inputcombolayout->addWidget (inputlabel3, 2, 0);
  inputcombolayout->addWidget (comboactype, 0, 1);
  inputcombolayout->addWidget (comboinputbkcolor, 1, 1);
  inputcombolayout->addWidget (comboinputcolor, 2, 1);
  inputcombolayout->setSpacing (10);
  inputcombolayout->setRowStretch (3, 10);
  inputcombolayout->setColumnStretch (2, 3);
    //swap ENTERS
  chkswapenters = new QCheckBox (i18n ("S&wap ENTER and CTRL+ENTER for multi-line"), frminput);
  chkswapenters->setWhatsThis( i18n ("When turned off, CTRL+ENTER will insert another "
      "line in the multi-line input line, and ENTER will send commands to the server. "
      "If turned on, then the behaviour of these keys gets swapped."));
   //place widgets on the page
  inputlayout->addWidget (chkkeeptext);
  inputlayout->addWidget (chkselectkepttext);
  inputlayout->addWidget (chkarrowshistory);
  inputlayout->addWidget (chktelnetpaste);
  inputlayout->addWidget (chktrimspaces);
  inputlayout->addWidget (chkautocompletion);
  inputlayout->addWidget (inputcombos);
  inputlayout->addWidget (chkswapenters);
  inputlayout->setSpacing (10);
  inputlayout->addStretch (10);

  //page 5
  QHBoxLayout *colorlayout = new QHBoxLayout (frmcolors);
  colorlayout->setSpacing (10);
  QGroupBox *colorsbox1 = new QGroupBox (i18n ("Dark colors"), frmcolors);
  QGroupBox *colorsbox2 = new QGroupBox (i18n ("Bright colors"), frmcolors);
  QVBoxLayout *colors1layout = new QVBoxLayout (colorsbox1);
  QVBoxLayout *colors2layout = new QVBoxLayout (colorsbox2);
  for (int i = 0; i < 8; i++)
  {
    KHBox *colorboxn = new KHBox (colorsbox1);
    QLabel *l = new QLabel (cltext[i], colorboxn);
    cbutton[i] = new KColorButton (colorboxn);
    colorboxn->setStretchFactor (l, 2);
    colorboxn->setStretchFactor (cbutton[i], 1);
    colors1layout->addWidget (colorboxn);
  }
  for (int i = 8; i < 16; i++)
  {
    KHBox *colorboxn = new KHBox (colorsbox2);
    QLabel *l = new QLabel (cltext[i], colorboxn);
    cbutton[i] = new KColorButton (colorboxn);
    colorboxn->setStretchFactor (l, 1);
    colorboxn->setStretchFactor (cbutton[i], 0);
    colors2layout->addWidget (colorboxn);
  }
  colorlayout->addWidget (colorsbox1);
  colorlayout->addWidget (colorsbox2);
  
  //page 6
  QGridLayout *charlayout = new QGridLayout (frmchars);
   //row 1
  QLabel *chl1 = new QLabel (i18n ("&Command separator string"), frmchars);
  edcmdseparator = new KLineEdit (frmchars);
  chl1->setBuddy (edcmdseparator);
  edcmdseparator->setWhatsThis( i18n ("You can enter multiple commands at "
        "once, they will get executed together. Here you can enter string "
        "that can be used to separate individual commands."));
   //row 2  
  QLabel *chl2 = new QLabel (i18n ("&Speed-walk string"), frmchars);
  edspeedwalk = new KLineEdit (frmchars);
  chl2->setBuddy (edspeedwalk);
  edspeedwalk->setWhatsThis( i18n ("If you start a command with this string, "
        "the command will be interpreted as speed-walking. Every character "
        "is treated as one command: n, s, w, e, j(=ne), k(=se), l(=sw), m(=nw), u, d."
        "<p>Commands that will be sent for each direction can be set up in "
        "<i>MUD preferences</i> (<b>profile-based</b> connections only)."));
   //row 3
  chkemptywalk = new QCheckBox (i18n ("S&peed-walk even if empty"), frmchars);
  chkemptywalk->setWhatsThis( i18n ("Enabling this will cause KMuddy to "
        "expand speed-walk strings even if no speed-walk string has been "
        "given. This allows faster speed-walking, but keep in mind that "
        "EVERY command that consists only of valid speed-walk characters "
        "(news, for example) will be treated as speed-walking. To avoid "
        "this, you'll have to type at least one character in upper-case."));
   //row 4
  QLabel *chl3 = new QLabel (i18n ("Macro-ca&ll string"), frmchars);
  edscript = new KLineEdit (frmchars);
  chl3->setBuddy (edscript);
  edscript->setWhatsThis( i18n ("If you start a command with this string, "
        "the command will be interpreted as a macro-call.<br>"
        "Syntax is &lt;this string&gt; &lt;command&gt; "
        "[parameters]  (/exec calls external scripts)"));
   //row 5
  QLabel *chl4 = new QLabel (i18n ("&Repeater string"), frmchars);
  edmulticmd = new KLineEdit (frmchars);
  chl4->setBuddy (edmulticmd);
  edmulticmd->setWhatsThis( i18n ("Allows you to repeat some command. For "
        "example, typing <i>#10 say hello</i> sends that command 10 times. "
        "<br>Note that this also works with aliases and macro execution.<br>"
        "Repeater and number may be separated by a space (spaces)."));

    //row 6
  QLabel *chl5 = new QLabel (i18n ("&Focus string"), frmchars);
  edfocus = new KLineEdit (frmchars);
  chl5->setBuddy (edfocus);
  edfocus->setWhatsThis( i18n("If a command is started with this string, "
        "the command will be interpreted as a command-focus-call. This will "
        "allow you to send commands to other connected characters.<br>"
        "Syntax is &lt;this string&gt; &lt;focus tab&gt; &lt;this string&gt; &lt;command&gt;" ));

    //row 7
  QLabel *chl6 = new QLabel (i18n ("Send &as-is"), frmchars);
  ednoparse = new KLineEdit (frmchars);
  chl6->setBuddy (ednoparse);
  ednoparse->setWhatsThis( i18n("If a command is started with this string, "
        "it will be sent as-is, without any parsing. This string will be removed first." ));

   //row 8
  chkexpandBS = new QCheckBox (i18n ("&Expand backslashes"), frmchars);
  chkexpandBS->setWhatsThis( i18n ("This will enable backslash expansion, "
      "that is: \\n is end-of-line, \\t is TAB, \\\\ is \\, \\<anything else> "
      "means <anything else>."));

   //place widgets
  charlayout->setSpacing (10);
  charlayout->setColumnStretch (2, 10);
  charlayout->addWidget (chl1, 0, 0);
  charlayout->addWidget (chl2, 1, 0);
  charlayout->addWidget (chl3, 3, 0);
  charlayout->addWidget (chl4, 4, 0);
  charlayout->addWidget (chl5, 5, 0);
  charlayout->addWidget (chl6, 6, 0);
  charlayout->addWidget (edcmdseparator, 0, 1);
  charlayout->addWidget (edspeedwalk, 1, 1);
  charlayout->addWidget (edscript, 3, 1);
  charlayout->addWidget (edmulticmd, 4, 1);
  charlayout->addWidget (edfocus, 5, 1);
  charlayout->addWidget (ednoparse, 6, 1);
  charlayout->addWidget (chkemptywalk, 2, 0, 1, 2);
  charlayout->addWidget (chkexpandBS, 7, 0, 1, 2);

  //page 7
  QVBoxLayout *soundlayout = new QVBoxLayout (frmsound);
  chkallowmsp = new QCheckBox (i18n ("&Enable sounds globally"), frmsound);
  chkallowmsp->setWhatsThis( i18n ("<qt>Enable playing sounds for all connections.\n"
      "Note that you can override this setting in MUD Preferences dialog.\n"
      "Also note that this will only work if your MUD supports <b>MSP</b> "
      "(MUD Sound Protocol).</qt>"));
  chkallowdownloads = new QCheckBox (i18n ("Allow &downloading sounds"), frmsound);
  QString localdir = KStandardDirs::locateLocal ("appdata", "sounds/");
  chkallowdownloads->setWhatsThis( i18n ("MSP version 0.3 supports "
      "downloading of sound files directly from the web or FTP. You can "
      "enable this functionality here. Note that every sound will only be "
      "downloaded once. The downloaded files are stored in your homedir in "
      "directory\n") + QString (localdir));
  dirlist = new cDirList (frmsound);

  soundlayout->setSpacing (5);
  soundlayout->addWidget (chkallowmsp);
  soundlayout->addWidget (chkallowdownloads);
  soundlayout->addWidget (dirlist);

  //page 8
   //layout ensures that the widget fills entire available space...
  QHBoxLayout *keylayout = new QHBoxLayout (frmshortcuts);
  keys = new KShortcutsEditor (cActionManager::self()->getACol (), frmshortcuts);
  keylayout->addWidget (keys);

  cActionManager::self()->invokeEvent ("dialog-create", 0, "app-prefs");

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (this, SIGNAL (okClicked()), this, SLOT (getSettingsFromDialog()));
  connect (this, SIGNAL (applyClicked()), this, SLOT (getSettingsFromDialog()));
}

dlgAppSettings::~dlgAppSettings()
{
  delete keys;
  cDialogList::self()->removeDialog ("app-prefs");
}

QSize dlgConnect::sizeHint() const
{
  return QSize (600, 400);
}

void dlgAppSettings::showSettingsDialog ()
{
  //so first we have to create the dialog...
  // TODO: make it a parent of the main window - this could fail ...
  dlgAppSettings *sdlg = new dlgAppSettings (KApplication::kApplication()->activeWindow());

  //next we fill in its data
  sdlg->putSettingsToDialog ();

  //dialog is ready - show it!
  sdlg->exec ();

  //further action is handled via slots issued by buttons, so that we only
  //have to destroy the dialog...
  delete sdlg;
}

int dlgAppSettings::getcolorindex (int which)
{
  if ((which >= 0) && (which <= 3))
    return combo[which]->currentIndex ();
  return 0;
}

void dlgAppSettings::setcolorindex (int which, int value)
{
  if ((which >= 0) && (which <= 3) && (value >= 0) && (value <= 15))
    combo[which]->setCurrentIndex (value);
}

bool dlgAppSettings::cmdEcho ()
{
  return chkcmdecho->isChecked ();
}

bool dlgAppSettings::messages ()
{
  return chkmessages->isChecked ();
}

void dlgAppSettings::setCmdEcho (bool value)
{
  chkcmdecho->setChecked (value);
}

void dlgAppSettings::setMessages (bool value)
{
  chkmessages->setChecked (value);
}

void dlgAppSettings::setIndentation (int value)
{
  edindent->setValue (value);
}

int dlgAppSettings::indentation ()
{
  return edindent->value ();
}

void dlgAppSettings::setHistory (int value)
{
  edhistory->setValue (value);
}

int dlgAppSettings::history ()
{
  return edhistory->value ();
}

int dlgAppSettings::forceRedraw ()
{
  return edforceredraw->value ();
}

void dlgAppSettings::setForceRedraw (int value)
{
  edforceredraw->setValue (value);
}

bool dlgAppSettings::enableBlinking ()
{
  return chkblinking->isChecked ();
}

void dlgAppSettings::setEnableBlinking (bool value)
{
  chkblinking->setChecked (value);
}

bool dlgAppSettings::alwaysTabBar ()
{
  return chkalwaystabbar->isChecked ();
}

void dlgAppSettings::setAlwaysTabBar (bool value)
{
  chkalwaystabbar->setChecked (value);
}

bool dlgAppSettings::auxInput ()
{
  return chkauxinput->isChecked ();
}

void dlgAppSettings::setAuxInput (bool val)
{
  chkauxinput->setChecked (val);
}

bool dlgAppSettings::alwaysNotify ()
{
  return chkalwaysnotify->isChecked ();
}

void dlgAppSettings::setAlwaysNotify (bool value)
{
  chkalwaysnotify->setChecked (value);
}

bool dlgAppSettings::localNotify ()
{
  return chklocalnotify->isChecked ();
}

void dlgAppSettings::setLocalNotify (bool value)
{
  chklocalnotify->setChecked (value);
}

bool dlgAppSettings::globalNotify ()
{
  return chkglobalnotify->isChecked ();
}

void dlgAppSettings::setGlobalNotify (bool value)
{
  chkglobalnotify->setChecked (value);
}

bool dlgAppSettings::sysTrayEnabled ()
{
  return chksystray->isChecked ();
}

bool dlgAppSettings::passivePopup ()
{
  return chkpassivepopup->isChecked();
}

void dlgAppSettings::setSysTrayEnabled (bool value){
  chksystray->setChecked (value);
}

void dlgAppSettings::setPassivePopup (bool value){
  chkpassivepopup->setChecked (value);
}

QString dlgAppSettings::autoConnect ()
{
  if (!chkautoconnect->isChecked())  //no auto-connect
    return QString();
  return cProfileManager::self()->profileList()[autoconnect->currentIndex ()];
}

void dlgAppSettings::setAutoConnect (const QString &ac)
{
  chkautoconnect->setChecked (false);
  if (ac.isEmpty()) return;
  int pos = cProfileManager::self()->profileList().indexOf (ac);
  if (pos == -1) return;  // no such profile
  chkautoconnect->setChecked (true);
  autoconnect->setCurrentIndex (pos);
}

void dlgAppSettings::fontchooser1 ()
{
  if (KFontDialog::getFont (font[0], KFontChooser::FixedFontsOnly, this))
    fonted1->setText (font[0].family() + ", " +
          QString::number(font[0].pointSize()) +
          (font[0].bold() ? (" " + i18n("Bold")) : (QString) "") +
          (font[0].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

void dlgAppSettings::fontchooser2 ()
{
  if (KFontDialog::getFont (font[1], KFontChooser::NoDisplayFlags, this))
    fonted2->setText (font[1].family() + ", " +
          QString::number(font[1].pointSize()) +
          (font[1].bold() ? (" " + i18n("Bold")) : (QString) "") +
          (font[1].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

void dlgAppSettings::fontchooser3 ()
{
  if (KFontDialog::getFont (font[2], KFontChooser::NoDisplayFlags, this))
    fonted3->setText (font[2].family() + ", " +
          QString::number(font[2].pointSize()) +
          (font[2].bold() ? (" " + i18n("Bold")) : (QString) "") +
          (font[2].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

void dlgAppSettings::setFont1 (QFont f)
{
  font[0] = f;
  fonted1->setText (font[0].family() + ", " +
        QString::number(font[0].pointSize()) +
        (font[0].bold() ? (" " + i18n("Bold")) : (QString) "") +
        (font[0].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

void dlgAppSettings::setFont2 (QFont f)
{
  font[1] = f;
  fonted2->setText (font[1].family() + ", " +
        QString::number(font[1].pointSize()) +
        (font[1].bold() ? (" " + i18n("Bold")) : (QString) "") +
        (font[1].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

void dlgAppSettings::setFont3 (QFont f)
{
  font[2] = f;
  fonted3->setText (font[2].family() + ", " +
        QString::number(font[2].pointSize()) +
        (font[2].bold() ? (" " + i18n("Bold")) : (QString) "") +
        (font[2].italic() ? (" " + i18n("Italic")) : (QString) ""));
}

QFont dlgAppSettings::font1 ()
{
  return font[0];
}

QFont dlgAppSettings::font2 ()
{
  return font[1];
}

QFont dlgAppSettings::font3 ()
{
  return font[2];
}

bool dlgAppSettings::keeptext ()
{
  return chkkeeptext->isChecked ();
}

bool dlgAppSettings::selectkepttext ()
{
  return chkselectkepttext->isChecked ();
}

bool dlgAppSettings::arrowsHistory ()
{
  return chkarrowshistory->isChecked ();
}

bool dlgAppSettings::autocompletion ()
{
  return chkautocompletion->isChecked ();
}

void dlgAppSettings::setKeepText (bool value)
{
  chkkeeptext->setChecked (value);
  chkselectkepttext->setEnabled (value);
}

void dlgAppSettings::setSelectKeptText (bool value)
{
  chkselectkepttext->setChecked (value);
}

void dlgAppSettings::setArrowsHistory (bool value)
{
  chkarrowshistory->setChecked (value);
}

void dlgAppSettings::setAutoCompletion (bool value)
{
  chkautocompletion->setChecked (value);
  comboactype->setEnabled (value);
}

int dlgAppSettings::actype ()
{
  return comboactype->currentIndex ();
}

bool dlgAppSettings::telnetPaste ()
{
  return chktelnetpaste->isChecked ();
}

void dlgAppSettings::setTelnetPaste (bool value)
{
  chktelnetpaste->setChecked (value);
}

bool dlgAppSettings::trimSpaces ()
{
  return chktrimspaces->isChecked ();
}

void dlgAppSettings::setTrimSpaces (bool value)
{
  chktrimspaces->setChecked (value);
}

int dlgAppSettings::inputbkcolor ()
{
  return comboinputbkcolor->currentIndex ();
}

int dlgAppSettings::inputcolor ()
{
  return comboinputcolor->currentIndex ();
}

void dlgAppSettings::setACType (int value)
{
  comboactype->setCurrentIndex (value);
}

void dlgAppSettings::setInputBkColor (int value)
{
  comboinputbkcolor->setCurrentIndex (value);
}

void dlgAppSettings::setInputColor (int value)
{
  comboinputcolor->setCurrentIndex (value);
}

bool dlgAppSettings::swapEnters ()
{
  return chkswapenters->isChecked ();
}

void dlgAppSettings::setSwapEnters (bool value)
{
  chkswapenters->setChecked (value);
}

void dlgAppSettings::setColor (int i, QColor color)
{
  cbutton[i]->setColor (color);
}

QColor dlgAppSettings::color (int i)
{
  return cbutton[i]->color ();
}

QString dlgAppSettings::cmdseparator ()
{
  return edcmdseparator->text().trimmed();
}

QString dlgAppSettings::speedwalk ()
{
  return edspeedwalk->text().trimmed();
}

QString dlgAppSettings::script ()
{
  return edscript->text().trimmed();
}

QString dlgAppSettings::multicmd ()
{
  return edmulticmd->text().trimmed();
}

QString dlgAppSettings::focus ()
{
  return edfocus->text().trimmed();
}

QString dlgAppSettings::noParse ()
{
  return ednoparse->text().trimmed();
}

void dlgAppSettings::setCmdSeparator (QString what)
{
  edcmdseparator->setText (what);
}

void dlgAppSettings::setSpeedWalk (QString what)
{
  edspeedwalk->setText (what);
}

void dlgAppSettings::setScript (QString what)
{
  edscript->setText (what);
}

void dlgAppSettings::setMultiCmd (QString what)
{
  edmulticmd->setText (what);
}

void dlgAppSettings::setFocusCmd (QString what)
{
  edfocus->setText (what);
}

void dlgAppSettings::setNoParse (QString what)
{
  ednoparse->setText (what);
}

bool dlgAppSettings::allowEmptyWalkStr ()
{
  return chkemptywalk->isChecked();
}

bool dlgAppSettings::expandBS ()
{
  return chkexpandBS->isChecked();
}

void dlgAppSettings::setAllowEmptyWalkStr (bool val)
{
  chkemptywalk->setChecked (val);
}

void dlgAppSettings::setExpandBS (bool val)
{
  chkexpandBS->setChecked (val);
}

const QStringList & dlgAppSettings::soundDirs ()
{
  return dirlist->getDirList();
}

bool dlgAppSettings::allowMSP ()
{
  return chkallowmsp->isChecked ();
}

bool dlgAppSettings::allowDownloads ()
{
  return chkallowdownloads->isChecked ();
}

void dlgAppSettings::setSoundDirs (const QStringList &list)
{
  dirlist->setDirList(list);
}

void dlgAppSettings::setAllowMSP (bool val)
{
  chkallowmsp->setChecked (val);
}

void dlgAppSettings::setAllowDownloads (bool val)
{
  chkallowdownloads->setChecked (val);
}

void dlgAppSettings::getSettingsFromDialog ()
{
  cGlobalSettings *gs = cGlobalSettings::self();
  gs->disableEvents ();  // prevent a save/update after each change

  //Window
  gs->setBool ("always-tab-bar", alwaysTabBar ());
  gs->setBool ("aux-input", auxInput ());
  gs->setBool ("always-notify", alwaysNotify ());
  gs->setBool ("local-notify", localNotify ());
  gs->setBool ("global-notify", globalNotify ());
  gs->setBool ("systray-enabled", sysTrayEnabled ());
  gs->setBool ("passive-popup", passivePopup());
  gs->setString ("auto-connect", autoConnect ());

  //Output area
  gs->setInt ("bg-color", getcolorindex (0));
  gs->setInt ("fg-color", getcolorindex (1));
  gs->setInt ("echo-color", getcolorindex (2));
  gs->setInt ("system-color", getcolorindex (3));
  gs->setBool ("allow-blink", enableBlinking ());
  gs->setBool ("command-echo", cmdEcho ());
  gs->setBool ("show-messages", messages ());
  gs->setInt ("indent", indentation ());
  gs->setInt ("history-size", history ());
  gs->setInt ("force-redraw", forceRedraw ());

  //Fonts
  gs->setFont ("console-font", font1 ());
  gs->setFont ("input-font", font2 ());
  gs->setFont ("multi-line-font", font3 ());

  //Input line
  gs->setBool ("keep-text", keeptext ());
  gs->setBool ("select-kept", selectkepttext ());
  gs->setBool ("cursors-browse", arrowsHistory ());
  gs->setBool ("auto-completion", autocompletion ());
  gs->setInt ("auto-completion-type", actype ());
  gs->setBool ("telnet-style-paste", telnetPaste ());
  gs->setBool ("trim-spaces", trimSpaces ());
  gs->setInt ("input-bg-color", inputbkcolor ());
  gs->setInt ("input-fg-color", inputcolor ());
  gs->setBool ("swap-enters", swapEnters ());

  //Colors
  for (int i = 0; i < 16; i++)
    gs->setColor ("color-" + QString::number (i), color (i));

  //Characters
  gs->setString ("str-separator", cmdseparator ());
  gs->setString ("str-speedwalk", speedwalk ());
  gs->setString ("str-macro", script ());
  gs->setString ("str-multi", multicmd ());
  gs->setString ("str-focus", focus ());
  gs->setString ("str-noparse", noParse ());
  gs->setBool ("empty-walk", allowEmptyWalkStr ());
  gs->setBool ("expand-backslashes", expandBS ());

  //MSP
  QStringList sounddirs;
  sounddirs = soundDirs ();
  gs->setInt ("snd-path-count", sounddirs.size());
  QStringList::iterator it;
  int i = 0;
  for (it = sounddirs.begin(); it != sounddirs.end(); ++it) {
    gs->setString ("snd-path-"+QString::number(i), *it);
    ++i;
  }
  gs->setBool ("msp-allow", allowMSP ());
  gs->setBool ("msp-allow-downloads", allowDownloads ());

  //Shortcuts
  keys->save ();

  cActionManager::self()->invokeEvent ("dialog-save", 0, "app-prefs");

  //apply'em!
  gs->enableEvents ();
}

void dlgAppSettings::putSettingsToDialog ()
{
  cGlobalSettings *gs = cGlobalSettings::self();

  //Window
  setAlwaysTabBar (gs->getBool ("always-tab-bar"));
  setAuxInput (gs->getBool ("aux-input"));
  setAlwaysNotify (gs->getBool ("always-notify"));
  setLocalNotify (gs->getBool ("local-notify"));
  setGlobalNotify (gs->getBool ("global-notify"));
  setSysTrayEnabled (gs->getBool ("systray-enabled"));
  setPassivePopup (gs->getBool ("passive-popup"));
  setAutoConnect (gs->getString ("auto-connect"));

  //Output area
  setcolorindex (0, gs->getInt ("bg-color"));
  setcolorindex (1, gs->getInt ("fg-color"));
  setcolorindex (2, gs->getInt ("echo-color"));
  setcolorindex (3, gs->getInt ("system-color"));
  setEnableBlinking (gs->getBool ("allow-blink"));
  setCmdEcho (gs->getBool ("command-echo"));
  setMessages (gs->getBool ("show-messages"));
  setIndentation (gs->getInt ("indent"));
  setHistory (gs->getInt ("history-size"));
  setForceRedraw (gs->getInt ("force-redraw"));

  //Fonts
  setFont1 (gs->getFont ("console-font"));
  setFont2 (gs->getFont ("input-font"));
  setFont3 (gs->getFont ("multi-line-font"));

  //Input line
  setKeepText (gs->getBool ("keep-text"));
  setSelectKeptText (gs->getBool ("select-kept"));
  setArrowsHistory (gs->getBool ("cursors-browse"));
  setAutoCompletion (gs->getBool ("auto-completion"));
  setACType (gs->getInt ("auto-completion-type"));
  setTelnetPaste (gs->getBool ("telnet-style-paste"));
  setTrimSpaces (gs->getBool ("trim-spaces"));
  setInputBkColor (gs->getInt ("input-bg-color"));
  setInputColor (gs->getInt ("input-fg-color"));
  setSwapEnters (gs->getBool ("swap-enters"));

  //Colors
  for (int i = 0; i < 16; i++)
    setColor (i, gs->getColor("color-" + QString::number (i)));

  //Characters
  setCmdSeparator (gs->getString ("str-separator"));
  setSpeedWalk (gs->getString ("str-speedwalk"));
  setScript (gs->getString ("str-macro"));
  setMultiCmd (gs->getString ("str-multi"));
  setFocusCmd (gs->getString ("str-focus"));
  setNoParse (gs->getString ("str-noparse"));
  setAllowEmptyWalkStr (gs->getBool ("empty-walk"));
  setExpandBS (gs->getBool ("expand-backslashes"));

  //MSP
  QStringList sounddirs;
  for (int i = 0; i < gs->getInt ("snd-path-count"); ++i)
    sounddirs << gs->getString ("snd-path-"+QString::number(i));
  setSoundDirs (sounddirs);
  setAllowMSP (gs->getBool ("msp-allow"));
  setAllowDownloads (gs->getBool ("msp-allow-downloads"));

  //Shortcuts
  //--- NOTHING NEEDED ---
}

