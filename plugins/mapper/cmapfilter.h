/***************************************************************************
                               cmapfilter.h
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

#ifndef CMAPFILTER_H
#define CMAPFILTER_H

#include <QString>
#include <QStringList>

class CMapManager;

/**This is used to process the input/output of the mud for the mapper
  *@author Kmud Developer Team
  */

class CMapFilter
{
public:
  CMapFilter(CMapManager *manager);
  ~CMapFilter();

  /** This method is called with when input is sent to the mud */
  QString processCommand (const QString &);
  /** This method is called when text comes from the server */
  void processServerOutput (const QString &);

private:
  QString executeBeforeCommand (const QString &command);
  QString executeAfterCommand (const QString &command);

private:
  CMapManager *mapManager;
  QStringList directionCmdQueue;
};

#endif
