//
// C++ Interface: cmultiinputline
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CMULTIINPUTLINE_H
#define CMULTIINPUTLINE_H

#include <q3textedit.h>

#include "cactionbase.h"

/**
The multi-line input line. It can hold several lines, and can automatically expand/shrink as needed.

@author Tomas Mecir
*/
class cMultiInputLine : public Q3TextEdit, public cActionBase
{
Q_OBJECT
public:
  /** constructor */
  cMultiInputLine (int sess, QWidget *parent = 0);
  /** destructor */
  ~cMultiInputLine ();
  
  /** initialize the input line - must be separated
  from the constructor, because it uses cSession, which is not
  available in constructor */
  void initialize ();

  void keepText (bool value);
  void selectKeptText (bool value);
  void swapEnters (bool value);
  
  /** set height, given as a number of lines */
  void setLinesHeight (int lines);
  
  /** set the new font */
  virtual void setFont (const QFont &font);
  
signals:
  void commandEntered (const QString &command);

protected slots:
  void updateHeight ();
protected:
  virtual void eventNothingHandler (QString event, int session);
  void sendCommands ();

  virtual void keyPressEvent (QKeyEvent *e);

  int _lines;

  bool keeptext;
  bool selectkepttext;
  bool swapenters;
};

#endif
