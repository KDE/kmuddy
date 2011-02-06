/***************************************************************************
                               kmemconfig.cpp
                             -------------------
    begin                : Thu Jun 13 2002
    copyright            : (C) 2002 by Kmud Developer Team
    copyright            : (C) 2008 by Tomas Mecir
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

#include "kmemconfig.h"

KMemConfig::KMemConfig() : KConfig (QString(), KConfig::SimpleConfig)
{
}

