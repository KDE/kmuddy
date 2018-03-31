//
// C++ Interface: cwindowlist
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

#ifndef CWINDOWLIST_H
#define CWINDOWLIST_H

#include "cactionbase.h"

#include <qstringlist.h>
#include <qfont.h>

#include <map>

class cTextChunk;
class dlgOutputWindow;

/**
This class manages output windows.

@author Tomas Mecir
*/

class cWindowList: public cActionBase {
public:
  /** constructor */
  cWindowList (int sess);
  /** destructor */
  ~cWindowList ();

  /** does this window exist? */
  bool exists (const QString &name);
  /** adds a window, returns true if successful */
  bool add (const QString &name, bool autoadd = false);
  /** removes a window, returns true if successful */
  bool remove (const QString &name);

  /** returns a list of windows - kinda slow */
  QStringList windowList ();

  bool show (const QString &name);
  bool hide (const QString &name);
  /** show if hidden, hide if shown :) */
  void toggle (const QString &name);
  bool isShown (const QString &name);

  void textToWindow (const QString &name, cTextChunk *chunk);

  void save ();

  void adjustFonts(QFont font);
  void applySettings (bool allowblinking, int indentvalue, int forceredraw);

protected:
  void load ();
  virtual void eventNothingHandler (QString event, int session) override;

  std::map<QString, dlgOutputWindow *> windows;
  QStringList toerase;

private:
  QString directory;
  QString name, file;
};

#endif
