/***************************************************************************
                               cmapcommand.cpp
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

#include "cmapcommand.h"

#include <kapplication.h>


CMapCommand::CMapCommand(QString name) : QUndoCommand()

{
	m_name = name;
}

CMapCommand::~CMapCommand()
{
}

