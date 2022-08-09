/***************************************************************************
                          dlgmultiline.h  -  multi-line input dialog
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Aug 26 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
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

#ifndef DLGMULTILINE_H
#define DLGMULTILINE_H

#include <QDockWidget>

class QTextEdit;
class KLineEdit;

/**
Multi-line input dialog.
  *@author Tomas Mecir
*/

class dlgMultiLine : public QDockWidget {
   Q_OBJECT
public: 
  dlgMultiLine (QWidget *parent = nullptr);
  ~dlgMultiLine () override;
  QSize sizeHint() const override;
  /** set font */
  void setFont (const QFont &font);
signals:
  void commands (const QStringList &commands);
protected slots:
  void sendClicked ();
protected:
  /** the most important stuff here... */
  QTextEdit *editor;
  KLineEdit *prefix, *suffix;
};

#endif
