/***************************************************************************
                          cshortcut.cpp  -  macro key/shortcut
                             -------------------
    begin                : St máj 28 2003
    copyright            : (C) 2003-2008 by Tomas Mecir
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

#include "cshortcut.h"

#include "cactionmanager.h"
#include "cinputline.h"
#include "cshortcutlist.h"
#include "cscripteval.h"

#include <qkeysequence.h>
#include <KLocalizedString>

struct cShortcut::Private {
  QString command;
  unsigned int key, modifiers;
  bool send, overwrite;
};

cShortcut::cShortcut (cList *list) : cListObject (list)
{
  d = new Private;
  d->send = true;
  d->overwrite = false;
  d->key = 0;
  d->modifiers = 0;
}

cShortcut::~cShortcut ()
{
  delete d;
}

void cShortcut::attribChanged (const QString &name)
{
  if (name == "command") {
    d->command = strVal ("command");
    updateVisibleName ();
    return;
  }
  if (name == "key") {
    d->key = intVal ("key");
    updateVisibleName ();
    return;
  }
  if (name == "modifiers") {
    d->modifiers = intVal ("modifiers");
    updateVisibleName ();
    return;
  }
  if (name == "send") {
    d->send = boolVal ("send");
    return;
  }
  if (name == "overwrite") {
    d->overwrite = boolVal ("overwrite");
    return;
  }
  if (name == "script") updateVisibleName ();
}

void cShortcut::updateVisibleName()
{
  QString cmd = d->command;
  if (cmd.isEmpty() && (!strVal ("script").isEmpty())) cmd = "(script)";
  if (cmd.isEmpty())
    cListObject::updateVisibleName();
  setVisibleName (keyToString (d->key, d->modifiers) + " -> " + cmd);
}

cList::TraverseAction cShortcut::traverse (int traversalType)
{
  cActionManager *am = cActionManager::self();
  int sess = list()->session();

  if (traversalType == SHORTCUT_MATCH) {
    cShortcutList *sl = (cShortcutList *) list ();
    if ((sl->currentKey == d->key) && (sl->currentModifiers == d->modifiers)) {
      // match
      if (!d->command.isEmpty()) {
        if (d->send)
          am->invokeEvent ("command", sess, d->command);
        else {
          cInputLine *inputline = dynamic_cast<cInputLine *>(am->object ("inputline", sess));
          d->overwrite ? inputline->setText (d->command) : inputline->insert (d->command);
        }
      }
      // execute the script, if any
      QString script = strVal ("script");
      if (!script.isEmpty()) {
        cScriptEval *eval = dynamic_cast<cScriptEval *>(am->object ("scripteval", sess));
        if (eval) eval->eval (script);
      }

      sl->matched = true;
    }
    return cList::Continue;
  }
  return cList::Stop;
}


QString cShortcut::keyToString (int _key, int _state)
{
  //all existing functions are strange... Or at least I failed to get
  //anything useful out of KKey and KShortcut. QKeySequence also seems
  //to accept input in some other format, and it doesn't recognize numpad
  //keys properly. Therefore, I made my own function that works well :)

  QString base = QKeySequence (_key).toString();
  if (base.length() == 0)
    base = i18n ("(unknown)");
  if (_key == 0)
    base = i18n ("(none)");
  if (_state & Qt::KeypadModifier)
    base = "KP_" + base;
  if (_state & Qt::ShiftModifier)
    base = i18n ("Shift") + " + " + base;
  if (_state & Qt::ControlModifier)
    base = i18n ("Ctrl") + " + " + base;
  if (_state & Qt::AltModifier)
    base = i18n ("Alt") + " + " + base;
  if (_state & Qt::MetaModifier)
    base = i18n ("Meta") + " + " + base;
  return base;  
}

