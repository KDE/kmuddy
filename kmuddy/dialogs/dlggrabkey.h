/***************************************************************************
                          dlggrabkey.h  -  keygrabber
                             -------------------
    begin                : Pi máj 30 2003
    copyright            : (C) 2003 by Tomas Mecir
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

#ifndef DLGGRABKEY_H
#define DLGGRABKEY_H

#include <QDialog>

/**
This dialog is used to grab keys. It works in conjuction with event filter
installed in KMuddy class. Maybe I'll rewrite this to use
QWidget::grabKeyboard instead, but current implementation is sufficient now.

KKeyChooser is partially broken, it cannot handle numpad keys properly,
so I have to create my own keygrabber.
(or maybe it's QAction not supporting numpad keys - at least that's what
KDE people say...). I don't use QAction here, so macro keys are not affected.

  *@author Tomas Mecir
  */

class dlgGrabKey : public QDialog  {
   Q_OBJECT
public:
  dlgGrabKey (QWidget *parent);
  ~dlgGrabKey ();
  virtual QSize sizeHint() const override;
  /** key is here - sent by KMuddy::eventFilter */
  void gotKey (int key, Qt::KeyboardModifiers state);

  int key() { return _key; };
  Qt::KeyboardModifiers state() { return _state; };
protected:
  int _key;
  Qt::KeyboardModifiers _state;
};

#endif
