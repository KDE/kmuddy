/***************************************************************************
                          dlgappsettings.h  -  application settings
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : St aug 14 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#ifndef DLGAPPSETTINGS_H
#define DLGAPPSETTINGS_H


#include <qfont.h>
#include <kpagedialog.h>

class QLabel;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class KColorButton;
class KShortcutsEditor;
class cDirList;

//TODO: constructor is a bit TOO complicated & long - should be splitted
//into more functions (1 function per page)!!!

/**
Dialog with main application settings.

  *@author Tomas Mecir
  */

class dlgAppSettings : public KPageDialog  {
   Q_OBJECT
public: 
  dlgAppSettings(QWidget *parent=nullptr);
  ~dlgAppSettings() override;

  QSize sizeHint() const override;

//functions to get/set values
//Window
  bool alwaysTabBar ();
  void setAlwaysTabBar (bool value);
  bool auxInput ();
  void setAuxInput (bool val);
  bool alwaysNotify ();
  void setAlwaysNotify (bool value);
  bool localNotify ();
  void setLocalNotify (bool value);
  bool globalNotify ();
  void setGlobalNotify (bool value);
  bool sysTrayEnabled ();
  bool passivePopup ();
  void setSysTrayEnabled (bool value);
  void setPassivePopup(bool value);
  QString autoConnect ();
  void setAutoConnect (const QString &ac);

//Output area
  int getcolorindex (int which);
  void setcolorindex (int which, int value);
  bool cmdEcho ();
  bool messages ();
  void setCmdEcho (bool value);
  void setMessages (bool value);
  bool enableBlinking ();
  void setEnableBlinking (bool value);
  void setIndentation (int value);
  int indentation ();
  void setHistory (int value);
  int history ();
  
//Font
  void setFont1 (QFont f);
  void setFont2 (QFont f);
  void setFont3 (QFont f);
  QFont font1 ();
  QFont font2 ();
  QFont font3 ();

//Inputline
  bool keeptext ();
  bool selectkepttext ();
  bool arrowsHistory ();
  bool autocompletion ();
  int actype ();
  void setKeepText (bool value);
  void setSelectKeptText (bool value);
  void setArrowsHistory (bool value);
  void setAutoCompletion (bool value);
  void setACType (int value);
  bool telnetPaste ();
  void setTelnetPaste (bool value);
  bool trimSpaces ();
  void setTrimSpaces (bool value);
  int inputbkcolor ();
  int inputcolor ();
  void setInputBkColor (int value);
  void setInputColor (int value);
  bool swapEnters ();
  void setSwapEnters (bool value);

//Colors
  void setColor (int i, QColor color);
  QColor color (int i);
  
//Characters
  QString cmdseparator ();
  QString speedwalk ();
  QString script ();
  QString multicmd ();
  QString focus ();
  QString noParse ();
  void setCmdSeparator (QString what);
  void setSpeedWalk (QString what);
  void setScript (QString what);
  void setMultiCmd (QString what);
  void setFocusCmd (QString what);
  void setNoParse (QString what);

  bool allowEmptyWalkStr ();
  bool expandBS ();
  void setAllowEmptyWalkStr (bool val);
  void setExpandBS (bool val);

//MSP
  const QStringList &soundDirs ();
  bool allowMSP ();
  bool allowDownloads ();
  void setSoundDirs (const QStringList &list);
  void setAllowMSP (bool val);
  void setAllowDownloads (bool val);

protected:

  /** put settings to the dialog */
  void putSettingsToDialog ();

//Window
  QCheckBox *chkalwaystabbar, *chkauxinput;
  QCheckBox *chkalwaysnotify, *chkglobalnotify, *chklocalnotify;
  QCheckBox *chkautoconnect;
  QComboBox *autoconnect;
  QCheckBox *chksystray;
  QCheckBox *chkpassivepopup;
//Output area
  QComboBox *combo[4];
  QCheckBox *chkcmdecho, *chkmessages;
  QCheckBox *chkblinking;
  QSpinBox *edindent, *edhistory;
//Font
  QLabel *fonted1, *fonted2, *fonted3;
  QFont font[3];
//Inputline
  QCheckBox *chkkeeptext, *chkselectkepttext, *chkautocompletion;
  QCheckBox *chkarrowshistory;
  QCheckBox *chktelnetpaste;
  QCheckBox *chktrimspaces;
  QCheckBox *chkswapenters;
  QComboBox *comboactype, *comboinputbkcolor, *comboinputcolor;
//Colors
  KColorButton *cbutton[16];
//Characters
  QLineEdit *edcmdseparator, *edspeedwalk, *edscript, *edmulticmd, *edfocus, *ednoparse;
  QCheckBox *chkemptywalk, *chkexpandBS;
//MSP
  QCheckBox *chkallowmsp, *chkallowdownloads;
  cDirList *dirlist;

//Shortcuts
  KShortcutsEditor *keys;
protected slots:
  void fontchooser1 ();
  void fontchooser2 ();
  void fontchooser3 ();
  void getSettingsFromDialog ();
};

#endif
