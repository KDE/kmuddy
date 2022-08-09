/***************************************************************************
                          cvariablelist.cpp  -  list of variables
                             -------------------
    begin                : Po sep 8 2003
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

#include "cvariablelist.h"

#include "ccmdqueue.h"
#include "cprofilemanager.h"
#include "cvalue.h"
#include "cvariable.h"

#include <kdebug.h>
#include <KLocalizedString>

#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using namespace std;

//maximum number of passes when doing recursive variable expansion

#define MAXEXPANDPASSES 10

cVariableList::cVariableList (int sess)
      : cActionBase ("variables", sess)
{
  load ();
}

cVariableList::~cVariableList ()
{
  save ();
  vars.clear ();
}

QString cVariableList::getValue (const QString &varname)
{
  cValue *val = value (varname);
  //no such variable -> nothing to do
  if (!val) return QString();

  return val->asString();
}

int cVariableList::getIntValue (const QString &varname)
{
  cValue *val = value (varname);
  //no such variable -> nothing to do
  if (!val) return 0;

  return val->asInteger();
}

cValue *cVariableList::value (const QString &varname, cCmdQueue *queue)
{
  if (varname.isEmpty()) return nullptr;
  QString vn = varname;
  if (varname[0] == '$')
    vn = vn.mid(1);

  if (queue) {
    cValue *value = queue->value (vn);
    if (value)
      return value;
  }

  if (!exists (vn))
    return nullptr;
  
  return vars[vn]->getValue();
}

cValue *cVariableList::valueNotEmpty (const QString &varname, cCmdQueue *queue)
{
  cValue *val = value (varname, queue);
  if (val) return val;
  // doesn't exist - create empty !
  set (varname, QString());
  return value (varname, queue);
}
  
bool cVariableList::exists (const QString &varname)
{
  if (varname.isEmpty()) return false;
  QString vn = varname;
  if (varname[0] == '$')
    vn = vn.mid(1);
  return (vars.count (vn) != 0);
}

void cVariableList::set (const QString &varname, const QString &value)
{
  cValue v (value);
  set (varname, &v);
}

void cVariableList::set (const QString &varname, cValue *value)
{
  if (varname.isEmpty()) return;
  QString vn = varname;
  if (varname[0] == '$')
    vn = vn.mid(1);

  QString oldval = QString();
  QString newval = value ? value->asString() : QString();
  bool changed = (oldval != newval);
  if (exists (vn))
  {
    //variable already exists - we just update its value
    oldval = vars[vn]->value ();
    vars[vn]->setValue (value);
  }
  else
  {
    //good, add new variable to the list
    cVariable *var = new cVariable;
    var->setName (vn);
    var->setValue (value);
    vars[vn] = var;
    changed = true;   // if we're adding a new value, always report the change
  }

  //inform everyone about the change, if needed
  if (!changed) return;
  invokeEvent ("old-var-value", sess(), vn, oldval);
  invokeEvent ("var-changed", sess(), vn, value ? value->asString() : QString());
}

void cVariableList::unset (const QString &varname)
{
  if (varname.isEmpty()) return;
  QString vn = varname;
  if (varname[0] == '$')
    vn = vn.mid(1);

  //no such variable -> nothing to do
  if (!exists (vn))
    return;

  //get the variable
  cVariable *var = vars[vn];
  QString oldval = var->value();
  //and remove it
  vars.erase (vn);
  delete var;

  invokeEvent ("old-var-value", sess(), vn, oldval);
  invokeEvent ("var-changed", sess(), vn, QString());
}

void cVariableList::inc (const QString &varname, double delta)
{
  if (!exists (varname)) {
    cValue val (delta);
    set (varname, &val);
  } else {
    QString vn = varname;
    if (varname[0] == '$')
      vn = vn.mid(1);
    cValue *val = vars[vn]->getValue();
    if (val) {
      QString oldval = val->asString();
      val->setValue (val->asDouble() + delta);
      // we are bypassing cVariableList::set here, hence we need to invoke
      // this event ourselves ...
      invokeEvent ("old-var-value", sess(), vn, oldval);
      invokeEvent ("var-changed", sess(), vn, val->asString());
    }
    else {
      cValue d (delta);
      set (varname, &d);
    }
  }
}

void cVariableList::dec (const QString &varname, double delta)
{
  inc (varname, -1.0 * delta);
}

void cVariableList::provideResource (const QString &varname)
{
  //returns a resource to available ones
  inc (varname, 1);
}

bool cVariableList::requestResource (const QString &varname)
{
  //is a resource available?
  if (getIntValue (varname) <= 0)
    //no :(
    return false;
  //yes, "allocate" one
  dec (varname, 1);
  return true;
}

QStringList cVariableList::getList ()
{
  map<QString, cVariable *>::iterator it;
  QStringList list;
  for (it = vars.begin(); it != vars.end(); ++it)
    list.push_back (it->first);
  return list;
}

QString cVariableList::expandVariables (const QString &string, bool recursive, cCmdQueue *queue)
{
  return doExpandVariables (string, recursive ? MAXEXPANDPASSES : 1, queue);
}

QString cVariableList::doExpandVariables (const QString &string, int recursionCounter,
    cCmdQueue *queue)
{
  QString newstring = "";
  int len = string.length ();
  bool hadvar = false; //whether we had some variable - used to prevent
                       //unneeded recursion
  bool invar = false;
  bool inpar = false; //variable name is in parentheses '(' and ')'
  QString varname;
  for (int i = 0; i < len; i++)
  {
    QChar ch = string[i];

    //support things like $$$a correctly
    if (invar && (!inpar) && (ch == '$') && (varname.isEmpty()))
    {
      invar = false;
      newstring += ch;
    }

    if (!invar)
    {
      if (ch == '$')
      {
        invar = true;
        inpar = false;
        varname = "";
      }
      else
        newstring += ch;
    }
    else
    {
      if ((varname == "") && (ch == '(') && (!inpar))
        inpar = true;
      else
      if ((!ch.isLetterOrNumber())&&
          (ch != '_')               )  //end of variable name
      {
        invar = false;

        if (inpar)
        {
          inpar = false;
          if (ch == ')')
          {
            QString newstr = processVariable (varname, queue);
            if (newstr == QString())
              newstring += "$(" + varname + ")";
            else
            {
              newstring += newstr;
              hadvar = true;
            }
          }
          else
            newstring += "$(" + varname + ch;
        }
        else
        {
          QString newstr = processVariable (varname, queue);
          if (newstr.isEmpty())
            newstring += "$" + varname;
          else
          {
            newstring += newstr;
            hadvar = true;  
          }
          newstring += ch;
        }
      }
      else
        varname += ch;
    }
  }

  if (invar) {    //if a variable ends the string
    if (inpar)
      newstring += "$(" + varname;  //no ending parenthesis - no variable
    else
    {
      QString newstr = processVariable (varname, queue);
      if (newstr.isEmpty())
        newstring += "$" + varname;
      else
      {
        newstring += newstr;
        hadvar = true;
      }
    }
 }

  //another pass if needed, then return the result
  recursionCounter--;
  if (hadvar && (recursionCounter > 0))
    return doExpandVariables (newstring, recursionCounter, queue);
  return newstring;
}

QString cVariableList::processVariable (const QString &varname, cCmdQueue *queue)
{
  if (varname.isEmpty()) return varname;  // nothing if the varname is empty
  QString vn = varname;
  if (varname[0] == '$')
    vn = vn.mid(1);

  if (queue && queue->varExists (vn))
    return queue->getValue (vn);
  return getValue (vn);
}

void cVariableList::load ()
{
  // load the list contents
  cProfileManager *pm = cProfileManager::self();

  QString path = pm->profilePath (sess());
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  QString fName = "variables.xml";

  QFile f (path + "/" + fName);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    kDebug() << "No " << fName << " file - nothing to do." << endl;
    return;  // no profiles - nothing to do
  }

  QXmlStreamReader *reader = new QXmlStreamReader (&f);

  // we have the reader, now we actually load the list
  reader->readNext ();  // read the document start
  reader->readNext ();
  if (reader->isStartElement ())
    if (reader->name() == "variables")
      if (reader->attributes().value ("version") == "1.0") {
        // we're inside the root element, now we load the list
        while (!reader->atEnd()) {
          cValue val;
          QString name = val.load (reader);
          if (!name.isEmpty())
            set (name, &val);
        }
      }
      else
        reader->raiseError (i18n ("This file was created by a newer version of KMuddy, and this version is unable to open it."));
    else
      reader->raiseError (i18n ("This is not a KMuddy variable list file."));
  else if (!reader->hasError())
    reader->raiseError (i18n ("This file is corrupted."));

  if (reader->hasError()) {
    // TODO: present the error to the user
    QString lastError = i18n ("Error at line %1, column %2: %3",
                                QString::number (reader->lineNumber()),
                                QString::number (reader->columnNumber()),
                                reader->errorString());
    kWarning() << lastError << endl;
  }
  delete reader;
  f.close ();
}

void cVariableList::save ()
{
  // save the list contents
  cProfileManager *pm = cProfileManager::self();

  QString path = pm->profilePath (sess());
  QDir dir = QDir (path);
  if (!dir.exists()) QDir::root().mkpath (dir.absolutePath());

  dir.remove ("variables.backup");
  QString fName = "variables.xml";
  if (!QFile(path + "/" + fName).copy (path + "/variables.backup")) {
    kDebug() << "Unable to backup " << fName << endl;  // not fatal, may simply not exist
  }

  QFile f (path + "/" + fName);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    kDebug() << "Unable to open " << (path + "/" + fName) << " for writing." << endl;
    return;  // problem
  }

  QXmlStreamWriter *writer = new QXmlStreamWriter (&f);

  writer->setAutoFormatting (true);  // make the generated XML more readable
  writer->writeStartDocument ();

  writer->writeStartElement ("variables");
  writer->writeAttribute ("version", "1.0");

  map<QString, cVariable *>::iterator it;
  for (it = vars.begin(); it != vars.end(); ++it)
    it->second->getValue()->save (writer, it->first);

  writer->writeEndElement ();
  writer->writeEndDocument ();

  f.close ();
  delete writer;
}

