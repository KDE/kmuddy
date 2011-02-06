/***************************************************************************
                          domconfig.cpp  -  description
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


#include "domconfig.h"

// KDE/Qt includes
#include <kconfiggroup.h>
#include <qmap.h>

// application specific includes
#include <kmemconfig.h>
#include <kdebug.h>
#include <kvbox.h>

DomConfig* DomConfig::findGroup(const QString& name, QDomElement& currentGroup, bool create)
{
	if (name.isNull() || currentGroup.isNull())
		return 0;

	QString path = name.trimmed();
	QDomElement group = currentGroup;
	if (path.at(0) == '/')
	{
		if (group.nodeName() != "config")
		{
			// see if we can find a config child
			QDomNodeList children = group.elementsByTagName("config");
			if (children.count() < 1)
			{
				if (create)
				{
					QDomElement configElement = group.ownerDocument().createElement("config");
					group.appendChild(configElement);
					group = configElement;
				}
				else
					return 0;
			}
			else
			{
				group = children.item(0).toElement();
			}
		}
		
		QDomNode node = group.parentNode();
		while (node.nodeName() == "config")
		{
			group = node.toElement();
			node = node.parentNode();
		}
	}
	
	QStringList groups = QStringList::split("/", path);
	if (groups.count() == 0)
	{
		DomConfig* c = new DomConfig(group);
		c->load();
		return c;
	}
	
	QStringList::Iterator groupIt = groups.begin();
	while (groupIt != groups.end())
	{
		bool atEnd = false;
		bool found = false;
		QDomNode childNode = group.firstChild();
		while (!found && !atEnd)
		{
			// find next child element
			while (!childNode.isNull() && !childNode.isElement())
				childNode = childNode.nextSibling();
			if (childNode.isNull())
				atEnd = true;
			else
			{
				QDomElement childGroup = childNode.toElement();
				if (childGroup.nodeName() == "config" && childGroup.attribute("name") == (*groupIt))
				{
					found = true;
					
					// step in
					group = childGroup;
				}
				else
				{
					childNode = childNode.nextSibling();
				}
			}
		}
		
		if (!found)
		{
			if (create)
			{
				QDomElement parent = group;
				group = currentGroup.ownerDocument().createElement("config");
				group.setAttribute("name", (*groupIt));
				parent.appendChild(group);
			}
			else
				return 0;
		}
		++groupIt;
	}
	
	DomConfig* c =  new DomConfig(group);
	c->load();
	return c;
}

DomConfig::DomConfig(QDomElement& groupElement)
{
	m_groupElement = groupElement;
	m_groups.setAutoDelete(true);
	
	QString instname("domconfig-"+groupElement.attribute("name"));
	m_config = new KMemConfig();
}

DomConfig::~DomConfig()
{
	delete m_config;
}

void DomConfig::save()
{
	saveConfig();
	
	// remove dublicates
	QDomNode node = m_groupElement.firstChild();
	QDomNode next;
	while(!node.isNull())
	{
		bool remove = false;
		if (node.isElement() && node.nodeName() == "group")
		{
			QString name = node.toElement().attribute("name");
			if (!name.isNull())
			{
				QDomElement* entry = m_groups.find(name);
				if (entry)
				{
					remove = true;
					syncGroup(entry, node.toElement());
				}
			}
		}
		next = node.nextSibling();
		if (remove)
			m_groupElement.removeChild(node);
		
		node = next;
	}
	
	// append entries
	Q3DictIterator<QDomElement> it(m_groups);
	while (it.current())
	{
		QDomElement elem(*(it.current()));
		m_groupElement.appendChild(elem);
		++it;
	}
}

void DomConfig::load()
{
	m_groups.clear();
	for (QDomNode node = m_groupElement.firstChild(); !node.isNull();
	     node = node.nextSibling())
	{
		if (node.isElement())
		{
			QString nodeName = node.nodeName();
			if (nodeName == "group")
			{
				loadGroup(node.toElement());
			}
			else if (nodeName == "config")
			{
				// do nothing
			}
			else
				kWarning() << "DomConfig::load: unknown child node " << nodeName;
		}
	}
	
	loadConfig();
}

void DomConfig::loadGroup(QDomElement element)
{
	QString name = element.attribute("name");
	if (name.isEmpty())
	{
		kWarning() << "DomConfig::loadGroup: element " << element.nodeName()
		            << " has empty name. skipping!\n";
	}
	else
		m_groups.replace(name, new QDomElement(element));
	
}

void DomConfig::syncGroup(QDomElement* newGroup, QDomElement oldGroup)
{
	// load entries of newGroup into a dict
	Q3Dict<QDomElement> m_entries;
	m_entries.setAutoDelete(true);
	for (QDomNode node = newGroup->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		if (node.isElement())
		{
			QDomElement* entry = new QDomElement(node.toElement());
			m_entries.replace(entry->attribute("name"), entry);
		}
	}
	
	// now we sync with oldGroup
	for (QDomNode node = oldGroup.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		if (node.isElement())
		{
			QDomElement entry = node.toElement();
			QString name = entry.attribute("name");
			if (!name.isEmpty() && !m_entries.find(name))
			{
				// the entry doesn't exist in newGroup yet
				newGroup->appendChild(entry);
			}
		}
	}
}


QDomElement* DomConfig::createEntry(const QString& tagname, const QString& name,
                                    const QString& textValue)
{
	QDomDocument doc = m_groupElement.ownerDocument();
	QDomElement* elem = new QDomElement(doc.createElement(tagname));
	elem->setAttribute("name", name);
	if (!textValue.isNull())
	{
		QDomText text = doc.createTextNode(textValue);
		elem->appendChild(text);
	}
	
	return elem;
}

QString DomConfig::parseString(QDomElement* entry, const QString& def)
{
	// fixxme_kevin: concatenate all text children
	QString result = entry->firstChild().nodeValue();
	if (result.isNull())
		return def;
	else
		return result;
}


KMemConfig* DomConfig::config() const
{
	return m_config;
}

void DomConfig::saveConfig()
{
	typedef QMap<QString, QString> EntryMap;
	
	m_groups.clear();
	QStringList groupList = m_config->groupList();
	for (QStringList::Iterator it = groupList.begin(); it != groupList.end(); ++it)
	{
		EntryMap entries = m_config->entryMap(*it);
		QDomElement* groupEntry = createEntry("group", *it);
		for (EntryMap::Iterator entryIt = entries.begin(); entryIt != entries.end(); ++entryIt)
		{
			QDomElement* entry = createEntry("entry", entryIt.key(), entryIt.data());
			groupEntry->appendChild(QDomElement(*entry));
			delete entry;
		}
		m_groups.replace(*it, groupEntry);
	}
}

void DomConfig::loadConfig()
{
	Q3DictIterator<QDomElement> it(m_groups);
	while (it.current())
	{
		QDomElement* groupElem = it.current();
		QString groupName = groupElem->attribute("name");
		if (!groupName.isEmpty())
		{
			KConfigGroup grp = m_config->group(groupName);
			for (QDomNode node = groupElem->firstChild(); !node.isNull(); node = node.nextSibling())
			{
				if (node.isElement() && node.nodeName() == "entry")
				{
					QDomElement entryElem = node.toElement();
					QString name = entryElem.attribute("name");
					if (!name.isEmpty())
					{
						grp.writeEntry(name, parseString(&entryElem, ""));
					}
				}
			}
		}
		++it;
	}
}

