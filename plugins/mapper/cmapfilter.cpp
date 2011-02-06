/***************************************************************************
                               cmapfilter.cpp
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by Kmud Developer Team
                           (C) 2007 by Tomas Mecir <kmuddy@kmuddy.net>
    email                : kmud-devel@kmud.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cmapfilter.h"

#include "cmapmanager.h"
#include "cmapviewbase.h"
#include "cmaproom.h"
#include "cmappath.h"

#include <qregexp.h>

CMapFilter::CMapFilter(CMapManager *manager)
{  
  mapManager = manager;
  directionCmdQueue.setAutoDelete(false);
}

CMapFilter::~CMapFilter()
{
}

/** This method is called with when input is sent to the mud */
QString CMapFilter::processCommand (const QString &command)
{
  if (mapManager->validMoveCmd(command))
  {
    bool found = false;

    for (CMapViewBase *view = mapManager->getViewList()->first(); view !=0 ; view = mapManager->getViewList()->next())
      if (view->getFollowMode())
      {
        found = true;
        break;
      }

    if (found)
    {
      QString newStr;
      newStr += executeBeforeCommand (command);
      newStr += command;
      newStr += executeAfterCommand (command);

      if (mapManager->getMapData()->validRoomCheck)
        directionCmdQueue.enqueue(new QString (command));
      else
        mapManager->movePlayerBy(command);
      return newStr;
    }
  }
  return command;
}

QString CMapFilter::executeBeforeCommand (const QString &command)
{
  QString specialCmd = "";
  directionTyp dir = mapManager->textToDirection(command);
  if (dir == SPECIAL)
    specialCmd = command;

  CMapPath *path = mapManager->getCurrentRoom()->getPathDirection(dir,specialCmd);

  if (path)
  {
    QString roomCmd = path->getBeforeCommand();

    if (!roomCmd.trimmed().isEmpty())
      return roomCmd + "\n";
  }
  return QString::null;
}

QString CMapFilter::executeAfterCommand (const QString &command)
{
  QString specialCmd = "";
  directionTyp dir = mapManager->textToDirection(command);
  if (dir == SPECIAL)
    specialCmd = command;

  CMapPath *path = mapManager->getCurrentRoom()->getPathDirection(dir,specialCmd);

  if (path)
  {
    QString roomCmd = path->getAfterCommand();

    if (!roomCmd.trimmed().isEmpty())
      return "\n" + roomCmd;
  }
  return QString::null;
}

/** This method is called when output is sent to the mud */
void CMapFilter::processServerOutput(const QString &s)
{
  QString *dirCmd=directionCmdQueue.dequeue();

  if ((dirCmd!=NULL))
  {
    bool found = false;

    for (CMapViewBase *view = mapManager->getViewList()->first(); view !=0 ; view = mapManager->getViewList()->next())
    {
      if (view->getFollowMode())
      {
        found = true;
        break;
      }
    }

    if (found)
    {
      bool movePlayer = true;

      QStringList::iterator it;
      for (it = mapManager->getMapData()->failedMoveMsg.begin();
           it != mapManager->getMapData()->failedMoveMsg.end(); ++it)
      {
        QString str = *it;
        if (!str.isEmpty())
        {
          QRegExp r(str);
          
          if ( r.indexIn(s) != -1)
          {
            movePlayer = false;
            break;
          }
        }

      }

      if (movePlayer)
        mapManager->movePlayerBy(*dirCmd);
    }

    delete dirCmd;
  }
}

