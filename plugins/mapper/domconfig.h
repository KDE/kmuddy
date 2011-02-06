/***************************************************************************
                          domconfig.h  -  description
                             -------------------
    begin                : Mon Aug 13 2001
    copyright            : (C) 2001 by Kmud Developer Team
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


#ifndef DOMCONFIG_H
#define DOMCONFIG_H

// Qt inlcudes
#include <qdom.h>
#include <q3dict.h>
#include <kvbox.h>

// forward declerations
class QFont;
class QColor;
class KMemConfig;

/** This class handles reading/writing of config entries
  * into a DOM tree
  * @author Kevin Krammer
  */

class DomConfig {
public:
	~DomConfig();

	/** Writes the stored entries into the DOM tree. Existing entries with the same
	  * name as one of the new entries, will be overwritten. */
	void save();
	/** Reads the entries stored in the DOM tree. This will delete all internal entries
	  * prior to loading. */
	void load();

	KMemConfig* config() const;

private: // private methods
	DomConfig(QDomElement& groupElement);
	
	/** Loads a single entry into internal store */
	void loadGroup(QDomElement element);
	void syncGroup(QDomElement* newGroup, QDomElement oldGroup);

	/** Creates an internal entry node. tagname is the node name of the DOM element.
	  * name is the value for the name attribute.
	  * textValue is an optional string that will be stored in a DOM Text node
	  * as a child of the newly created element. */
	QDomElement* createEntry(const QString& tagname, const QString& name,
	                         const QString& textValue = QString::null);

	/** Helper methods for parsing data contained in an entry element */
	QString parseString(QDomElement* entry, const QString& def);
	
	void saveConfig(); // saves values contained in the KConfig object
	void loadConfig(); // load values into the KConfig object
	
private: // private data
	// the dom element to which the entries will be appended as children
	QDomElement m_groupElement;

	// dictionary for caching entries
	Q3Dict<QDomElement> m_groups;

	KMemConfig* m_config;
	
public: // static methods
	/** Tries to find a group with the specified name/path relative to the group repesented
	  * by a DOM element. If the name begins with a slash '/' the search will start relative
	  * to the top most parent 'configgroup' element..
	  * If a group cannot be found this method returns 0, unless the third paramter is true.
	  * In this case the group (and all necessary groups in the path) will be created if
	  * it doesn't already exist. */
	static DomConfig* findGroup(const QString& name, QDomElement& currentGroup, bool create = false);
};

#endif
