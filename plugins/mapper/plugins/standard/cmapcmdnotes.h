/***************************************************************************
                               cmapcmdnotes.h
                             -------------------
    begin                : Sun Mar 9 2003
    copyright            : (C) 2003 by Kmud Developer Team
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

#ifndef CMAPCMDNOTES_H
#define CMAPCMDNOTES_H

#include <qstring.h>

#include "../../cmapcommand.h"

class CMapManager;
class CMapElement;
class CMapPluginStandard;

/**This is used to add and remove the notes properties from a element
  *@author Kmud Developer Team
  */

class CMapCMDNotes : public CMapCommand
{
   //Q_OBJECT
public: 
	CMapCMDNotes(CMapPluginStandard *plugin,CMapElement *element,QString note);
	~CMapCMDNotes();

	virtual void execute();
	virtual void unexecute();
	
private:
	/** The element being used */
	CMapElement *m_element;
	/** Pointer to the pluign */
	CMapPluginStandard *m_plugin;
	/** The new note of the element */
	QString m_newNote;
	/** The old note of the element */
	QString m_oldNote;
};

#endif
