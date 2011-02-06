/***************************************************************************
                               cmapcmdnotes.cpp
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

#include "cmapcmdnotes.h"

#include "cmappluginstandard.h"

#include "../../cmapmanager.h"
#include "../../cmapelement.h"

#include <klocale.h>

CMapCMDNotes::CMapCMDNotes(CMapPluginStandard *plugin,CMapElement *element,QString note) : CMapCommand (i18n("Change element notes"))
{
	m_plugin = plugin;
	m_element = element;
	m_newNote = note;
	m_oldNote = m_plugin->getNote(element);
}

CMapCMDNotes::~CMapCMDNotes()
{
}

void CMapCMDNotes::execute()
{
	m_plugin->addNote(m_element,m_newNote);
}

void CMapCMDNotes::unexecute()
{
	m_plugin->addNote(m_element,m_oldNote);
}
