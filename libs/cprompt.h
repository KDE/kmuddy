//
//
// C++ Interface: cprompt
//
// Description:
//
/*
Copyright 2003-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#ifndef CPROMPT_H
#define CPROMPT_H

#include <cactionbase.h>
#include <kmuddy_export.h>

#include <qlabel.h>

class KMUDDY_EXPORT cPrompt : public QLabel, public cActionBase {
   Q_OBJECT
 public:
  cPrompt (int sess, QWidget *parent);
  ~cPrompt();
  void updatePrompt (const QString &text);
 protected:
  virtual void eventStringHandler (QString event, int, QString &par1, const QString &) override;
  virtual void eventNothingHandler (QString event, int) override;

};

#endif
