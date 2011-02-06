/***************************************************************************
                          csaveablefield.cpp  -  alias/trigger baseclass
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So sep 7 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#include "csaveablefield.h"

#include <kconfig.h>

cSaveableField::cSaveableField ()
{
  prev = NULL;
  next = NULL;
  globalmatch = false;
}

cSaveableField::~cSaveableField ()
{
}

void cSaveableField::setCond (const QString &c)
{
  condition = c;
}


