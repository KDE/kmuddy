//
// C++ Implementation: cgaugebaritem
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

#include "cgaugebaritem.h"

#include <qlabel.h>
#include <qpainter.h>
#include <QHBoxLayout>

cGaugeBarItem::cGaugeBarItem (QWidget *parent)
  : QWidget(parent)
{
  QHBoxLayout *layout = new QHBoxLayout (this);
  layout->setSpacing (2);
  setContentsMargins (0, 0, 0, 0);
  setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Preferred);
  label = new QLabel (this);
  label->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
  gaugepriv = new cGaugeBarItemPrivate (this);
  layout->addWidget (label);
  layout->addWidget (gaugepriv);
}

cGaugeBarItem::~cGaugeBarItem ()
{
}

void cGaugeBarItem::setText (const QString &caption)
{
  label->setText (caption);
}

QString cGaugeBarItem::text () const
{
  return label->text();
}

void cGaugeBarItem::setValue (int value)
{
  gaugepriv->setValue (value);
}

int cGaugeBarItem::value () const
{
  return gaugepriv->value();
}

void cGaugeBarItem::setColor (const QColor &color)
{
  gaugepriv->setColor (color);
}

QColor cGaugeBarItem::color () const
{
  return gaugepriv->color ();
}

cGaugeBarItemPrivate::cGaugeBarItemPrivate (QWidget *parent)
  : QWidget(parent)
{
  val = 0;
  col = Qt::white;

  setMaximumWidth (102);
  setFocusPolicy (Qt::NoFocus);
}

cGaugeBarItemPrivate::~cGaugeBarItemPrivate ()
{

}

void cGaugeBarItemPrivate::setValue (int value)
{
  val = value;
  update ();
}

int cGaugeBarItemPrivate::value () const
{
  return val;
}

void cGaugeBarItemPrivate::setColor (const QColor &color)
{
  col = color;
  update ();
}

QColor cGaugeBarItemPrivate::color () const
{
  return col;
}

void cGaugeBarItemPrivate::paintEvent (QPaintEvent *)
{
  int w = width();
  int h = height();
  QPainter p (this);
  p.setPen (QPen (palette().dark(), 1));
  p.drawRect (0, 0, w - 1, h - 1);
  if (val)
  {
    w = val * (w - 2) / 100;   //width of the colored part
    p.fillRect (1, 1, w, h - 2, col);
  }
}

QSize cGaugeBarItemPrivate::sizeHint () const {
  return QSize (102, 16);
}


#include "cgaugebaritem.moc"
