/***************************************************************************
                               cmaplistviewitem.cpp
                             -------------------
    begin                : Mon May 14 2001
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

#include "cmaplistviewitem.h"

#include "cmapelement.h"
#include "cmaplevel.h"

CMapListViewItem::CMapListViewItem( Q3ListView * parent )
	: Q3ListViewItem (parent)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListViewItem * parent )
	: Q3ListViewItem (parent)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListView * parent, Q3ListViewItem * after )
	: Q3ListViewItem(parent,after)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after )
	: Q3ListViewItem(parent,after)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListView * parent,
                              QString label1,
                              QString label2,
                              QString label3,
                              QString label4,
                              QString label5,
                              QString label6,
                              QString label7,
                              QString label8 )
	: Q3ListViewItem(parent,label1,label2,label3,label4,label5,label6,label7,label8)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListViewItem * parent,
                              QString label1,
                              QString label2,
                              QString label3,
                              QString label4,
                              QString label5,
                              QString label6,
                              QString label7,
                              QString label8 )
	: Q3ListViewItem(parent,label1,label2,label3,label4,label5,label6,label7,label8)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListView * parent, Q3ListViewItem * after,
                              QString label1,
                              QString label2,
                              QString label3,
                              QString label4,
                              QString label5,
                              QString label6,
                              QString label7,
                              QString label8 )
	: Q3ListViewItem(parent,after,label1,label2,label3,label4,label5,label6,label7,label8)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::CMapListViewItem( Q3ListViewItem * parent, Q3ListViewItem * after,
                              QString label1,
                              QString label2,
                              QString label3,
                              QString label4,
                              QString label5,
                              QString label6,
                              QString label7,
                              QString label8 )
	: Q3ListViewItem(parent,after,label1,label2,label3,label4,label5,label6,label7,label8)
{
	element = NULL;
	level = NULL;
}

CMapListViewItem::~CMapListViewItem()
{
}

void CMapListViewItem::setElement(CMapElement *mapElement)
{
	element = mapElement;
}

CMapElement *CMapListViewItem::getElement(void)
{
	return element;
}

CMapLevel *CMapListViewItem::getLevel(void)
{
	return level;
}

void CMapListViewItem::setLevel(CMapLevel *mapLevel)
{
	level = mapLevel;
}

