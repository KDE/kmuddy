//
// C++ Implementation: testplugin
//
// Description: Testing plugin.
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "testplugin.h"

#include "clistobject.h"
#include "clist.h"
#include "clistmanager.h"
#include "clistgroupeditor.h"

#include <kdebug.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <klineedit.h>

#include <QVBoxLayout>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class cTestEditor : public cListEditor {
 public:
  cTestEditor (QWidget *parent) : cListEditor (parent) {
  }

  void createGUI (QWidget *parent)
  {
    QVBoxLayout *layout = new QVBoxLayout (parent);
    line = new KLineEdit (parent);
    QWidget *commonEditor = createCommonAttribEditor (parent);
    layout->addWidget (line);
    layout->addWidget (commonEditor);
  }

  void fillGUI (const cListObjectData &data)
  {
    fillCommonAttribEditor (data);
    line->setText (data.strValue("text"));
  }

  void getDataFromGUI (cListObjectData *data)
  {
    getDataFromCommonAttribEditor (data);
    data->strValues["text"] = line->text();
  }

 private:
  KLineEdit *line;
};

class cTestObject : public cListObject {
 public:
 protected:
  cTestObject (cList *list, QStandardItem *modelItem = 0)
    : cListObject (list, modelItem) {
  }
  virtual ~cTestObject () {}

  friend class cTestList;
};

class cTestList : public cList {
 public:
  cTestList () : cList ("test") {
    addBoolProperty ("testbool", "Testing boolean property", true);
  }
  virtual ~cTestList () {};
  static cList *newList () { return new cTestList; };
  virtual cListObject *newObject () { return new cTestObject (this); };
  virtual QString objName () { return "Testing Object"; }
  virtual cListEditor *editor (QWidget *parent) { return new cTestEditor (parent); };

};

K_PLUGIN_CLASS_WITH_JSON(cTestPlugin, "testplugin.json")

cTestPlugin::cTestPlugin (QObject *, const QVariantList &)
{
  // register the testing list
  cListManager::self()->registerType ("test", "Testing List", cTestList::newList);

  // your code here ...
  kDebug() << "Testing plugin loaded.";
}

cTestPlugin::~cTestPlugin()
{
  // your code here ...
  kDebug() << "Testing plugin unloaded.";
}


void cTestPlugin::sessionAdd (int sess, bool fresh)
{
  kDebug() << "Testing plugin: sessionAdd " << sess << ", " << (fresh?"fresh":"not fresh");
}

void cTestPlugin::sessionRemove (int sess, bool closed)
{
  kDebug() << "Testing plugin: sessionRemoved " << sess << ", " << (closed?"closed":"not closed");
}

void cTestPlugin::sessionSwitch (int sess)
{
  kDebug() << "Testing plugin: sessionSwitch " << sess;
}

void cTestPlugin::connected (int sess)
{
  kDebug() << "Testing plugin: connected " << sess;
  
  cList *list = cListManager::self()->getList (sess, "test");
  if (!list) { kDebug() << "We do not have the list!" << endl; return; }
  list->addGroup (list->rootGroup(), "Testing Group 1");
  list->addGroup (list->rootGroup(), "Testing Group 2");
  list->addGroup (list->rootGroup(), "Testing Group 3");
  list->addGroup (list->group("Testing Group 2"), "Testing SubGroup 2A");
  cListObject *obj = list->newObject();
  list->addToGroup (list->group ("Testing Group 1"), obj);
  obj->setInt ("testint", 42);
  obj->setBool ("testbool", false);
  obj->setStr ("teststring", "Some&nbsp;<b>lions</b>.");
  list->setObjectName (obj, "xaxaxa");
  kDebug() << "List filled" << endl;
/*
  QFile f ("testlist.xml");
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  QXmlStreamReader reader (&f);
  list->load (&reader);
  f.close();
  if (list->hasError()) kDebug() << list->lastError() << endl;
  else kDebug() << "List loaded" << endl;

  QFile file ("testlist2.xml");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    return;
  QXmlStreamWriter writer (&file);
  list->save (&writer);
  file.close();
  kDebug() << "List saved" << endl;
*/
}

void cTestPlugin::disconnected (int sess)
{
  kDebug() << "Testing plugin: disconnected " << sess;

}

void cTestPlugin::rawData (int sess, char * data)
{
  static bool first = false;
  if (!first)
    kDebug() << "Testing plugin: rawData " << sess << " (first call)";
  first = true;
}

void cTestPlugin::decompressedData (int sess, char * data)
{
  static bool first = false;
  if (!first)
    kDebug() << "Testing plugin: decompressedData " << sess << " (first call)";
  first = true;
}

void cTestPlugin::processInput (int sess, int phase, cTextChunk * chunk,
    bool gagged)
{
  static bool first = false;
  if (!first)
    kDebug() << "Testing plugin: processInput " << sess << " (first call)";
  first = true;
}

void cTestPlugin::processPrompt (int sess, cTextChunk * chunk)
{
  static bool first = false;
  if (!first)
    kDebug() << "Testing plugin: processPrompt " << sess << " (first call)";
  first = true;
}

void cTestPlugin::processCommand (int sess, QString & command, bool & dontSend)
{
  static bool first = false;
  if (!first)
    kDebug() << "Testing plugin: processCommand " << sess << " (first call)";
  first = true;
}



#include "testplugin.moc"
