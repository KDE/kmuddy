/***************************************************************************
                               cmapcommand.h
                             -------------------
    begin                : Fri Jun 7 2002
    copyright            : (C) 2002 by Kmud Developer Team
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

#ifndef CMAPCOMMAND_H
#define CMAPCOMMAND_H

#include <QUndoCommand>
#include <QString>
#include <kmuddy_export.h>

/**
  *@author Kmud Developer Team
  */

class KMUDDY_EXPORT CMapCommand : public QUndoCommand
{
  public: 
    CMapCommand(QString name);
    ~CMapCommand() override;

    QString name() const { return m_name; }

  private:
    QString m_name;
};

#endif
