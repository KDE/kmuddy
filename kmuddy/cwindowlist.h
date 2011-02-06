//
// C++ Interface: cwindowlist
//
// Description: 
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
  void applySettings (bool allowblinking, bool wordwrapping, int wrappos,
      int indentvalue, int forceredraw);

protected:
  void load ();
  virtual void eventNothingHandler (QString event, int session);

  std::map<QString, dlgOutputWindow *> windows;
  QStringList toerase;

private:
  QString directory;
  QString name, file;
};

#endif
