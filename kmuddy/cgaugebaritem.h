//
// C++ Interface: cgaugebaritem
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGAUGEBARITEM_H
#define CGAUGEBARITEM_H

#include <qwidget.h>

//the "real" gauge displayer
class cGaugeBarItemPrivate;
class QLabel;

/**
One item in the gauge bar. Contains a label and the gauge.

@author Tomas Mecir
*/
class cGaugeBarItem : public QWidget
{
Q_OBJECT
public:
  /** constructor */
  cGaugeBarItem (QWidget *parent = 0);
  /** destructor */
  ~cGaugeBarItem ();
  void setText (const QString &caption);
  QString text () const;
  void setValue (int value);
  int value () const;
  void setColor (const QColor &color);
  QColor color () const;
protected:
  QLabel *label;
  cGaugeBarItemPrivate *gaugepriv;
};

class cGaugeBarItemPrivate: public QWidget
{
Q_OBJECT
public:
  /** constructor */
  cGaugeBarItemPrivate (QWidget *parent = 0);
  /** destructor */
  ~cGaugeBarItemPrivate ();
  void setValue (int value);
  int value () const;
  void setColor (const QColor &color);
  QColor color () const;

  virtual QSize sizeHint () const;
protected:
  void paintEvent (QPaintEvent *);

  int val;
  QColor col;
};

#endif
