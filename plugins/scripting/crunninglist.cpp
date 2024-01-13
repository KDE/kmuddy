/***************************************************************************
                          crunninglist.cpp  -  list of running scripts
                             -------------------
    begin                : Ne dec 22 2002
    copyright            : (C) 2002-2009 by Tomas Mecir
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

#include "crunninglist.h"

#include "cansiparser.h"
#include "coutput.h"
#include "crunningscript.h"
#include "ctextchunk.h"

#include <QAbstractTableModel>
#include <QRegExp>
#include <KLocalizedString>

#include <stdlib.h>

/** Model used in the Running Scripts dialog. */
class cRunningModel: public QAbstractTableModel {
   public:
  cRunningModel (cRunningList *l) :
    QAbstractTableModel (0), lst (l)
  {
  }
  virtual ~cRunningModel () {}

  virtual int columnCount (const QModelIndex &parent = QModelIndex()) const
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return 1;  // we have 1 column
  }

  virtual int rowCount (const QModelIndex &parent = QModelIndex()) const
  {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return lst->scripts.size();
  }

  virtual QVariant headerData ( int section, Qt::Orientation /*orientation*/,
      int role = Qt::DisplayRole ) const
  {
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
      case 0: return i18n ("Name");
      default: return QVariant();
    }
  }

  virtual QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole) const
  {
    // display and user role only
    if ((role != Qt::DisplayRole) && (role != Qt::UserRole)) return QVariant();

    if (index.parent().isValid()) return QVariant();
    int row = index.row();
    int col = index.column();
    if ((col != 0)) return QVariant();
    if ((row < 0) || (row > (int) lst->scripts.size())) return QVariant();

    map<int, cRunningScript *>::iterator it;
    const cRunningScript *entry = 0;
    int cnt = -1;
    for (it = lst->scripts.begin(); it != lst->scripts.end(); ++it) {
      if (++cnt < row) continue;
      entry = it->second;
    }
    if (!entry) return QVariant();

    // user role - return script ID
    if (role == Qt::UserRole) return entry->getId();

    // we want the displayed text - so fetch it and return it
    switch (col) {
      case 0: return entry->name();
      default: return QVariant();
    };
  }

  void resetModel () {
    reset ();
  }

 private:
  cRunningList *lst;
};


cRunningList::cRunningList (int sess) : cActionBase ("runninglist", sess)
{
  scripts.clear ();
  lastid = 0;
  waitCounter = 0;
  waitLock    = false;
  textQueue.clear ();
  typeQueue.clear ();
 
  cANSIParser *ansiparser = dynamic_cast<cANSIParser *>(object ("ansiparser"));
  m_currentFgColor=ansiparser->defaultTextColor();
  m_currentBkColor=ansiparser->defaultBkColor();

  model = new cRunningModel (this);

  //sending text to scripts...
  // react on command-sent, got-line, got-prompt

  addEventHandler ("command-sent", 200, PT_STRING);
  addEventHandler ("got-line", 200, PT_STRING);
  addEventHandler ("got-prompt", 200, PT_STRING);
}

cRunningList::~cRunningList()
{
  removeEventHandler ("command-sent");
  removeEventHandler ("got-line");
  removeEventHandler ("got-prompt");
  
  killAll ();

  delete model;
  model = 0;
}

QAbstractItemModel *cRunningList::getModel ()
{
  return model;
}

void cRunningList::eventStringHandler (QString event, int, QString &par1,
    const QString &)
{
  if (event == "command-sent")
    sendCommand (par1);
  else if (event == "got-line")
    sendServerOutput (par1);
  else if (event == "got-prompt")
    sendPrompt (par1);
}

void cRunningList::addScript (cRunningScript *script)
{
  //connect the script!
  connect (script, SIGNAL (sendText (const QString &)), this,
        SLOT (sendText (const QString &)));
  connect (script, SIGNAL (displayText (const QString &)), this,
        SLOT (displayText (const QString &)));
  connect (script, SIGNAL (scriptFinished (cRunningScript *, int)), this,
        SLOT (scriptFinished (cRunningScript *, int)));
  connect (script, SIGNAL (scriptKilled (cRunningScript *)), this,
        SLOT (scriptKilled (cRunningScript *)));
  connect (script, SIGNAL (scriptFailed (cRunningScript *)), this,
        SLOT (scriptFailed (cRunningScript *)));
  connect (script, SIGNAL (textSent ()), this, SLOT (scriptTextSent ()));
  connect (script, SIGNAL (textAccepted ()), this, SLOT (scriptTextAccepted ()));

  //set script ID
  //FIXME: things may break down if we run out of free IDs!
  //  (id is an 32-bit int)
  script->setId (++lastid);

  //and add it to my list
  scripts[lastid] = script;

  //inform about the change
  model->resetModel();
}

void cRunningList::killAll ()
{
  map<int, cRunningScript *>::iterator it;
  for (it = scripts.begin(); it != scripts.end(); ++it)
    delete it->second;
  scripts.clear();
}

const QString cRunningList::name (int id)
{
  cRunningScript *rs = getRunningScript (id);
  return rs->name();
}

void cRunningList::terminate (int id)
{
  cRunningScript *rs = getRunningScript (id);
  rs->terminate();
}

void cRunningList::kill (int id)
{
  cRunningScript *rs = getRunningScript (id);
  rs->kill();
}

bool cRunningList::requestLock (cRunningScript *script, const QString &varname)
{
  if (locks.count (varname))
  {
    //look who has the lock
    cRunningScript *rs = locks[varname];
    //report success if the caller has the lock :)
    if (script == rs)
      return true;
    //report failure otherwise
    return false;
  }
  else
  {
    //success
    locks[varname] = script;
    return true;
  }
}

void cRunningList::releaseLock (cRunningScript *script, const QString &varname)
{
  if (locks.count (varname))
    if (locks[varname] == script)
      locks.erase (varname);
}

bool cRunningList::canModify (cRunningScript *script, const QString &varname)
{
  if (locks.count (varname))
    return (locks[varname] == script);
  return true;
}

cRunningScript *cRunningList::getRunningScript (int id)
{
  if (!scripts.count (id)) return 0;
  return scripts[id];
}

void cRunningList::removeScript (cRunningScript *script)
{
  //release all locks held by this script
  //we have to do this in two stages, because deleting from a map
  //immediately would invalidate our iterator (yes we could have two
  //iterators, but why bother...)
   //stage 1: obtain a list of all such locks
  list<QString> ourLocks;
  map<QString, cRunningScript *>::iterator it;
  for (it = locks.begin(); it != locks.end(); ++it)
    if (it->second == script)
      ourLocks.push_back (it->first);
   //stage 2: release all the locks
  list<QString>::iterator it2;
  for (it2 = ourLocks.begin(); it2 != ourLocks.end(); ++it2)
    locks.erase (*it2);

  //delete the script
  scripts.erase (script->getId());
  delete script;
}

void cRunningList::scriptFinished (cRunningScript *script, int returnValue)
{
  //display return code, if it is non-zero
  if (returnValue)
    invokeEvent ("message", sess(), "Script " + script->name() +
        " has finished with return code " + QString::number (returnValue) + ".");
  removeScript (script);

  model->resetModel();
}

void cRunningList::scriptKilled (cRunningScript *script)
{
  invokeEvent ("message", sess(), "Script " + script->name() + " has been terminated!");
  removeScript (script);

  model->resetModel();
}

void cRunningList::scriptFailed (cRunningScript *script)
{
  invokeEvent ("message", sess(), "Script " + script->name() + " could not be started!");

  removeScript (script);

  model->resetModel();
}

void cRunningList::sendThisNow (const QString &text, int type, bool noFC)
{
  map<int, cRunningScript *>::iterator it;

  if (noFC)
  {
     for (it = scripts.begin(); it != scripts.end(); ++it)
       //only scripts with the same no-flow-control setting
       if (!it->second->flowControl())
         it->second->sendCommandToScript (text, type);
  } // endif non flow-controlled script
  else
  {
     waitLock = true;  // Lock scriptTextSent() from sending data to scripts

     sendToFlowControlled(text,type);

     QString q_text;
     int     q_type;

     while ((waitCounter  == 0) &&
            (!textQueue.empty())  )
     {
       q_text = textQueue.front ();
       q_type = typeQueue.front ();
       textQueue.pop_front ();
       typeQueue.pop_front ();
       sendToFlowControlled(q_text, q_type);

     } // endwhile last script in list didn't send command and things in textQueue

     waitLock = false; // Allow scriptTextSent() to send data to scripts
  } // endelse flow-controlled script

} // cRunningList::sendThisNow



void cRunningList::sendToFlowControlled(const QString &text, int type)
{
  map<int, cRunningScript *>::iterator it;

  waitCounter = 0;

  // Count number of flow-controlled scripts
  for (it = scripts.begin(); it != scripts.end(); ++it)
    if (it->second->flowControl())
      waitCounter++;

  // Send data to flow-controlled scripts
  for (it = scripts.begin(); it != scripts.end(); ++it)
  {
    if (it->second->flowControl())
    {
      it->second->sendCommandToScript (text, type);

      if (!it->second->actuallySentCommand())
      {
        waitCounter--;
      } // endif no command actually sent to script

    } // endif flow-controlled script

  } // endfor more flow-controlled scripts to count

} // cRunningList::sendToFlowControlled


void cRunningList::sendText (const QString &text)
{
  invokeEvent ("command", sess(), text);
}

QColor * cRunningList::getColor(QString s)
{
    QRegExp r = QRegExp("(\\d+),(\\d+),(\\d+)");
    r.indexIn(s);
    QStringList rgbList = r.capturedTexts();
    int red, green, blue;
    if(rgbList.size()==4) 
    {
        // parse RGB color
        rgbList.pop_front(); //drop complete match
        red = rgbList.first().toInt();
        rgbList.pop_front();
        green = rgbList.first().toInt();
        rgbList.pop_front();
        blue = rgbList.first().toInt();
        if((red>=0)&&(red<256))
            if((green>=0)&&(green<256))
                if((blue>=0)&&(blue<256))
                {
                    QColor * pC = new QColor(red,green,blue);
                    return pC;
                }
    }
    return NULL;
}

void cRunningList::displayText (const QString &text)
{
  cConsole *console = (dynamic_cast<cOutput *>(object ("output")))->console();
  /* cANSIParser *ansiparser = dynamic_cast<cANSIParser *>(object ("ansiparser"));
  cTextChunk *chunk = cTextChunk::makeLine (text,
      ansiparser->defaultTextColor(), ansiparser->defaultBkColor(), console);*/
  cTextChunk *chunk = new cTextChunk(console, text);
  chunkStart start = chunk->startAttr();
  start.fg = m_currentFgColor;
  start.bg = m_currentBkColor;
  chunk->setStartAttr (start);

/* DISABLED - this should go to the /echo command - and likely should be done better
getColor also belongs to this

  bool colorChange = false;
  QColor nextBkColor, nextFgColor;
  for( int i=0, charBehindLastTag=0, startOfCurrentTag=0; i<text.length();i++)
  {
      if(text[i]=='<')      
      {
          startOfCurrentTag=i;
          // minimal tag size = 9 <fg0,0,0> or <bg0,0,0> 
          // maximal tag size = 15 <fg255,255,255> or <bg...>
          if((i+10)<=text.length())
          {
              if(text.mid(i+1,2)=="fg")    
              {
                  int tagEnd = text.indexOf('>',i+4);
                  if((tagEnd>-1)&&(tagEnd<i+15))
                  {
                      QColor * pC = getColor(text.mid(i+3,tagEnd-(i+3)));
                      if(pC!=NULL)
                      {
                          nextFgColor = *pC;
                          i=tagEnd; //+1 from the for loop
                          colorChange = true;
                      }
                  }
              }
              if(text.mid(i+1,2)=="bg")    
              {
                  int tagEnd = text.indexOf('>',i+4);
                  if((tagEnd>-1)&&(tagEnd<i+15))
                  {
                      QColor * pC = getColor(text.mid(i+3,tagEnd-(i+3)));
                      if(pC!=NULL)
                      {
                          nextBkColor = *pC;
                          i=tagEnd; //+1 from the for loop
                          colorChange = true;
                      }
                  }
              }
          }
      }
      if((colorChange)||(i==text.length()))
      { 
          cTextChunk * tmp = 0;
          colorChange = false;
          if((startOfCurrentTag-charBehindLastTag)>=0)
          {
              tmp = new cTextChunk(console,text.mid(charBehindLastTag,startOfCurrentTag-charBehindLastTag));
          }
          if(i==text.length())
          {
              if(!colorChange)
              {   
                  tmp = new cTextChunk(console,text.mid(charBehindLastTag,text.length()-1));
              }
          }

          chunkStart start = tmp->startAttr();
          start.fg = m_currentFgColor;
          start.bg = m_currentBkColor;
          tmp->setStartAttr (start);
          chunk->append(tmp);
          charBehindLastTag = i+1; // with i=endOfCurrentTag
          m_currentFgColor=nextFgColor;
          m_currentBkColor=nextBkColor;
      }
  }
*/

  invokeEvent ("display-line", sess(), chunk);
  delete chunk;  // consumers create a copy of the chunk, so we just delete this
}

void cRunningList::sendCommand (const QString &text)
{
  //manage no-flow-control scripts first
  sendThisNow (text + "\n", USERCOMMAND, true);

  //if we're not waiting for anything
  if ((waitCounter == 0) && (textQueue.empty()))
    sendThisNow (text + "\n", USERCOMMAND);
  else
  {
    textQueue.push_back (text + "\n");
    typeQueue.push_back (USERCOMMAND);
  }
}

void cRunningList::sendPrompt (const QString &text)
{
  //manage no-flow-control scripts first
  sendThisNow (text + "\n", PROMPT, true);

  //if we're not waiting for anything
  if ((waitCounter == 0) && (textQueue.empty()))
    sendThisNow (text + "\n", PROMPT);
  else
  {
    textQueue.push_back (text + "\n");
    typeQueue.push_back (PROMPT);
  }
}

void cRunningList::sendServerOutput (const QString &text)
{
  //manage no-flow-control scripts first
  sendThisNow (text + "\n", SERVEROUTPUT, true);

  //if we're not waiting for anything
  if ((waitCounter == 0) && (textQueue.empty()))
    sendThisNow (text + "\n", SERVEROUTPUT);
  else
  {
    textQueue.push_back (text + "\n");
    typeQueue.push_back (SERVEROUTPUT);
  }
}

void cRunningList::scriptTextAccepted ()
{
//  waitCounter++;
}

void cRunningList::scriptTextSent ()
{
  waitCounter--;

  if (waitLock == true)
  {
    return;
  } // endif sending from textQueue locked off
  else if ((waitCounter == 0) && (!(textQueue.empty())))
  {
    //we're no longer waiting - send next line, if any

    QString text = textQueue.front ();
    int     type = typeQueue.front ();
    textQueue.pop_front ();
    typeQueue.pop_front ();
    sendThisNow (text, type);

  } // endelse allowed to send from textQueue

} // scriptTextSent

#include "moc_crunninglist.cpp"
