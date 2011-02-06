//
// C++ Interface: cmultiinputline
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
