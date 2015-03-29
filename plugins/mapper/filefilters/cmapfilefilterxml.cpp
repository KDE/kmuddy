/***************************************************************************
                               cmapfilefilterxml.cpp
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

#include "cmapfilefilterxml.h"

#include <kmessagebox.h>
#include <klocale.h>
#include <kzip.h>
#include <ktemporaryfile.h>

#include <qfile.h>
#include <qdom.h>
#include <qfileinfo.h>
#include <q3textstream.h>

#include "../cmaproom.h"
#include "../cmappath.h"
#include "../cmaptext.h"
#include "../cmapmanager.h"
#include "../cmaplevel.h"
#include "../cmapelementutil.h"
#include "../cmappluginbase.h"

#include <kdebug.h>

CMapFileFilterXML::CMapFileFilterXML(CMapManager *manager) : CMapFileFilterBase(manager)
{
}

CMapFileFilterXML::~CMapFileFilterXML()
{
}

/** This returns name of the import/export filter. This should be kept small
  * @return The Name of the filter */
QString CMapFileFilterXML::getName(void)
{
	return "Kmud 2 map xml filles (*.mapxml)";
}

/** This returns a discription of the import/export filter
  * @return The discription */
QString CMapFileFilterXML::getDescription(void)
{
	return "Kmud 2 save/load filter";
}

/** This returns the extension  of the filename that will be loaded,created
  * @return The exstension */
QString CMapFileFilterXML::getExtension(void)
{
	return ".mapxml";
}

/** This returns the pattern extension of the filename that will be loaded,created
      * @return The exstension */
QString CMapFileFilterXML::getPatternExtension(void)
{
	return "*" + getExtension();
}

/** This method will return true or false depending on if it's a export filter
  * @return True if this is a export filter, otherwise false */
bool CMapFileFilterXML::supportSave(void)
{
	return true;
}

/** This method will return true or false depending on if it's a import filter
  * @return True if this is a import filter, otherwise false */
bool CMapFileFilterXML::supportLoad(void)
{
	return true;
}

/** This method should be reimplemented if this is a to be a export filter. It
  * is called to save the map data
  * @param url The url of the file to be saved
  * @return 0, The file was saved succesfully
  * @return -1, The file could not be created
  */
int CMapFileFilterXML::saveData(QString filename)
{
	// Create the archive
	KZip zip(filename);	
	if ( !zip.open( QIODevice::WriteOnly ) )
	{

		return -1;
	}

	zip.setCompression(KZip::DeflateCompression);

	// Create the xml map file
	QString result = saveXMLFile();
	if (!result.isEmpty())
	{
		kDebug() << "Write map.xml : " << result.size();
		zip.writeFile("map.xml", QString(), QString(), result.toLocal8Bit(), result.size());
		kDebug() << "Done write";		

	}
	
	zip.close();
	
	return result.isEmpty() ? -1 : 0;
}

QString CMapFileFilterXML::saveXMLFile()
{
	for (CMapPluginBase *plugin = m_mapManager->getPluginList()->first(); plugin!=0; plugin = m_mapManager->getPluginList()->next())
	{
		plugin->saveAboutToStart();
	}

	// Create XML Document and add root node
	QDomDocument doc ("kmudmap");
	QDomElement root = doc.createElement ("kmudmap");
	doc.appendChild(root);

	// Write version
	QDomElement version = doc.createElement("Version");
	version.setAttribute("Major",1);
	version.setAttribute("Minor",0);
	root.appendChild(version);

	// Write main header
	
	// Write the zone
	CMapZone *rootZone = m_mapManager->getZone();
	saveZone(&doc,&root,rootZone);

	// Write Path Header
	QDomElement paths = doc.createElement("Paths");
	root.appendChild(paths);

	// Write Links Header
	QDomElement links = doc.createElement("Links");
	root.appendChild(links);

	// Write Paths
	saveZoneLinks(&doc,&paths,&links,m_mapManager->getZone());

	// Write Speedwalk list

	// return the result
        return doc.toString();
}

/**
 * This method is used to save the zone and all of it's sub elements
 * @param doc The document being the elemnts are saved too 
 * @param rootNode The XML node to save the zone to
 * @param zone The zone to save
 */
void CMapFileFilterXML::saveZone(QDomDocument *doc,QDomNode *rootNode,CMapZone *zone)
{
	// Save Zone
	QDomElement zoneProperties = doc->createElement("Zone");
	zone->saveQDomElement(doc,&zoneProperties);
	savePluginPropertiesForElement(zone,doc,&zoneProperties);	
	
	foreach (CMapLevel *level, *zone->getLevels())
	{
		QDomElement levelProperties = doc->createElement("Level");
		levelProperties.setAttribute("ID",level->getLevelID());
		levelProperties.setAttribute("Number",level->getNumber());
		levelProperties.setAttribute("NumRooms",level->getRoomList()->count());
		levelProperties.setAttribute("NumTexts",level->getTextList()->count());
		
		// Save Rooms
		foreach (CMapRoom* room, *level->getRoomList())
		{
			QDomElement roomProperties = doc->createElement("Room");
			room->saveQDomElement(doc,&roomProperties);
			savePluginPropertiesForElement(room,doc,&roomProperties);
			levelProperties.appendChild(roomProperties);
		}
		
		// Save Texts
		foreach (CMapText* text, *level->getTextList())
		{
			QDomElement textProperties = doc->createElement("Text");
			text->saveQDomElement(doc,&textProperties);
			savePluginPropertiesForElement(text,doc,&textProperties);			
			levelProperties.appendChild(textProperties);	
		}

		zoneProperties.appendChild(levelProperties);
	}

	rootNode->appendChild(zoneProperties);
}

/**
  * This method is used to save all the paths in a zone
  * @param doc The document being the paths are saved too
  * @param rootNode The XML node to save the paths to
  * @param zone The zone to save
  */
void CMapFileFilterXML::saveZoneLinks(QDomDocument *doc,QDomElement *pathsNode,QDomElement *linksNode,CMapZone *zone)
{
  if (zone == NULL)
    return;

  foreach (CMapLevel *level, *zone->getLevels())
  {
    foreach (CMapRoom *room, *level->getRoomList())
    {
      foreach (CMapPath *path, *room->getPathList())
      {
        QDomElement pathElement = doc->createElement("Path");
        path->saveQDomElement(doc,&pathElement);
        savePluginPropertiesForElement(path,doc,&pathElement);
        pathsNode->appendChild(pathElement);
      }
    }

    foreach (CMapText *text, *level->getTextList())
    {
      CMapElement *element = text->getLinkElement();
      if (element)
      {
        QDomElement linkElement = doc->createElement("Link");

        linkElement.setAttribute("SrcType",text->getElementType());
        linkElement.setAttribute("SrcLevel",text->getLevel()->getLevelID());
        linkElement.setAttribute("SrcID",text->getTextID());
        linkElement.setAttribute("DestType",element->getElementType());
        linkElement.setAttribute("DestLevel",element->getLevel()->getLevelID());
        if (element->getElementType()==ROOM)
        {
          linkElement.setAttribute("DestID",((CMapRoom *)element)->getRoomID());
          linkElement.setAttribute("LabelPos",(int)((CMapRoom *)element)->getLabelPosition());
        }

        linksNode->appendChild(linkElement);
      }
    }
  }
}

/** This method should be reimplemeted if this is to be a import filter. It is
  * called to load the map data
  * @param url The url of the file to be loaded
  * @return  0 , The file was loaded without problems
  *         -1 , Could not open the file
  *         -2 , If the file is corrupt
  *         -4 , Wrong version
  */
int CMapFileFilterXML::loadData(QString filename)
{
	KZip zip(filename);
	if ( !zip.open( QIODevice::ReadOnly ) )
	{
		return -1;
	}

	int result = -1;
	const KArchiveDirectory* dir = zip.directory();
	const KArchiveEntry *e = dir->entry("map.xml");
	if (e->isFile())
	{
		const KArchiveFile* mapFile = (KArchiveFile*)e;

		if (mapFile)
		{
			QByteArray arr( mapFile->data() );
			result = loadXMLData(arr);
		}
	}

	zip.close();

	return result;
}
  
/** This method should be reimplemeted if this is to be a import filter. It is
  * called to load the map data
  * @param url The url of the file to be loaded
  * @return  0 , The file was loaded without problems
  *         -1 , Could not open the file
  *         -2 , If the file is corrupt
  *         -4 , Wrong version
  */
//int CMapFileFilterXML::loadXMLData(QString filename)
int CMapFileFilterXML::loadXMLData(const QByteArray & buffer)
{
	// TODO_jp : Make sure Zone ID and level ID max value is set corretly
	QDomDocument doc ("map");	

	if (!doc.setContent( buffer))
	{
		kDebug() << "Unable to open the map file, not a valid xml document";
		// file.close();
		return -1;
	}

	for (CMapPluginBase *plugin = m_mapManager->getPluginList()->first(); plugin!=0; plugin = m_mapManager->getPluginList()->next())
	{
		plugin->loadAboutToStart();
	}

    QDomElement docElem = doc.documentElement();

	// Check that this version of the file can be loaded
	QDomElement e = readChildElement(&docElem,"Version");
	if (!e.isNull())
	{
		QString major = e.attribute("Major","");
		QString minor = e.attribute("Minor","");
		
		if (major != "1" || minor != "0")
		{
			 //TODO_jp : Output error message
			kDebug() << "This version can't be loaded";
			return -4;
		}
    }
    else
    {
		//TODO_jp : Output error message
		kDebug() << "Unable to find version";		
		return -2;
    }

	// Find Root Zone
    QDomElement rootZoneNode = readChildElement(&docElem,"Zone");
    if (rootZoneNode.isNull())
    {
		//TODO_jp : Output error message
		kDebug() << "Unable to find root zone";
		return -2;
    }

    // Load Root Zone
    int errorZone =loadZone(&rootZoneNode);
    
    if (errorZone!=0)
    {
		return errorZone;
	}

    // Find Paths
    QDomElement pathsNode = readChildElement(&docElem,"Paths");
    if (pathsNode.isNull())
    {
		//TODO_jp : Output error message
		kDebug() << "Unable to find paths";
		return -2;
    }

	// Load Paths
	int errorPath = loadPaths(&pathsNode);

	if (errorPath!=0)
	{
		return errorPath;
	}

	// Find Links
	QDomElement linksNode = readChildElement(&docElem,"Links");
	if (pathsNode.isNull())
	{
		//TODO_jp : Output error message
		kDebug() << "Unable to find links";
		return -2;
	}

	int errorLinks = loadLinks(&linksNode);
	if (errorLinks!=0)
	{
		return errorLinks;
	}

	// Return no error
	return 0;
}

/** This method is used to load all of the links
  * @param pathsNode The XML node to load the links from
  * @return  0 , The file was loaded without problems
  *         -2 , If the file is corrupt
  */
int CMapFileFilterXML::loadLinks(QDomElement *pathsNode)
{
	QDomNode n = pathsNode->firstChild();
	while (!n.isNull() )
	{
		QDomElement e = n.toElement();

		if (e.isNull() )
		{
			kDebug() << "Unable to find link element ";
			return -2;
		}

		if (e.tagName()=="Link")
		{
			
			int srcLevelID = e.attribute("SrcLevel","-2").toInt();
			int destLevelID = e.attribute("DestLevel","-2").toInt();

			if (srcLevelID == -2 || destLevelID == -2)
			{
				kDebug() << "Unable to find link end points";
				return -2;				
			}

			CMapLevel *srcLevel = m_mapManager->findLevel(srcLevelID);
			CMapLevel *destLevel = m_mapManager->findLevel(destLevelID);


			int textID = e.attribute("SrcID","-2").toInt();
			int destID = e.attribute("DestID","-2").toInt();
			int labelPos = e.attribute("LabelPos","0").toInt();
			int srcTyp = e.attribute("SrcType","-2").toInt();
			int destTyp = e.attribute("DestType","-2").toInt();
			

			if (textID == -2 || destID == -2 || destTyp == -2 || srcTyp == -2)
			{
				kDebug() << "Unable to find link end points";
				return -2;
			}

			if (srcTyp == (int)TEXT)
			{
				CMapText *text = srcLevel->findText(textID);
				if (destTyp==(int)ROOM)
				{
					CMapRoom *destElement = destLevel->findRoom(destID);
					destElement->setLabelPosition((CMapRoom::labelPosTyp)labelPos,text);
				}
			}            
		}

		n = n.nextSibling();
	}

	return 0;
}


/** This method is used to load all of the paths
  * @param pathsNode The XML node to load the paths from
  * @return  0 , The file was loaded without problems
  *         -2 , If the file is corrupt
  */  
int CMapFileFilterXML::loadPaths(QDomElement *pathsNode)
{
	QDomNode n = pathsNode->firstChild();
	while (!n.isNull() )
	{
		QDomElement e = n.toElement();

		if (e.isNull() )
		{
			kDebug() << "Unable to find path element ";
			return -2;
		}

		if (e.tagName()=="Path")
		{
			int srcLevelID = e.attribute("SrcLevel","-2").toInt();
			int destLevelID = e.attribute("DestLevel","-2").toInt();

			if (srcLevelID == -2 || destLevelID == -2)
			{
				kDebug() << "Unable to find path end points";
				return -2;
			}
			CMapLevel *srcLevel = m_mapManager->findLevel(srcLevelID);
			CMapLevel *destLevel = m_mapManager->findLevel(destLevelID);

			int srcRoomID = e.attribute("SrcRoom","-2").toInt();
			int destRoomID = e.attribute("DestRoom","-2").toInt();

			if (destRoomID == -2 || srcRoomID == -2)
			{
				kDebug() << "Unable to find path end points";
				return -2;
			}

			CMapRoom *srcRoom = srcLevel->findRoom(srcRoomID);
			CMapRoom *destRoom = destLevel->findRoom(destRoomID);			

			if (srcRoom==NULL || destRoom==NULL)
			{				
				kDebug() << "Src or Dest room is NULL while creating path";
                
				return -2;
			}

			directionTyp srcDir = (directionTyp)e.attribute("SrcDir","0").toInt();
			directionTyp destDir = (directionTyp)e.attribute("DestDir","0").toInt();

			CMapPath *path = m_mapManager->createPath(srcRoom,srcDir,destRoom,destDir);
			path->loadQDomElement(&e);
			loadPluginPropertiesForElement(path,&e);
		}

		n = n.nextSibling();
	}
	kDebug() << "loadPaths Here 4";	

	return 0;
}

/**
  * This method is used to load the zone and all of it's sub elememnts
  * @param zoneNode The XML node to load the zone from
  * @param intoLevel The level to create the zone in
  * @return  0 , The file was loaded without problems
  *         -2 , If the file is corrupt
  */  
int CMapFileFilterXML::loadZone(QDomElement *zoneNode)
{
	int gridWidth = m_mapManager->getMapData()->gridSize.width();
	int gridHeight = m_mapManager->getMapData()->gridSize.height();

	
	CMapZone *zone = m_mapManager->getZone();

	QDomNode n = zoneNode->firstChild();
	while (!n.isNull() )
	{
		QDomElement e = n.toElement();

		if (e.isNull() )
		{
			kDebug() << "Unable to find element ";
			return -2;
		}

		if (e.tagName()=="Level")
		{
			CMapLevel *level = m_mapManager->createLevel(UP);
			QString id = e.attribute("ID","-2");
			if (id=="-2")
			{
				kDebug() << "Unable to find level ID";
				return -2;
			}
			level->setLevelID(id.toInt());

			QDomNode n2 = e.firstChild();
			while (!n2.isNull() )
			{
				QDomElement e2 = n2.toElement();

				if (e2.isNull() )
				{
					kDebug() << "Unable to find element ";
					return -2;
				}
			
				int x1 = e2.attribute("X",QString::number(-1)).toInt();
				int y1 = e2.attribute("Y",QString::number(-1)).toInt();			
				
				if (x1==-1 || y1==-1)
				{
					kDebug() << "Unable to find pos ";
					return -2;
				}

				if (e2.tagName()=="Room")
				{
					CMapRoom *room = CMapElementUtil::createRoom(m_mapManager, QPoint(x1 * gridWidth,y1 * gridHeight),level);
					
					room->loadQDomElement(&e2);
					loadPluginPropertiesForElement(room,&e2);
				}
				else if (e2.tagName()=="Text")
				{
					CMapText *text = CMapElementUtil::createText(m_mapManager, QPoint (x1,y1),level,"");
					text->loadQDomElement(&e2);
					loadPluginPropertiesForElement(text,&e2);
				}
				else
				{
					kDebug() << "Unknown Type :  " << e2.tagName();
				}
				
				n2 = n2.nextSibling();
			}
		}
				

		n = n.nextSibling();
	}
	
	return 0;
}

/**
 * This method is used to read a child XML object of a XML object
 * @param parent The parent XML object
 * @param key The name of the child
 * @return The child node
 */
QDomElement CMapFileFilterXML::readChildElement(QDomElement *parent,QString key)
{
	QDomElement e;
	
	// Find Root Zone
    QDomNode n = parent->namedItem(key);
    if (n.isNull())
    {
		e.clear();
		return e;
    }

    e = n.toElement();

    return e;
}

/**
 * This method is used to save element properties that are stored in plugins
 * @param element The element being saved
 * @param doc The XML document
 * @param elProperties The xml properties of the element
 */
void CMapFileFilterXML::savePluginPropertiesForElement(CMapElement *element,QDomDocument *doc,QDomElement *elProperties)
{
	typedef QMap<QString, QString> EntryMap;
    
	QDomElement pluginsNode = doc->createElement ("plugins");


	for (CMapPluginBase *plugin = m_mapManager->getPluginList()->first(); plugin!=0; plugin = m_mapManager->getPluginList()->next())
	{
		QDomElement pNode = doc->createElement(plugin->name());
		KMemConfig pluginProperties;
		plugin->saveElementProperties(element,&pluginProperties);

		EntryMap entries = pluginProperties.entryMap("Properties");
		for (EntryMap::ConstIterator it = entries.begin(); it != entries.end(); ++it)
		{
			pNode.setAttribute(it.key(),it.data());
		}
		
		pluginsNode.appendChild(pNode);
	}

	elProperties->appendChild(pluginsNode);
}

/**
 * This method is used to load element properties that are stored in plugins
 * @param element The Element being loaded
 * @param elProperties the xml properties of the element
 */
void CMapFileFilterXML::loadPluginPropertiesForElement(CMapElement *element,QDomElement *elProperties)
{
	QDomElement pluginsNode = readChildElement(elProperties,"plugins");
	if (!pluginsNode.isNull())
	{
		QDomNode n = pluginsNode.firstChild();
		while (!n.isNull() )
		{

			QDomElement e = n.toElement();

			if (!e.isNull() )
			{
				for (CMapPluginBase *plugin = m_mapManager->getPluginList()->first(); plugin!=0; plugin = m_mapManager->getPluginList()->next())
				{
					if (plugin->name()==e.tagName())
					{
						KMemConfig pluginProperties;

						QDomNamedNodeMap attribs = e.attributes();

						for (int i=0; i<attribs.length();i++)
						{
							QDomNode n2 = attribs.item(i);

							kDebug() << "Attrib " << n2.nodeName() << " = " << n2.nodeValue();
							pluginProperties.group("Properties").writeEntry(n2.nodeName(),n2.nodeValue());
							
						}
						
						plugin->loadElementProperties(element,&pluginProperties);
						break;	
					}
                }
			}

			n = n.nextSibling();
		}
	}
}
