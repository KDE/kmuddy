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
#include "cmapview.h"
#include "cmaproom.h"
#include "cmappath.h"

#include <qregexp.h>

CMapFilter::CMapFilter(CMapManager *manager)
{  
  mapManager = manager;
}

CMapFilter::~CMapFilter()
{
}

/** This method is called with when input is sent to the mud */
QString CMapFilter::processCommand (const QString &command)
{
  if (mapManager->validMoveCmd(command))
  {
    CMapView *view = mapManager->getActiveView();
    if (view->getFollowMode())
    {
      QString newStr;
      newStr += executeBeforeCommand (command);
      newStr += command;
      newStr += executeAfterCommand (command);

      if (mapManager->getMapData()->validRoomCheck)
        directionCmdQueue.append (command);
      else
        mapManager->movePlayerBy(command);
      return newStr;
    }
  }
  return command;
}

QString CMapFilter::executeBeforeCommand (const QString &command)
{
  QString specialCmd;
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
  return QString();
}

QString CMapFilter::executeAfterCommand (const QString &command)
{
  QString specialCmd;
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
  return QString();
}

/** This method is called when output is sent to the mud */
void CMapFilter::processServerOutput(const QString &s)
{
  if (!directionCmdQueue.isEmpty()) {
     QString dirCmd = directionCmdQueue.takeFirst();

    if (mapManager->getActiveView()->getFollowMode())
    {
      bool movePlayer = true;

      QStringList::iterator it;
      for (it = mapManager->getMapData()->failedMoveMsg.begin();
           it != mapManager->getMapData()->failedMoveMsg.end(); ++it)
      {
        QString str = *it;
        if (!str.isEmpty())
        {
          QRegularExpression r(str);
          auto res = r.match(s);
          if (res.hasMatch())
          {
            movePlayer = false;
            break;
          }
        }

      }

      if (movePlayer)
        mapManager->movePlayerBy(dirCmd);
    }
  }
}

