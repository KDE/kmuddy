//
// C++ Implementation: converterplugin
//
// Description: Converter plugin.
//
/*
Copyright 2008-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "converterplugin.h"

#include "converterdialog.h"

#include "cactionmanager.h"
#include "cmacromanager.h"
#include "cmenumanager.h"
#include "cprofilemanager.h"
#include "cprofilesettings.h"

#include "cgenericlist.h"
#include "cgenericitem.h"

#include "csaveablelist.h"
#include "caction.h"
#include "calias.h"
#include "cconnprefs.h"
#include "cgauge.h"
#include "cscript.h"
#include "cshortcut.h"
#include "cstatusvar.h"
#include "ctimer.h"
#include "ctrigger.h"
#include "cvariable.h"
#include "cvartrigger.h"

#include <QAction>
#include <QDir>
#include <QFile>
#include <QXmlStreamWriter>

#include <KLocalizedString>
#include <kmessagebox.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_CLASS_WITH_JSON(cConverterPlugin, "converterplugin.json")


// the main plug-in code which registers the functions:

struct cConverterPluginPrivate {
  QAction *converter;
};

cConverterPlugin::cConverterPlugin (QObject *, const QVariantList &)
{
  d = new cConverterPluginPrivate;

  d->converter = new QAction (this);
  d->converter->setText (i18n ("Profile &Converter..."));
  connect (d->converter, SIGNAL (triggered()), this, SLOT (converterDialog()));
 
  cMenuManager *menu = cMenuManager::self();
  menu->plug (d->converter, "tools-slot3");
} 

cConverterPlugin::~cConverterPlugin()
{
  cMenuManager *menu = cMenuManager::self();
  menu->unplug (d->converter);
  delete d;
}

void cConverterPlugin::converterDialog ()
{
  // create the dialog
  // TODO: use the main window as a parent
  cConverterDialog *dlg = new cConverterDialog (cActionManager::self()->mainWidget());

  // search for profiles and fill in profile info
  std::list<ConversionDialogItem *> items;
  QStringList paths;
  paths << (QDir::homePath() + "/.kde/share/apps/kmuddy/profiles");
  paths << (QDir::homePath() + "/.kde3/share/apps/kmuddy/profiles");
  paths << (QDir::homePath() + "/.kde4/share/apps/kmuddy/profiles");
  QStringList::iterator pathit;
  for (pathit = paths.begin(); pathit != paths.end(); ++pathit) {
    QDir dir (*pathit);
    dir.setFilter (QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
      ConversionDialogItem *item = new ConversionDialogItem;
      item->convert = false;
      item->name = list.at(i).fileName();
      item->path = dir.absolutePath() + "/" + item->name;
      items.push_back (item);
    }
  }

  // display it
  bool res = dlg->display (items);

  // TODO: display some progress dialog or something ... KProgressDialog likely

  int count = 0;
  if (res) {
    // we want to do the conversion
    std::list<ConversionDialogItem *>::iterator it;
    for (it = items.begin(); it != items.end(); ++it) {
      ConversionDialogItem *item = *it;
      if (!item->convert) continue;
      convertProfile (item->path, item->name);
      ++count;
    }
    if (count)
      KMessageBox::information (nullptr, i18n ("The profiles have been successfully converted."));
    else
      KMessageBox::error (nullptr, i18n ("You did not specify any profiles to convert."));
  }

  // delete the profile info
  std::list<ConversionDialogItem *>::iterator it;
  for (it = items.begin(); it != items.end(); ++it)
    delete *it;

  delete dlg;
}

void cConverterPlugin::convertProfile (const QString &path, const QString &name)
{
  // TODO: what to do with the output windows ? They're not converted currently
  
  // load all the data
  cConnPrefs *cp = new cConnPrefs (path);
  cp->load ();
  cSaveableList *aliases = new cSaveableList (path + "/aliases", "Alias", new cAlias(0));
  cSaveableList *triggers = new cSaveableList (path + "/triggers", "Trigger", new cTrigger(0));
  cSaveableList *actions = new cSaveableList (path + "/actions", "Action", new cAction(0));
  cSaveableList *gauges = new cSaveableList (path + "/gauges", "Gauge", new cGauge(0));
  cSaveableList *scripts = new cSaveableList (path + "/scripts", "Script", new cScript(0));
  cSaveableList *shortcuts = new cSaveableList (path + "/macrokeys", "Macro key", new cShortcut(0));
  cSaveableList *statusvars = new cSaveableList (path + "/statusvars", "Status-var", new cStatusVar(0));
  cSaveableList *timers = new cSaveableList (path + "/timers", "Timer", new cTimer(0));
  cSaveableList *variables = new cSaveableList (path + "/variables", "Variable", new cVariable());
  cSaveableList *vartrigs = new cSaveableList (path + "/vartriggers", "Variable trigger", new cVarTrigger(0));

  // create the profile
  cProfileManager *pm = cProfileManager::self();
  QString profile = pm->newProfile (name);
  QString profilePath = pm->profilePath (profile);
  cProfileSettings *sett = pm->settings (profile);

  // fill in settings
  sett->setString ("server", cp->server());
  sett->setInt ("port", cp->port());
  sett->setString ("login", cp->login());
  sett->setString ("password", cp->password());
  QStringList con = cp->connStr();
  sett->setInt ("on-connect-count", con.size());
  for (int i = 0; i < con.size(); ++i)
    sett->setString ("on-connect-"+QString::number(i), con[i]);

  sett->setBool ("use-ansi", cp->ansiColors ());
  sett->setBool ("limit-repeater", cp->limitRepeater ());
  sett->setBool ("startup-negotiate", cp->negotiateOnStartup ());
  sett->setBool ("lpmud-style", cp->LPMudStyle());
  sett->setBool ("prompt-label", cp->promptLabel());
  sett->setBool ("prompt-status", cp->statusPrompt());
  sett->setBool ("prompt-console", cp->consolePrompt());
  sett->setBool ("auto-adv-transcript", cp->autoAdvTranscript());

  for (int i = 0; i < 10; i++)
    sett->setString ("movement-command-"+QString::number(i), cp->cmd (i));
  sett->setString ("script-directory", cp->scriptDir ());
  sett->setString ("script-working-directory", cp->workDir ());
  sett->setString ("transcript-directory", cp->transcriptDir ());

  QStringList sdirs = cp->soundDirs();
  sett->setInt ("sound-dir-count", sdirs.size());
  int idx = 0;
  QStringList::iterator it;
  for (it = sdirs.begin(); it != sdirs.end(); ++it)
    sett->setString ("sound-dir-" + QString::number (++idx), *it);
  sett->setBool ("use-msp", cp->useMSP());
  sett->setBool ("always-msp", cp->alwaysMSP());
  sett->setBool ("midline-msp", cp->midlineMSP());

  sett->setBool ("use-mxp", cp->useMXP());
  sett->setString ("mxp-variable-prefix", cp->varPrefix ());

  sett->save ();

  // fill in generic lists for that profile and store them
  cGenericList *list;

  // aliases
  list = new cGenericList;
  list->init ();
  list->intProperty ("matching", (int) cPattern::begin);
  list->boolProperty ("cs", true);
  list->boolProperty ("prefix-suffix", true);
  list->boolProperty ("whole-words", true);
 
  for (aliases->reset(); *aliases; (*aliases)++) {
    cAlias *alias = (cAlias *) **aliases;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);
    obj->setStr ("pattern", alias->getText());
    obj->setInt ("matching", alias->getType());
    obj->setStr ("condition", alias->cond());

    QStringList ntext = alias->getNewText();
    obj->setInt ("newtext-count", ntext.size());
    QStringList::iterator it; int i;
    for (i = 1, it = ntext.begin(); it != ntext.end(); ++i, ++it)
      obj->setStr ("newtext-" + QString::number (i), *it);

    // Options
    obj->setBool ("orig", alias->sendOriginal());
    obj->setBool ("whole-words", alias->wholeWords());
    obj->setBool ("cs", alias->caseSensitive());
    obj->setBool ("prefix-suffix", alias->includePrefixSuffix());
    obj->setBool ("global", alias->global());
  }
  list->saveList (profilePath + "/aliases.xml");
  delete list;

  // triggers
  list = new cGenericList;
  list->init ();
  list->intProperty ("matching", (int) cPattern::substring);
  list->boolProperty ("cs", true);
  list->boolProperty ("prefix-suffix", true);
  list->boolProperty ("whole-words", true);
  list->intProperty ("action-matched", (int) cList::Stop);
  list->intProperty ("action-not-matched", (int) cList::Continue);
 
  for (triggers->reset(); *triggers; (*triggers)++) {
    cTrigger *trigger = (cTrigger *) **triggers;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);
    obj->setStr ("pattern", trigger->getText());
    obj->setInt ("matching", trigger->getType());
    obj->setStr ("condition", trigger->cond());

    QStringList ntext = trigger->getNewText();
    obj->setInt ("newtext-count", ntext.size());
    QStringList::iterator it; int i;
    for (i = 1, it = ntext.begin(); it != ntext.end(); ++i, ++it)
      obj->setStr ("newtext-" + QString::number (i), *it);

    // Options
    obj->setBool ("dont-send", trigger->dontSend());
    obj->setBool ("cs", trigger->caseSensitive());
    obj->setBool ("global", trigger->global());
    obj->setInt ("action-matched", int (trigger->continueIfMatch() ? cList::Continue : cList::Stop));
    obj->setInt ("action-not-matched", int (trigger->continueIfNoMatch() ? cList::Continue : cList::Stop));

    // Colors
    obj->setBool ("colorize", trigger->isColorTrigger());
    if (trigger->isColorTrigger()) {
      obj->setInt ("colorize-count", trigger->getColorizationsCount());
      for (int i = 0; i < trigger->getColorizationsCount(); i++) {
        int fg = trigger->getColorizationColor(i) % 256;
        QColor fgc = trigger->getColorizationFg(i);
        if (fg == 16) {  // RGB
          fg = fgc.red() * 256 * 256 + fgc.green() * 256 + fgc.blue()+ 1;
        } else if (fg == 255)
          fg = 0;
        else
          fg -= 16;
        int bg = trigger->getColorizationColor(i) / 256;
        QColor bgc = trigger->getColorizationBg(i);
        if (bg == 16) {  // RGB
          bg = bgc.red() * 256 * 256 + bgc.green() * 256 + bgc.blue()+ 1;
        } else if (bg == 255) 
          bg = 0;
        else
          bg -= 16;
        obj->setInt ("colorize-fg-"+QString::number(i+1), fg);
        obj->setInt ("colorize-bg-"+QString::number(i+1), bg);
        obj->setStr ("colorize-variable-"+QString::number(i+1), trigger->getColorizationVariable (i));
      }
    }

    // Rewrite
    obj->setBool ("rewrite", trigger->isRewriteTrigger());
    obj->setStr ("rewrite-text", trigger->rewriteText());
    obj->setStr ("rewrite-var", trigger->rewriteVar());

    // Special
    obj->setBool ("gag", trigger->isGagTrigger());
    obj->setBool ("notify", trigger->isNotifyTrigger());
    obj->setBool ("prompt", trigger->isPromptDetectTrigger());
    obj->setBool ("sound", trigger->isSoundTrigger());
    obj->setStr ("sound-file", trigger->soundFileName());
 
    obj->setBool ("output-window", trigger->isOutputWindowTrigger());
    obj->setBool ("output-gag-in-main", trigger->isGagOutputWindow());
    obj->setStr ("output-window-name", trigger->getOutputWindowName());
  }
  list->saveList (profilePath + "/triggers.xml");
  delete list;

  // actions
  list = new cGenericList;
  list->init ();
 
  for (actions->reset(); *actions; (*actions)++) {
    cAction *action = (cAction *) **actions;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("command", action->getCommand());
    obj->setStr ("command-released", action->getCommand2());
    obj->setStr ("caption", action->getCaption());
    obj->setStr ("icon", action->getIconName());
    obj->setBool ("pushdown", action->isPushDown());
  }
  list->saveList (profilePath + "/buttons.xml");
  delete list;

  // gauges
  list = new cGenericList;
  list->init ();
 
  for (gauges->reset(); *gauges; (*gauges)++) {
    cGauge *gauge = (cGauge *) **gauges;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("variable", gauge->variable());
    obj->setStr ("max-variable", gauge->maxVariable());
    obj->setStr ("caption", gauge->caption());
    QColor c = gauge->color();
    obj->setInt ("color", c.red() * 256 * 256 + c.green() * 256 + c.blue());
    if (gauge->hidden()) obj->setEnabled (false);
  }
  list->saveList (profilePath + "/gauges.xml");
  delete list;

  // shortcuts
  list = new cGenericList;
  list->init ();
  list->boolProperty ("send", true);
 
  for (shortcuts->reset(); *shortcuts; (*shortcuts)++) {
    cShortcut *shortcut = (cShortcut *) **shortcuts;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("command", shortcut->getText());
    int key = shortcut->key();
    int state = shortcut->state();
    // convert from Qt3 key codes to Qt4
    if (key > 0xff) {
      if (key & 0x1000 && (key % 0x1000 <= 0xff))
        key -= 0x1000;
      key |= 0x1000000;
    }
   int modifiers = 0;
    if (state & 0x0100)  // Shift
      modifiers |= Qt::ShiftModifier;
    if (state & 0x0200)  // Ctrl
      modifiers |= Qt::ControlModifier;
    if (state & 0x0400)  // Alt
      modifiers |= Qt::AltModifier;
    if (state & 0x0800)  // Meta
      modifiers |= Qt::MetaModifier;
    if (state & 0x4000)  // Keypad
      modifiers |= Qt::KeypadModifier;
    obj->setInt ("key", key);
    obj->setInt ("modifiers", modifiers);
    obj->setBool ("send", shortcut->sendIt());
    obj->setBool ("overwrite", shortcut->overwriteInput());
  }
  list->saveList (profilePath + "/macrokeys.xml");
  delete list;

 
  // status variables
  list = new cGenericList;
  list->init ();
 
  for (statusvars->reset(); *statusvars; (*statusvars)++) {
    cStatusVar *statusvar = (cStatusVar *) **statusvars;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("variable", statusvar->variable());
    obj->setStr ("max-variable", statusvar->maxVariable());
    obj->setStr ("caption", statusvar->caption());
    obj->setBool ("percentage", statusvar->percentage());
    if (statusvar->hidden()) obj->setEnabled (false);
  }
  list->saveList (profilePath + "/statusvars.xml");
  delete list;
 
  // timers
  list = new cGenericList;
  list->init ();
  list->intProperty ("interval", 60);
 
  for (timers->reset(); *timers; (*timers)++) {
    cTimer *timer = (cTimer *) **timers;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("command", timer->command());
    obj->setInt ("interval", timer->interval());
    obj->setBool ("single-shot", timer->singleShot());
    if (!timer->active()) obj->setEnabled (false);
  }
  list->saveList (profilePath + "/timers.xml");
  delete list;

  // variable triggers
  list = new cGenericList;
  list->init ();
 
  for (vartrigs->reset(); *vartrigs; (*vartrigs)++) {
    cVarTrigger *vartrigger = (cVarTrigger *) **vartrigs;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    obj->setStr ("variable", vartrigger->varName());
    QStringList ntext = vartrigger->getNewText();
    obj->setInt ("command-count", ntext.size());
    QStringList::iterator it; int i;
    for (i = 1, it = ntext.begin(); it != ntext.end(); ++i, ++it)
      obj->setStr ("command-" + QString::number (i), *it);
  }
  list->saveList (profilePath + "/vartriggers.xml");
  delete list;

  // scripts
  list = new cGenericList;
  list->init ();
  list->boolProperty ("flow-control", true);
 
  for (scripts->reset(); *scripts; (*scripts)++) {
    cScript *script = (cScript *) **scripts;
    cListObject *obj = list->newObject();
    list->addToGroup (list->rootGroup(), obj);

    list->setObjectName (obj, script->getName());
    obj->setStr ("command", script->getCommand());
    obj->setStr ("work-directory", script->getWorkDir());
    obj->setBool ("send-user-commands", script->getSendUserCommands());
    obj->setBool ("adv-comunication", script->getUseAdvComm());
    obj->setBool ("flow-control", !script->getNoFlowControl());
    obj->setBool ("enable-variables", script->getAllowVars());
    obj->setBool ("single-instance", script->getSingleInstance());
  }
  list->saveList (profilePath + "/scripts.xml");
  delete list;

 
  // variables - these are saved differently
  QFile f (profilePath + "/variables.xml");
  if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    QXmlStreamWriter *writer = new QXmlStreamWriter (&f);
    writer->setAutoFormatting (true);  // make the generated XML more readable
    writer->writeStartDocument ();

    writer->writeStartElement ("variables");
    writer->writeAttribute ("version", "1.0");

    for (variables->reset(); *variables; (*variables)++) {
      cVariable *variable = (cVariable *) **variables;
      variable->getValue()->save (writer, variable->name());
    }

    writer->writeEndElement ();
    writer->writeEndDocument ();

    f.close ();
    delete writer;
  } 
 
  // clean up
  delete cp;
  delete aliases;
  delete gauges;
  delete shortcuts;
  delete statusvars;
  delete timers;
  delete triggers;
  delete variables;
  delete vartrigs;
}

#include "converterplugin.moc"
#include "moc_converterplugin.cpp"
