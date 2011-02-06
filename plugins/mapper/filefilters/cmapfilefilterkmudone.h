/***************************************************************************
                               cmapfilefilterkmudone.h
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

#ifndef CMAPFILEFILTERKMUDONE_H
#define CMAPFILEFILTERKMUDONE_H

#include "../cmapfilefilterbase.h"
#include "../cmapelement.h"

#include <qfile.h>
#include <qstring.h>
#include <kvbox.h>

class CMapManager;

/**This class is use to load map files save in the old kmud 1 binary  format.
  *@author Kmud Developer Team
  */

class CMapFileFilterKmudOne : public CMapFileFilterBase
{
public: 
	CMapFileFilterKmudOne(CMapManager *manager);
	~CMapFileFilterKmudOne();

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

private:
	/**
	 * This method is used to read a string from a file
	 * @param f The file it is to be read from
	 * @return The string that was read
	 */
	QString readStr(QFile *f);
	/**
	 * This method is used to read a int from a file
	 * @param f The file that it is to be read from
	 * @return The int that was read
	 */
	int readInt(QFile *f);

private:

	// Sturcts used to save and load the maps

	struct zoneTyp
	{
		signed int x;
		signed int y;
		signed int numLevels;
		signed int level;
		signed int zone;
	};

	struct roomHeaderTyp
	{
		int numRooms;
	};

	struct roomTyp
	{	
		int x;
		int y;
		int red,green,blue;
		int useDefaultCol;
		signed int level;
		int zone;
	};

	struct speedwalkRoomTyp
	{
		int x;
		int y;
		signed int level;
		int zone;
	};

	struct textHeaderTyp
	{
		int numText;
	};
    	
	struct textTyp
	{
		signed int level;
		int x,y;
		int size;
		int weight;
		int italic;
		int red, green,	blue;
		int zone;
	};

	struct pathHeaderTyp
	{
		int numPaths;
	};

	struct pathTyp
	{
		int srcX;
		int srcY;
		signed int srcLevel;
		int srcZone;
		int destX;
		int destY;
		signed destLevel;
		int destZone;
		directionTyp srcDir;
		directionTyp destDir;
		bool special;
		int bendsCount;
	};

	struct mainHeaderTyp
	{
		int numZones;
	};

};

#endif
