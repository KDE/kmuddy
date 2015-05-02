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

/** This method is called by the map manager to save map data.	
  * @param url The url of the file to be saved
  * @return  0 , The file was saved without problems
  *         -1 , Could not open the file
  *         -3 , KIO count not create the file
  */
int CMapFileFilterBase::saveData(const KUrl &url)
{
	int result = -3;
	KTemporaryFile file;

	QString tempfile = file.fileName();
	result = saveData(tempfile);

	if (result == 0)
	{
		if (!KIO::NetAccess::upload(tempfile, url, m_mapManager))
			result = -3;
	}

	return result;
}

/** This method is called by the map manager to load map data.	
  * @param url The url of the file to be loaded
  * @return  0 , The file was loaded without problems
  *         -1 , Could not open the file
  *         -2 , If the file is corrupt
  *         -3 , KIO count not download the file
  */
int CMapFileFilterBase::loadData(const KUrl &url)
{
	int result = -3;
	QString tempfile;

	if (KIO::NetAccess::download(url, tempfile, m_mapManager))
	{
    	result = loadData(tempfile);
	    KIO::NetAccess::removeTempFile(tempfile);
	}

	return result;
}
