/***************************************************************************
                          cmultilineinput.h  -  multi line input handler
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Po Aug 26 2002
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

#ifndef CMULTILINEINPUT_H
#define CMULTILINEINPUT_H

#include "cactionbase.h"

#include <qobject.h>
#include <QFont>
#include <qstring.h>

class dlgMultiLine;
class QDockWidget;

/**
This class handles the multi-line input edit. It contains function to show,
hide, send contents etc.
Contents are sent to currently active connection.
  *@author Tomas Mecir
*/

class cMultiLineInput : public QObject, public cActionBase {
   Q_OBJECT
public:
  static cMultiLineInput *self();
  ~cMultiLineInput ();
  QDockWidget *dialog ();
  void setFont (QFont font);
protected slots:
  void sendInput (const QStringList &list);
protected:
  cMultiLineInput ();
  void init ();
  virtual void eventNothingHandler (QString event, int session);

  dlgMultiLine *multiline;
  static cMultiLineInput *_self;
};

#endif
