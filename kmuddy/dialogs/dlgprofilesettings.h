/***************************************************************************
                          dlgprofilesettings.h  -  profile settings
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

#ifndef DLGPROFILESETTINGS_H
#define DLGPROFILESETTINGS_H

#include <config-mxp.h>
#include <kpagedialog.h>

class QCheckBox;
class QComboBox;
class QPushButton;
class KLineEdit;
class KCodecAction;
class cDirList;

/**
Dialog that allows changing of profile-related settings.
  *@author Tomas Mecir
  */

class dlgProfileSettings : public KPageDialog {
   Q_OBJECT
public: 
  dlgProfileSettings (QWidget *parent=nullptr);
  ~dlgProfileSettings () override;

  QSize sizeHint() const override;
  bool useAnsi ();
  void setUseAnsi (bool val);
  bool limitRepeater ();
  void setLimitRepeater (bool val);
  bool negotiateOnStartup ();
  void setNegotiateOnStartup (bool val);
  bool promptLabel();
  void setPromptLabel (bool val);
  bool LPMudStyle();
  void setLPMudStyle (bool val);
  QString encoding ();
  void setEncoding (const QString &encoding);
  QString movementCmd (int which);
  bool statusPrompt();
  void setStatusPrompt(bool val);
  bool consolePrompt();
  void setConsolePrompt(bool val);
  bool autoAdvTranscript();
  void setAutoAdvTranscript(bool val);
  
  void setMovementCmd (int which, QString val);
  QString scriptDir ();
  void setScriptDir (const QString &s);
  QString scriptWorkDir ();
  void setScriptWorkDir (const QString &s);
  QString transcriptDir ();
  void setTranscriptDir (const QString &s);
  bool useMSP ();
  void setUseMSP (bool val);
  bool alwaysMSP ();
  void setAlwaysMSP (bool val);
  bool midlineMSP ();
  void setMidlineMSP (bool val);
  const QStringList &soundDirList ();
  void setSoundDirList (const QStringList &list);
#ifdef HAVE_MXP
  int MXPAllowed ();
  void setMXPAllowed (int allow);
  void setVariablePrefix (const QString &prefix);
  QString variablePrefix ();
#endif
protected slots:
  void choosedir1 ();
  void choosedir2 ();
  void choosedir3 ();
  void encodingChanged (const QString &);
protected:
  QCheckBox *useansi, *limitrepeater, *startupneg, *promptlabel;
  QPushButton *btencoding;
  KCodecAction *codecAction;
  QCheckBox *lpmudstyle, *statusprompt, *consoleprompt;
  QCheckBox *autoadvtranscript;
  KLineEdit *edcmd[10];
  KLineEdit *edscriptdir, *edscriptworkdir;
  KLineEdit *edtranscriptdir;
  QCheckBox *usemsp;
  cDirList *dirlist;
  QCheckBox *alwaysmsp, *midlinemsp;
#ifdef HAVE_MXP
  QComboBox *cbmxpallowed;
  KLineEdit *edvarprefix;
#endif
};

#endif
