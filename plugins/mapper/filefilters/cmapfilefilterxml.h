/***************************************************************************
                               cmapfilefilterxml.h
                             -------------------
    begin                : Tue Nov 19 2002
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

#ifndef CMAPFILEFILTERXML_H
#define CMAPFILEFILTERXML_H

#include "../cmapfilefilterbase.h"
#include "../cmapelement.h"

#include <qfile.h>
#include <qstring.h>

class CMapManager;

/**
  *@author Kmud Developer Team
  */

class CMapFileFilterXML : public CMapFileFilterBase
{
public:
	CMapFileFilterXML(CMapManager *manager);
	~CMapFileFilterXML();

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
	  * @param url The url of the file to be saved
	  * @return 0, The file was saved succesfully
	  * @return -1, The file could not be created
	  */
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
	int loadXMLData(const QByteArray & buffer);	

	QString saveXMLFile();

	/**
	 * This method is used to save element properties that are stored in plugins
	 * @param element The element being saved
	 * @param doc The XML document
	 * @param elProperties The xml properties of the element
	 */
	void savePluginPropertiesForElement(CMapElement *element,QDomDocument *doc,QDomElement *elProperties);
	/**
	 * This method is used to load element properties that are stored in plugins
	 * @param element The Element being loaded
	 * @param elProperties the xml properties of the element
	 */
	void loadPluginPropertiesForElement(CMapElement *element,QDomElement *elProperties);


	/**
	  * This method is used to save the zone and all of it's sub elements
	  * @param doc The document being the elemnts are saved too
	  * @param rootNode The XML node to save the zone to
	  * @param zone The zone to save
	  */
	void saveZone(QDomDocument *doc,QDomNode *rootNode,CMapZone *zone);

	/**
	  * This method is used to save all the paths in a zone
	  * @param doc The document being the paths are saved too
	  * @param rootNode The XML node to save the paths to
	  * @param zone The zone to save
	  */
	void saveZoneLinks(QDomDocument *doc,QDomElement *pathsNode,QDomElement *linksNode,CMapZone *zone);
	
	/**
	  * This method is used to load the zone and all of it's sub elememnts
	  * @param zoneNode The XML node to load the zone from
	  * @param intoLevel The level to create the zone in
	  * @return  0 , The file was loaded without problems
	  *         -2 , If the file is corrupt
	  */
	int loadZone(QDomElement *zoneNode,CMapLevel *intoLevel);

	/** This method is used to load all of the paths
	  * @param pathsNode The XML node to load the paths from
	  * @return  0 , The file was loaded without problems
	  *         -2 , If the file is corrupt
	  */
	int loadPaths(QDomElement *pathsNode);

	/** This method is used to load all of the links
	  * @param pathsNode The XML node to load the links from
	  * @return  0 , The file was loaded without problems
	  *         -2 , If the file is corrupt
	  */
	int loadLinks(QDomElement *pathsNode);

	/**
	 * This method is used to read a child XML object of a XML object
	 * @param parent The parent XML object
	 * @param key The name of the child
	 * @return The child node
	 */
	 QDomElement readChildElement(QDomElement *parent,QString key);	 

};

#endif
