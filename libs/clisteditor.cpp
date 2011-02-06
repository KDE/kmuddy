//
// C++ Implementation: clisteditor
//
// Description: list editor widget - base class
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "clisteditor.h"
#include "clist.h"
#include "clistmanager.h"

#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kpushbutton.h>
#include <klineedit.h>

struct cListEditor::Private {
  cListObject *obj;
  int objNum;
  cListObjectData data;      // data from the object
  cListObjectData guiData;   // data, as filled into the GUI

  // GUI elements of the common attrib editor
  KIntNumInput *edpriority;
  QCheckBox *chkenabled;
  KLineEdit *edname;

  QWidget *centralBar;
  bool guiCreated;
  bool saving;
};

cListEditor::cListEditor (QWidget *parent)
  : QWidget (parent)
{
  d = new Private;
  d->objNum = 0;
  d->obj = 0;
  d->guiCreated = false;
  d->saving = false;

  // set our policy to expanding, and give us a grid layout.
  QSizePolicy qsp (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setSizePolicy (qsp);
  QVBoxLayout *layout = new QVBoxLayout (this);
  layout->setSpacing (5);

  d->centralBar = new QWidget (this);
  d->centralBar->setSizePolicy (qsp);
  QWidget *buttonBar = new QWidget (this);
  QHBoxLayout *buttonLayout = new QHBoxLayout (buttonBar);
  buttonLayout->setSpacing (20);
  buttonLayout->setAlignment (Qt::AlignCenter);

  KPushButton *saveButton = new KPushButton (i18n ("&Apply changes"), buttonBar);
  KPushButton *undoButton = new KPushButton (i18n ("&Undo changes"), buttonBar);
  connect (saveButton, SIGNAL (clicked()), this, SLOT (saveClicked ()));
  connect (undoButton, SIGNAL (clicked()), this, SLOT (undoClicked ()));
  
  layout->addWidget (d->centralBar);
  layout->addWidget (buttonBar);
  // row 1 is fixed, row 0 will grow to fill the available space.
  layout->setStretchFactor (d->centralBar, 1);
  buttonLayout->addWidget (saveButton);
  buttonLayout->addWidget (undoButton);
}

cListEditor::~cListEditor ()
{
  delete d;
}

void cListEditor::setObject (cListObject *obj)
{
  if (d->obj) {
    disconnect (d->obj, SIGNAL (changed (cListObject *)), this, SLOT (objectChanged (cListObject *)));
    d->obj = 0;
  }

  if (!obj) return;
  if (!d->guiCreated)
  {
    d->guiCreated = true;
    createGUI (d->centralBar);
  }

  d->obj = obj;
  d->objNum = cListManager::self()->objectId (obj);
  connect (d->obj, SIGNAL (changed (cListObject *)), this, SLOT (objectChanged (cListObject *)));
  loadDataFromObject ();
  fillGUI (d->data);
}

cListObject *cListEditor::object ()
{
  return d->obj;
}

void cListEditor::objectChanged (cListObject *obj)
{
  if (d->saving) return;  // ignore events triggered by our saving
  if (obj != d->obj) return;   // this should never happen

  // okay, so merge in the changes
  // we have three lists - original object data, new object data, and data from the dialog
  // we replace original data with new data except where something changed in the dialog
  cListObjectData oldData = d->data;
  loadDataFromObject ();
  getDataFromGUI (&d->guiData);

  oldData.name = (d->guiData.name == oldData.name) ? d->data.name : d->guiData.name;
  oldData.priority = (d->guiData.priority == oldData.priority) ? d->data.priority : d->guiData.priority;
  oldData.enabled = (d->guiData.enabled == oldData.enabled) ? d->data.enabled : d->guiData.enabled;
  std::map<QString, int>::iterator it1;
  for (it1 = d->guiData.intValues.begin(); it1 != d->guiData.intValues.end(); ++it1)
    oldData.intValues[it1->first] = (it1->second == oldData.intValues[it1->first]) ? d->data.intValues[it1->first] : it1->second;
  std::map<QString, QString>::iterator it2;
  for (it2 = d->guiData.strValues.begin(); it2 != d->guiData.strValues.end(); ++it2)
    oldData.strValues[it2->first] = (it2->second == oldData.strValues[it2->first]) ? d->data.strValues[it2->first] : it2->second;
  std::map<QString, bool>::iterator it3;;
  for (it3 = d->guiData.boolValues.begin(); it3 != d->guiData.boolValues.end(); ++it3)
    oldData.boolValues[it3->first] = (it3->second == oldData.boolValues[it3->first]) ? d->data.boolValues[it3->first] : it3->second;

  fillGUI (oldData);
}

bool cListEditor::objectValid ()
{
  if (d->obj == 0) return false;
  return (cListManager::self()->objectId (d->obj) != 0);
}

void cListEditor::getGUIData ()
{
  // set some sane default, in case that getDataFromGUI doesn't fill something in
  d->guiData.name = d->data.name;
  d->guiData.enabled = d->data.enabled;
  d->guiData.priority = d->data.priority;
  d->guiData.intValues.clear();
  d->guiData.strValues.clear();
  d->guiData.boolValues.clear();
  getDataFromGUI (&d->guiData);
}

bool cListEditor::changed ()
{
  if (!objectValid()) return false;

  // retrieve data
  getGUIData ();
  cListObject *obj = d->obj;

  // compare data
  if (d->guiData.name != obj->name()) return true;
  if (d->guiData.priority != obj->priority()) return true;
  if (d->guiData.enabled != obj->enabled()) return true;
  std::map<QString, int>::iterator it1;
  for (it1 = d->guiData.intValues.begin(); it1 != d->guiData.intValues.end(); ++it1)
    if (obj->intVal (it1->first) != it1->second)
      return true;
  std::map<QString, QString>::iterator it2;
  for (it2 = d->guiData.strValues.begin(); it2 != d->guiData.strValues.end(); ++it2)
    if (obj->strVal (it2->first) != it2->second)
      return true;
  std::map<QString, bool>::iterator it3;;
  for (it3 = d->guiData.boolValues.begin(); it3 != d->guiData.boolValues.end(); ++it3)
    if (obj->boolVal (it3->first) != it3->second)
      return true;
  return false;  // no difference
}

void cListEditor::saveChanges ()
{
  getGUIData ();
  saveDataToObject ();
  // update our internal data, so that subsequent attempts to undo restore
  // the current status
  loadDataFromObject ();
}

void cListEditor::saveClicked ()
{
  saveChanges ();
}

void cListEditor::undoClicked ()
{
  // TODO: ask the user if he really means it
  fillGUI (d->data);
}

QWidget *cListEditor::createCommonAttribEditor (QWidget *parent)
{
  QWidget *widget = new QWidget (parent);
  QGridLayout *layout = new QGridLayout (widget);
  layout->setSpacing (5);
  d->chkenabled = new QCheckBox (i18n ("&Enabled"), widget);
  d->edpriority = new KIntNumInput (widget);
  d->edpriority->setLabel (i18n ("&Priority"), Qt::AlignLeft);
  d->edpriority->setRange (1, 1000, 1);
  d->edpriority->setSliderEnabled (false);
  d->edname = new KLineEdit (widget);
  d->edname->setValidator (new QRegExpValidator (QRegExp("^[0-9A-Za-z_ ]+$"), this));
  QLabel *label = new QLabel (i18n("&Name"), widget);
  label->setBuddy (d->edname);
  layout->addWidget (d->chkenabled, 0, 0, 1, 2);
  layout->addWidget (d->edpriority, 1, 0, 1, 2);
  layout->addWidget (label, 2, 0);
  layout->addWidget (d->edname, 2, 1);
  return widget;
}

void cListEditor::fillCommonAttribEditor (const cListObjectData &data)
{
  d->chkenabled->setChecked (data.enabled);
  d->edname->setText (data.name);
  d->edpriority->setValue (data.priority);
}

void cListEditor::getDataFromCommonAttribEditor (cListObjectData *data)
{
  data->enabled = d->chkenabled->isChecked ();
  data->name = d->edname->text();
  data->priority = d->edpriority->value ();
}


void cListEditor::loadDataFromObject ()
{
  if (!objectValid()) return;
  d->data = d->obj->data();

  // add default property values
  const std::map<QString, cListProperty> props = d->obj->list()->getPropertyList ();
  std::map<QString, cListProperty>::const_iterator it;
  for (it = props.begin(); it != props.end(); ++it) {
    switch (it->second.type) {
      case Int:
        if (d->data.intValues.count (it->second.name)) break;  // we have a value
        // no value - assign default
        d->data.intValues[it->second.name] = it->second.defIntValue;
        break;
      case String:
        if (d->data.strValues.count (it->second.name)) break;  // we have a value
        // no value - assign default
        d->data.strValues[it->second.name] = it->second.defStrValue;
        break;
      case Bool:
        if (d->data.boolValues.count (it->second.name)) break;  // we have a value
        // no value - assign default
        d->data.boolValues[it->second.name] = it->second.defBoolValue;
        break;
    };
  }
}

void cListEditor::saveDataToObject ()
{
  cListObject *obj = d->obj;
  d->saving = true;
  if (d->guiData.name != obj->name()) {
    if (obj->isGroup()) {
      cListGroup *g = obj->list()->group (d->guiData.name);
      if (g) {
        KMessageBox::sorry (this, i18n ("Can not rename the group, as a group with such name already exists."));
        return;
      }
      obj->list()->renameGroup ((cListGroup *) obj, d->guiData.name);
    } else {
      cListObject *o = obj->list()->getObject (d->guiData.name);
      if (o) {
        KMessageBox::sorry (this, i18n ("Can not rename the object, as an object with such name already exists."));
        return;
      }
      obj->list()->setObjectName (obj, d->guiData.name);
    }
  }
  if (d->guiData.priority != obj->priority())
    obj->setPriority (d->guiData.priority);
  if (d->guiData.enabled != obj->enabled())
    obj->setEnabled (d->guiData.enabled);

  std::map<QString, int>::iterator it1;
  for (it1 = d->guiData.intValues.begin(); it1 != d->guiData.intValues.end(); ++it1)
    obj->setInt (it1->first, it1->second);
  std::map<QString, QString>::iterator it2;
  for (it2 = d->guiData.strValues.begin(); it2 != d->guiData.strValues.end(); ++it2)
    obj->setStr (it2->first, it2->second);
  std::map<QString, bool>::iterator it3;;
  for (it3 = d->guiData.boolValues.begin(); it3 != d->guiData.boolValues.end(); ++it3)
    obj->setBool (it3->first, it3->second);
  d->saving = false;
}


#include "clisteditor.moc"
