//
// Description: All core internal macros and functions.
//     Also includes (un)registerInternalMacros
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, 2005-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cmacromanager.h"
#include "cactionmanager.h"
#include "ccmdqueue.h"
#include "ccmdprocessor.h"
#include "clist.h"
#include "clistgroup.h"
#include "clistmanager.h"
#include "ctimer.h"
#include "cvariablelist.h"

#include <klocale.h>

using namespace std;

class cMacroSet : public cMacro {
public:
  cMacroSet () : cMacro ("set") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = params.section (' ', 1, -1, QString::SectionSkipEmpty);
    //no variable expansion for our parameter, to allow various tricks :)
    varList(sess)->set (varname, value);
  }
};

class cMacroUnset : public cMacro {
public:
  cMacroUnset () : cMacro ("unset") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    varList(sess)->unset (varname);
  }
};

class cMacroSetVal : public cMacro {
  public:
    cMacroSetVal () : cMacro ("setval") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      // this is like /set, but the value gets parsed using as expression
      cCmdProcessor *cmdproc = dynamic_cast<cCmdProcessor *>(am->object ("cmdprocessor", sess));
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, -1, QString::SectionSkipEmpty);
      bool ok;
      cValue val = cmdproc->eval (value, queue, ok);
      if (!ok) {
        am->invokeEvent ("message", sess, i18n ("/setval: Expression parsing failed."));
        return;
      }
      varList(sess)->set (varname, &val);
    }
};

class cMacroLSet : public cMacro {
  public:
    cMacroLSet () : cMacro ("lset") {}
    virtual void eval (const QString &params, int, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, -1, QString::SectionSkipEmpty);
      //no variable expansion for our parameter, to allow various tricks :)
      queue->setValue (varname, value);
    }
};

class cMacroLUnset : public cMacro {
  public:
    cMacroLUnset () : cMacro ("lunset") {}
    virtual void eval (const QString &params, int, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      queue->delValue (varname);
    }
};

class cMacroInc : public cMacro {
public:
  cMacroInc () : cMacro ("inc") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    bool ok = false;
    double val = value.toDouble(&ok);
    if (ok) varList(sess)->inc (varname, val);
  }
};

class cMacroDec : public cMacro {
public:
  cMacroDec () : cMacro ("dec") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString value = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    bool ok = false;
    double val = value.toDouble(&ok);
    if (ok) varList(sess)->dec (varname, val);
  }
};

class cMacroLInc : public cMacro {
  public:
    cMacroLInc () : cMacro ("linc") {}
    virtual void eval (const QString &params, int, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, 1, QString::SectionSkipEmpty);
      bool ok = false;
      double val = value.toDouble(&ok);
      if (ok) {
        cValue *v = queue->value (varname);
        if (!v)  // no such variable ?
          queue->setValue (varname, cValue (val));
        else
          queue->setValue (varname, cValue (v->asDouble() + val));
      }
    }
};

class cMacroLDec : public cMacro {
  public:
    cMacroLDec () : cMacro ("ldec") {}
    virtual void eval (const QString &params, int, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, 1, QString::SectionSkipEmpty);
      bool ok = false;
      double val = value.toDouble(&ok);
      if (ok) {
        cValue *v = queue->value (varname);
        if (!v)  // no such variable ?
          queue->setValue (varname, cValue (-val));
        else
          queue->setValue (varname, cValue (v->asDouble() - val));
      }
    }
};

class cMacroProvideRes : public cMacro {
public:
  cMacroProvideRes () : cMacro ("provide-res") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    varList(sess)->provideResource (varname);
  }
};

class cMacroRequestRes : public cMacro {
public:
  cMacroRequestRes () : cMacro ("request-res") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    bool ret = varList(sess)->requestResource (varname);
    if (!ret)
      am->invokeEvent ("message", sess, i18n ("No free resources of that type."));
  }
};

class cMacroEcho : public cMacro {
public:
  cMacroEcho () : cMacro ("echo") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
    if( !params.isEmpty() )
    {
      QString txt = expandVariables (params, sess, queue);
      am->invokeEvent ("user-message", sess, txt);
    }
  }
};

class cMacroTick : public cMacro {
public:
  cMacroTick () : cMacro ("tick") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString objName = params;
    if (!objName.length()) {
      am->invokeEvent ("message", sess, i18n ("/tick: syntax: /tick timer-name"));
      return;
    }
    cList *list = cListManager::self()->getList (sess, "timers");
    if (!list) return;  // should never happen
    // get the timer object
    cTimer *timer = dynamic_cast<cTimer *>(list->getObject (objName));
    if (!timer) {
      am->invokeEvent ("message", sess, i18n ("/tick: a timer with that name does not exist."));
      return;
    }
    if (!timer->enabled()) {
      am->invokeEvent ("message", sess, i18n ("/tick: this timer is currently disabled."));
      return;
    }
    int time = timer->tickTimeout ();
    if (time > 0)
      am->invokeEvent ("message", sess, i18n ("Timer ticks in ") + QString::number(time) +
          ((time > 1) ? i18n (" seconds.") : i18n (" second.")));
    else
      am->invokeEvent ("message", sess, i18n ("Timer ticks right now."));
  }
};

class cMacroTickNow : public cMacro {
public:
  cMacroTickNow () : cMacro ("ticknow") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString objName = params;
    if (!objName.length()) {
      am->invokeEvent ("message", sess, i18n ("/ticknow: syntax: /ticknow timer-name"));
      return;
    }
    cList *list = cListManager::self()->getList (sess, "timers");
    if (!list) return;  // should never happen
    // get the timer object
    cTimer *timer = dynamic_cast<cTimer *>(list->getObject (objName));
    if (!timer) {
      am->invokeEvent ("message", sess, i18n ("/tick: a timer with that name does not exist."));
      return;
    }
    if (!timer->enabled()) {
      am->invokeEvent ("message", sess, i18n ("/tick: this timer is currently disabled."));
      return;
    }
    timer->execute ();
  }
};

class cMacroAddItem : public cMacro {
  public:
    cMacroAddItem () : cMacro ("additem") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, -1, QString::SectionSkipEmpty);
      varList(sess)->valueNotEmpty(varname, queue)->addToList (value);
      // TODO: report variable change ... same for the next three macros ...
    }
};

class cMacroDelItem : public cMacro {
  public:
    cMacroDelItem () : cMacro ("delitem") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString value = params.section (' ', 1, -1, QString::SectionSkipEmpty);
      cValue *val = varList(sess)->value(varname, queue);
      if (val) val->removeFromList (value);
    }
};

class cMacroArraySet : public cMacro {
  public:
    cMacroArraySet () : cMacro ("arrayset") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString index = params.section (' ', 1, 1, QString::SectionSkipEmpty);
      QString value = params.section (' ', 2, -1, QString::SectionSkipEmpty);
      bool ok;
      int idx = index.toInt (&ok);
      if (!ok) return;
      varList(sess)->valueNotEmpty(varname, queue)->setItem(idx, value);
    }
};

class cMacroArrayDel : public cMacro {
  public:
    cMacroArrayDel () : cMacro ("arraydel") {}
    virtual void eval (const QString &params, int sess, cCmdQueue *queue) {
      QString varname = params.section (' ', 0, 0, QString::SectionSkipEmpty);
      QString index = params.section (' ', 1, 1, QString::SectionSkipEmpty);
      bool ok;
      int idx = index.toInt (&ok);
      if (!ok) return;
      cValue *val = varList(sess)->value(varname, queue);
      if (val) val->removeItem (idx);
    }
};


class cMacroIf : public cMacro {
public:
  cMacroIf () : cMacro ("if") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *queue)
  {
    // we should perform matching (else preproc would fail) - true if non-zero
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    par1 = expandVariables (par1, sess, queue);
    bool match = (cValue(par1).asInteger () != 0);
    
    cExecStack *es = queue->execStack ("if");
    
    // first /if ? register as preproc command
    if (es->empty())
      queue->addPreprocessMacro (this);

    // put value to the exec-stack
    cExecStackItem item;
    item.setAttrib ("match", match ? 1 : 0);
    es->push (item);
  }

  virtual bool preprocess (cCmdQueue *queue, cCmdQueueEntry *qe)
  {
    cExecStack *es = queue->execStack ("if");
    
    // do nothing if the stack is empty
    if (es->empty()) return true;

    // if the top value on the stack has non-zero match, we should execute the command
    cExecStackItem item = es->top ();
    if (item.attrib ("match")) return true;
    
    // match is 0, we (probably) won't execute ...
    // we need to check for nested if-s first
    // anything else than if, else, endif - simply return false
    if (!qe->isMacro) return false;
    cExecStack *es2 = queue->execStack ("if-ignore");
    cExecStackItem item2;
    if (qe->macroName == "if")
    {
      // nested ignored if
      // put a value to if-ignore stack
      es2->push (item2);
      return false;
    }
    else if (qe->macroName == "else")
    {
      // End of if branch, execute this command, unless the if-ignore stack is non-empty.
      // If the if-ignore stack is empty, we want to exec /else.
      return es2->empty();
    }
    else if (qe->macroName == "endif")
    {
      // If we ignore some if (if-ignore stack is not empty), we pop one item from there
      // and return false. Otherwise, we return true - want to exec /endif.
      if (es2->empty())
        return true;
      es2->pop ();
      return false;
    }
    else return false;   // ignoring all other macros
  };
};

class cMacroElse : public cMacro {
public:
  cMacroElse () : cMacro ("else") {}
  virtual void eval (const QString &, int sess, cCmdQueue *queue) {
    cExecStack *es = queue->execStack ("if");
    // display some error if the stack is empty
    if (es->empty()) {
      am->invokeEvent ("message", sess, i18n ("/else: called without the matching /if."));
      return;
    }
    // remove one if from the stack, switch its "match" value and push it back
    cExecStackItem item = es->pop ();
    item.setAttrib ("match", !(item.attrib ("match")));
    es->push (item);
  }
};

class cMacroEndIf : public cMacro {
public:
  cMacroEndIf () : cMacro ("endif") {}
  virtual void eval (const QString &, int sess, cCmdQueue *queue) {
    cExecStack *es = queue->execStack ("if");
    // display some error if the stack is empty
    if (es->empty()) {
      am->invokeEvent ("message", sess, i18n ("/endif: called without the matching /if."));
      return;
    }
    // remove one if from the stack
    es->pop ();
    // unregister the if preprocess macro, if we're not in any /if (ie., stack empty)
    if (es->empty ())
      queue->removePreprocessMacro (cMacroManager::self()->macro ("if"));
  }
};

class cMacroOAdd : public cMacro {
public:
  cMacroOAdd () : cMacro ("oadd") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, -1, QString::SectionSkipEmpty);
    if (par1.isEmpty() || par2.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/oadd: syntax: /oadd parent name"));
      return;
    }

    cListManager *lm = cListManager::self();
    int objId = cValue (par1).asInteger();
    cListObject *group = lm->object (objId);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/oadd: parent group does not exist."));
      return;
    }
    if (!group->isGroup()) {
      am->invokeEvent ("message", sess, i18n ("/oadd: parent must be a group."));
      return;
    }
    cList *list = group->list ();
    if (list->getObject (par2)) {
      am->invokeEvent ("message", sess, i18n ("/oadd: object with this name already exists."));
      return;
    }
    
    cListObject *obj = list->newObject();
    list->addToGroup ((cListGroup *) group, obj);
    list->setObjectName (obj, par2);
  }
};

class cMacroGAdd : public cMacro {
public:
  cMacroGAdd () : cMacro ("gadd") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, -1, QString::SectionSkipEmpty);
    if (par2.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/gadd: syntax: /gadd parent name"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/gadd: parent group does not exist."));
      return;
    }
    if (!obj->isGroup()) {
      am->invokeEvent ("message", sess, i18n ("/gadd: the parent must be a group"));
      return;
    }

    cList *list = obj->list();
    if (list->group (par2)) {
      am->invokeEvent ("message", sess, i18n ("/gadd: such group already exists in that list"));
      return;
    }
    list->addGroup ((cListGroup *) obj, par2);
  }
};

class cMacroODel : public cMacro {
public:
  cMacroODel () : cMacro ("odel") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/odel: object does not exist."));
      return;
    }
    cList *list = obj->list();
    if (obj == list->rootGroup()) {
      am->invokeEvent ("message", sess, i18n ("/odel: root group may not be deleted."));
      return;
    }
    if (obj->isGroup())
      list->removeGroup ((cListGroup *) obj);
    else
      list->deleteObject (obj);
  }
};

class cMacroOMove : public cMacro {
public:
  cMacroOMove () : cMacro ("omove") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    QString par3 = params.section (' ', 2, 2, QString::SectionSkipEmpty);
    if (par2.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/omove: syntax: /omove object group [position]"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/omove: object does not exist."));
      return;
    }
    if (obj == obj->list()->rootGroup()) {
      am->invokeEvent ("message", sess, i18n ("/omove: root group may not be moved."));
      return;
    }
    objId = cValue (par2).asInteger();
    cListObject *group = lm->object (objId);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/omove: group does not exist."));
      return;
    }
    if (!group->isGroup()) {
      am->invokeEvent ("message", sess, i18n ("/omove: target must be a group."));
      return;
    }
    if (obj->list() != group->list()) {
      am->invokeEvent ("message", sess, i18n ("/omove: object and target must be in the same list."));
      return;
    }

    cList *list = obj->list();
    list->addToGroup ((cListGroup *) group, obj);

    if (!par3.isEmpty()) {
      // position is given
      int pos = cValue (par3).asInteger();
      cListGroup *g = (cListGroup *) group;
      if ((pos < 1) || (pos > g->objectCount())) {
        am->invokeEvent ("message", sess, i18n ("/omove: position out of range"));
        return;
      }
      g->moveObjectToPosition (obj, pos - 1);
    }
  }
};

class cMacroORen : public cMacro {
public:
  cMacroORen () : cMacro ("oren") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, -1, QString::SectionSkipEmpty);
    if (par2.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/oren: syntax: /oren object new-name"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/oren: object does not exist."));
      return;
    }

    cList *list = obj->list();
    if (obj == list->rootGroup()) {
      am->invokeEvent ("message", sess, i18n ("/oren: root group cannot be renamed."));
      return;
    }
    if (obj->isGroup()) {
      cListGroup *g = list->group (par2);
      if (g && (g != obj)) {
        am->invokeEvent ("message", sess, i18n ("/oren: a group with this name already exists."));
        return;
      }
      list->renameGroup ((cListGroup *) obj, par2);
    } else {
      cListObject *o = list->getObject (par2);
      if (o && (o != obj)) {
        am->invokeEvent ("message", sess, i18n ("/oren: an object with this name already exists."));
        return;
      }
      list->setObjectName (obj, par2);
    }
  }
};

class cMacroOPriority : public cMacro {
public:
  cMacroOPriority () : cMacro ("opriority") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    if (par2.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/opriority: syntax: /opriority object priority"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/opriority: object does not exist."));
      return;
    }

    int priority = cValue (par2).asInteger();
    if ((priority < 1) || (priority > 1000)) {
      am->invokeEvent ("message", sess, i18n ("/opriority: priority must be in the 1-1000 range."));
      return;
    }
    obj->setPriority (priority);
  }
};

class cMacroOEnable : public cMacro {
public:
  cMacroOEnable () : cMacro ("oenable") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/oenable: object does not exist."));
      return;
    }
    obj->setEnabled (true);
  }
};

class cMacroODisable : public cMacro {
public:
  cMacroODisable () : cMacro ("odisable") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/odisable: object does not exist."));
      return;
    }
    obj->setEnabled (false);
  }
};

class cMacroOSetBAttr : public cMacro {
public:
  cMacroOSetBAttr () : cMacro ("osetbattr") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    QString par3 = params.section (' ', 2, 2, QString::SectionSkipEmpty);
    if (par2.isEmpty() || par3.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/osetbattr: syntax: /osetbattr object parameter value"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/osetbattr: object does not exist."));
      return;
    }

    bool val = (cValue (par3).asInteger() != 0);
    obj->setBool (par2, val);
  }
};

class cMacroOSetIAttr : public cMacro {
public:
  cMacroOSetIAttr () : cMacro ("osetiattr") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    QString par3 = params.section (' ', 2, 2, QString::SectionSkipEmpty);
    if (par2.isEmpty() || par3.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/osetiattr: syntax: /osetbattr object parameter value"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/osetiattr: object does not exist."));
      return;
    }

    int val = cValue (par3).asInteger();
    obj->setInt (par2, val);
  }
};

class cMacroOSetSAttr : public cMacro {
public:
  cMacroOSetSAttr () : cMacro ("osetsattr") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    QString par1 = params.section (' ', 0, 0, QString::SectionSkipEmpty);
    QString par2 = params.section (' ', 1, 1, QString::SectionSkipEmpty);
    QString par3 = params.section (' ', 2, -1, QString::SectionSkipEmpty);
    if (par2.isEmpty() || par3.isEmpty()) {
      am->invokeEvent ("message", sess, i18n ("/osetiattr: syntax: /osetbattr object parameter value"));
      return;
    }
    int objId = cValue (par1).asInteger();
    cListManager *lm = cListManager::self();
    cListObject *obj = lm->object (objId);
    if (!obj) {
      am->invokeEvent ("message", sess, i18n ("/osetsattr: object does not exist."));
      return;
    }

    obj->setStr (par2, par3);
  }
};

class cMacroTGroupOn : public cMacro {
public:
  cMacroTGroupOn () : cMacro ("tgroupon") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    cListManager *lm = cListManager::self();
    cList *list = lm->getList (sess, "triggers");
    if (!list) return;
    cListGroup *group = list->group (params);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/tgroupon: group does not exist."));
      return;
    }
    group->setEnabled (true);
  }
};
class cMacroTGroupOff : public cMacro {
public:
  cMacroTGroupOff () : cMacro ("tgroupoff") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    cListManager *lm = cListManager::self();
    cList *list = lm->getList (sess, "triggers");
    if (!list) return;
    cListGroup *group = list->group (params);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/tgroupoff: group does not exist."));
      return;
    }
    group->setEnabled (false);
  }
};

class cMacroAGroupOn : public cMacro {
public:
  cMacroAGroupOn () : cMacro ("agroupon") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    cListManager *lm = cListManager::self();
    cList *list = lm->getList (sess, "aliases");
    if (!list) return;
    cListGroup *group = list->group (params);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/agroupon: group does not exist."));
      return;
    }
    group->setEnabled (true);
  }
};

class cMacroAGroupOff : public cMacro {
public:
  cMacroAGroupOff () : cMacro ("agroupoff") {}
  virtual void eval (const QString &params, int sess, cCmdQueue *) {
    cListManager *lm = cListManager::self();
    cList *list = lm->getList (sess, "aliases");
    if (!list) return;
    cListGroup *group = list->group (params);
    if (!group) {
      am->invokeEvent ("message", sess, i18n ("/agroupoff: group does not exist."));
      return;
    }
    group->setEnabled (false);
  }
};


class cFunctionContains: public cFunction {
public:
  cFunctionContains () : cFunction ("contains") {}
  virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
    if (params.size() < 2) return cValue::empty();
    std::list<cValue>::iterator it = params.begin();
    cValue lst = *it;
    ++it;
    cValue val = *it;
    if (lst.isList())
      return cValue (lst.contains (val.asString()) ? 1 : 0);
    // not a list ? Doesn't contain it then !
    return cValue::empty();
  }
};

class cFunctionItem: public cFunction {
public:
  cFunctionItem () : cFunction ("item") {}
  virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
    if (params.size() < 2) return cValue::empty();
    std::list<cValue>::iterator it = params.begin();
    cValue lst = *it;
    ++it;
    cValue val = *it;
    if (lst.isArray())
      return cValue (lst.item (val.asInteger()));
    // not an array ? Not applicable then !
    return cValue::empty();
  }
};

class cFunctionCount: public cFunction {
  public:
    cFunctionCount () : cFunction ("count") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      int count = 0;
      std::list<cValue>::iterator it = params.begin();
      for (; it != params.end(); ++it) {
        count += (*it).size();
      }
      return cValue (count);
    }
};

class cFunctionGlobal: public cFunction {
  public:
    cFunctionGlobal () : cFunction ("global") {}
    virtual cValue eval (std::list<cValue> &params, int sess, cCmdQueue *) {
      if (params.size() == 0) return cValue::empty();
      QString varName = (*params.begin()).asString();
      cValue *val = varList(sess)->value (varName, 0);
      if (val) return *val;
      return cValue::empty();
    }
};

class cFunctionLocal: public cFunction {
  public:
    cFunctionLocal () : cFunction ("local") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *queue) {
      if ((params.size() == 0) || (!queue)) return cValue::empty();
      QString varName = (*params.begin()).asString();
      cValue *val = queue->value (varName);
      if (val) return *val;
      return cValue::empty();
    }
};

class cFunctionAttrib: public cFunction {
  public:
    cFunctionAttrib () : cFunction ("attrib") {}
    virtual cValue eval (std::list<cValue> &params, int sess, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString object = (*it).asString();
      ++it;
      QString attrib = (*it).asString();
      return cValue (am->object (object, sess)->attrib (attrib));
    }
};

class cFunctionStrAttrib: public cFunction {
  public:
    cFunctionStrAttrib () : cFunction ("strattrib") {}
    virtual cValue eval (std::list<cValue> &params, int sess, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString object = (*it).asString();
      ++it;
      QString attrib = (*it).asString();
      return cValue (am->object (object, sess)->strAttrib (attrib));
    }
};

class cFunctionObject: public cFunction {
  public:
    cFunctionObject () : cFunction ("object") {}
    virtual cValue eval (std::list<cValue> &params, int sess, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString listName = (*it).asString();
      ++it;
      QString objName = (*it).asString();

      cListManager *lm = cListManager::self();
      cList *list = lm->getList (sess, listName);
      if (!list) return cValue(0);
      cListObject *obj = list->getObject (objName);
      if (!obj) return cValue(0);
      return cValue (lm->objectId (obj));
    }
};

class cFunctionGroup: public cFunction {
  public:
    cFunctionGroup () : cFunction ("group") {}
    virtual cValue eval (std::list<cValue> &params, int sess, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString listName = (*it).asString();
      ++it;
      QString objName = (*it).asString();

      cListManager *lm = cListManager::self();
      cList *list = lm->getList (sess, listName);
      if (!list) return cValue(0);
      cListObject *obj = list->group (objName);
      if (!obj) return cValue(0);
      return cValue (lm->objectId (obj));
    }
};

class cFunctionOBAttr: public cFunction {
  public:
    cFunctionOBAttr () : cFunction ("obattr") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      int id = (*it).asInteger();
      ++it;
      QString attrName = (*it).asString();

      cListManager *lm = cListManager::self();
      cListObject *obj = lm->object (id);
      if (!obj) return cValue::empty();
      return cValue (obj->boolVal (attrName));
    }
};

class cFunctionOIAttr: public cFunction {
  public:
    cFunctionOIAttr () : cFunction ("oiattr") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      int id = (*it).asInteger();
      ++it;
      QString attrName = (*it).asString();

      cListManager *lm = cListManager::self();
      cListObject *obj = lm->object (id);
      if (!obj) return cValue::empty();
      return cValue (obj->intVal (attrName));
    }
};

class cFunctionOSAttr: public cFunction {
  public:
    cFunctionOSAttr () : cFunction ("osattr") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      int id = (*it).asInteger();
      ++it;
      QString attrName = (*it).asString();

      cListManager *lm = cListManager::self();
      cListObject *obj = lm->object (id);
      if (!obj) return cValue::empty();
      return cValue (obj->strVal (attrName));
    }
};


/*************************************************************/
/***    registerInternalMacros, unregisterInternalMacros   ***/
/*************************************************************/


static cMacroSet *mset;
static cMacroUnset *munset;
static cMacroSetVal *msetval;
static cMacroLSet *mlset;
static cMacroLUnset *mlunset;
static cMacroInc *minc;
static cMacroDec *mdec;
static cMacroLInc *mlinc;
static cMacroLDec *mldec;
static cMacroProvideRes *mprovideres;
static cMacroRequestRes *mrequestres;
static cMacroEcho *mecho;
static cMacroTick *mtick;
static cMacroTickNow *mticknow;
static cMacroAddItem *madditem;
static cMacroDelItem *mdelitem;
static cMacroArraySet *marrayset;
static cMacroArrayDel *marraydel;
static cMacroIf *mif;
static cMacroElse *melse;
static cMacroEndIf *mendif;
static cMacroTGroupOn *mtgroupon;
static cMacroTGroupOff *mtgroupoff;
static cMacroAGroupOn *magroupon;
static cMacroAGroupOff *magroupoff;

static cMacroOAdd *moadd;
static cMacroGAdd *mgadd;
static cMacroODel *model;
static cMacroOMove *momove;
static cMacroORen *moren;
static cMacroOPriority *mopriority;
static cMacroOEnable *moenable;
static cMacroODisable *modisable;
static cMacroOSetBAttr *mosetbattr;
static cMacroOSetIAttr *mosetiattr;
static cMacroOSetSAttr *mosetsattr;

static cFunctionContains *fcontains;
static cFunctionItem *fitem;
static cFunctionCount *fcount;
static cFunctionGlobal *fglobal;
static cFunctionLocal *flocal;
static cFunctionAttrib *fattrib;
static cFunctionStrAttrib *fstrattrib;

static cFunctionObject *fobject;
static cFunctionGroup *fgroup;
static cFunctionOBAttr *fobattr;
static cFunctionOIAttr *foiattr;
static cFunctionOSAttr *fosattr;

void registerInternalMacros ()
{
  mset = new cMacroSet;
  munset = new cMacroUnset;
  msetval = new cMacroSetVal;
  mlset = new cMacroLSet;
  mlunset = new cMacroLUnset;
  minc = new cMacroInc;
  mdec = new cMacroDec;
  mlinc = new cMacroLInc;
  mldec = new cMacroLDec;
  mprovideres = new cMacroProvideRes;
  mrequestres = new cMacroRequestRes;
  mecho = new cMacroEcho;
  mtick = new cMacroTick;
  mticknow = new cMacroTickNow;
  madditem = new cMacroAddItem;
  mdelitem = new cMacroDelItem;
  marrayset = new cMacroArraySet;
  marraydel = new cMacroArrayDel;
  mif = new cMacroIf;
  melse = new cMacroElse;
  mendif = new cMacroEndIf;

  moadd = new cMacroOAdd;
  mgadd = new cMacroGAdd;
  model = new cMacroODel;
  momove = new cMacroOMove;
  moren = new cMacroORen;
  mopriority = new cMacroOPriority;
  moenable = new cMacroOEnable;
  modisable = new cMacroODisable;
  mosetbattr = new cMacroOSetBAttr;
  mosetiattr = new cMacroOSetIAttr;
  mosetsattr = new cMacroOSetSAttr;

  mtgroupon = new cMacroTGroupOn;
  mtgroupoff = new cMacroTGroupOff;
  magroupon = new cMacroAGroupOn;
  magroupoff = new cMacroAGroupOff;

  fcontains = new cFunctionContains;
  fitem = new cFunctionItem;
  fcount = new cFunctionCount;
  fglobal = new cFunctionGlobal;
  flocal = new cFunctionLocal;
  fattrib = new cFunctionAttrib;
  fstrattrib = new cFunctionStrAttrib;

  fobject = new cFunctionObject;
  fgroup = new cFunctionGroup;
  fobattr = new cFunctionOBAttr;
  foiattr = new cFunctionOIAttr;
  fosattr = new cFunctionOSAttr;

}

void unregisterInternalMacros ()
{
  delete mset;
  delete munset;
  delete msetval;
  delete mlset;
  delete mlunset;
  delete minc;
  delete mdec;
  delete mlinc;
  delete mldec;
  delete mprovideres;
  delete mrequestres;
  delete mecho;
  delete mtick;
  delete mticknow;
  delete madditem;
  delete mdelitem;
  delete marrayset;
  delete marraydel;
  delete mif;
  delete melse;
  delete mendif;

  delete moadd;
  delete mgadd;
  delete model;
  delete momove;
  delete moren;
  delete moenable;
  delete modisable;
  delete mopriority;
  delete mosetbattr;
  delete mosetiattr;
  delete mosetsattr;

  delete mtgroupon;
  delete mtgroupoff;
  delete magroupon;
  delete magroupoff;

  delete fcontains;
  delete fitem;
  delete fcount;
  delete fglobal;
  delete flocal;
  delete fattrib;
  delete fstrattrib;

  delete fobject;
  delete fgroup;
  delete fobattr;
  delete foiattr;
  delete fosattr;
}

