/***************************************************************************
                               kmemconfig.h
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

#ifndef KMEMCONFIG_H
#define KMEMCONFIG_H

#include <kconfig.h>

/**
  This is a tiny wrapper around KConfig that initialises it with no file name,
  so that it operates in memory only. Used to be a full implementation of a
  memory-based storage, but this no longer seems necessary.
  */
class KMemConfig : public KConfig
{
public: 
  KMemConfig();
};

#endif
