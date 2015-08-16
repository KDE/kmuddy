/***************************************************************************
                               cmapfilefilterbase.cpp
                             -------------------
    begin                : Mon May 27 2002
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

#include "cmapfilefilterbase.h"

#include "cmapmanager.h"

#include <ktemporaryfile.h>
#include <kio/netaccess.h>
#include <kio/jobclasses.h>

#include <qstring.h>

CMapFileFilterBase::CMapFileFilterBase(CMapManager *manager)
{
	m_mapManager = manager;
}

CMapFileFilterBase::~CMapFileFilterBase()
{
}


