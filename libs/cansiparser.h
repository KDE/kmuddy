/***************************************************************************
                          cansiparser.h  -  ANSI parser
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Pá Jun 21 2002
    copyright            : (C) 2002-2004 by Tomas Mecir
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

#ifndef CANSIPARSER_H
#define CANSIPARSER_H

#include <QObject>
#include <QColor>

#include <cactionbase.h>

#define CL_BLACK 0
#define CL_RED 1
#define CL_GREEN 2
#define CL_YELLOW 3
#define CL_BLUE 4
#define CL_MAGENTA 5
#define CL_CYAN 6
#define CL_WHITE 7

#define CL_BRIGHT 8

/**
ANSI parser - slot is connected to cTelnet, output goes to displaying widget
and also to some other classes that handle triggers, automapper etc.
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cANSIParser : public QObject, public cActionBase  {
  Q_OBJECT
public: 
  cANSIParser (int sess);
  ~cANSIParser ();

  /** get color in color palette */
  QColor color (int index);
  /** change color palette */
  void setColor (QColor color, int index);

  /** get default textcolor */
  QColor defaultTextColor ();
  /** change default textcolor */
  void setDefaultTextColor (QColor color);
  /** get default background */
  QColor defaultBkColor ();
  /** change default background */
  void setDefaultBkColor (QColor color);

  void setUseAnsi (bool val) { useansi = val; };
  
  virtual void eventNothingHandler (QString event, int session);

signals:
  void fgColor (QColor color);
  void bgColor (QColor color);
  void attrib (int a);
  void plainText (const QString &text);
public slots:
  void parseText (const QString &data);
  void flush ();
protected:
  void changeColor (int color);
  void activateBright ();
  void deactivateBright ();
  QColor mycolor[16];
  
  QString buffer;
  bool useansi;
  
  /** some ANSI flags */
  bool brightactive, blinkactive;
  bool underline, italics, strikeout;
  bool negactive;
  bool invisible;
  
  /** some colors */
  QColor curcolor, curbkcolor;
  QColor defcolor, defbkcolor;
};

#endif
