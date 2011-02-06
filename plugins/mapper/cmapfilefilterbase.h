/***************************************************************************
                               cmapfilefilterbase.h
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

#ifndef CMAPFILEFILTERBASE_H
#define CMAPFILEFILTERBASE_H


#include <qfile.h>

#include <kurl.h>
#include <kvbox.h>


class CMapManager;

/**This class is used as a base calls for all save/load filters
  *@author Kmud Developer Team
  */

class CMapFileFilterBase
{
public: 
	CMapFileFilterBase(CMapManager *manager);
	virtual ~CMapFileFilterBase();

	/** This returns name of the import/export filter. This should be kept small
      * @return The Name of the filter */	
	virtual QString getName(void)=0;
	/** This returns a discription of the import/export filter
      * @return The discription */
	virtual QString getDescription(void)=0;
	/** This returns the extension  of the filename that will be loaded,created
      * @return The exstension */
	virtual QString getExtension(void)=0;
	/** This returns the pattern extension of the filename that will be loaded,created
      * @return The exstension */
	virtual QString getPatternExtension(void)=0;	

	/** This method will return true or false depending on if it's a export filter
      * @return True if this is a export filter, otherwise false */
	virtual bool supportSave(void)=0;
	/** This method will return true or false depending on if it's a import filter
      * @return True if this is a import filter, otherwise false */
	virtual bool supportLoad(void)=0;

	/** This method is called by the map manager to save map data.	
	  * @param url The url of the file to be saved
	  * @return  0 , The file was saved without problems
	  *         -1 , Could not open the file
	  *         -3 , KIO count not create the file
	  */
	int saveData(const KUrl &);
	/** This method is called by the map manager to load map data.	
	  * @param url The url of the file to be loaded
	  * @return  0 , The file was loaded without problems
	  *         -1 , Could not open the file
	  *         -2 , If the file is corrupt
	  *         -3 , KIO count not download the file
	  */
	int loadData(const KUrl &);

protected:
	/** This method should be reimplemeted if this is to be a export filter. It is
	  * called to save the map data
	  * @param filename The filename of the file to be loaded
	  * @return  0 , The file was saved without problems
	  *         -1 , Could not open the file
	  */
	virtual int saveData(QString ) { return 0; }
	/** This method should be reimplemeted if this is to be a import filter. It is
	  * called to load the map data
	  * @param filename The filename of the file to be loaded
	  * @return  0 , The file was loaded without problems
	  *         -1 , Could not open the file
	  *         -2 , If the file is corrupt
	  */
	virtual int loadData(QString) { return 0; }

protected:
	/** A pointer to the map manager */
	CMapManager *m_mapManager;
};

#endif
