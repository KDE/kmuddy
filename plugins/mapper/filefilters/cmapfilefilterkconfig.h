/***************************************************************************
                               cmapfilefilterkconfig.h
                             -------------------
    begin                : Wed Aug 7 2002
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

#ifndef CMAPFILEFILTERKCONFIG_H
#define CMAPFILEFILTERKCONFIG_H

#include "../cmapfilefilterbase.h"
#include "../cmapelement.h"

#include <qfile.h>
#include <qstring.h>
#include <kvbox.h>

class CMapManager;

/**
  *@author Kmud Developer Team
  */

class CMapFileFilterKConfig : public CMapFileFilterBase
{
public: 
	CMapFileFilterKConfig(CMapManager *manager);
	~CMapFileFilterKConfig();

	/** This returns name of the import/export filter. This should be kept small
      * @return The Name of the filter */
	QString getName(void);
	/** This returns a discription of the import/export filter
      * @return The discription */
	QString getDescription(void);
	/** This returns the extension  of the filename that will be loaded,created
      * @return The exstension */
	QString getExtension(void);
	/** This returns the pattern extension of the filename that will be loaded,created
      * @return The exstension */
	QString getPatternExtension(void);


	/** This method will return true or false depending on if it's a export filter
      * @return True if this is a export filter, otherwise false */
	bool supportSave(void);
	/** This method will return true or false depending on if it's a import filter
      * @return True if this is a import filter, otherwise false */
	bool supportLoad(void);

protected:
	/** This method should be reimplemented if this is a to be a export filter. It
	  * is called to save the map data
	  * @param filename The url of the file to be saved
	  * @return This method will always return 0 */
	int saveData(QString filename);
	/** This method should be reimplemeted if this is to be a import filter. It is
	  * called to load the map data
	  * @param filename The url of the file to be loaded
	  * @return  0 , The file was loaded without problems
	  *         -1 , Could not open the file
	  *         -2 , If the file is corrupt
	  */
	int loadData(QString filename);
};

#endif
