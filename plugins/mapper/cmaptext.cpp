/***************************************************************************
                               cmaptext.cpp
                             -------------------
    begin                : Sat Mar 10 2001
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

#include "cmaptext.h"

#include <qfontmetrics.h>
#include <qfont.h>
#include <qstring.h>
#include <qcolor.h>
#include <q3strlist.h>
#include <qpoint.h>

#include "cmapmanager.h"
#include "cmaplevel.h"

#include <kdebug.h>
#include <kvbox.h>

CMapText::CMapText(QString str,QFont f,QColor col,CMapManager *manager,QPoint pos,CMapLevel *level) : CMapElement(manager,level)
{	
	m_font = f;
	setRect(QRect(pos,pos));
	setText(str);

	setColor(col);

	m_linkElement = NULL;
	m_xscale = 0;
	m_yscale = 0;

	getZone()->m_text_id_count=getZone()->m_text_id_count+1;
	m_ID = getZone()->m_text_id_count;	
}

CMapText::CMapText(QString str,CMapManager *manager,QPoint pos,CMapLevel *level) : CMapElement(manager,level)
{
	m_font = manager->getMapData()->defaultTextFont;
	setRect(QRect(pos,pos));
	setText(str);
	setColor(Qt::black);
	m_linkElement = NULL;

	getZone()->m_text_id_count=getZone()->m_text_id_count+1;
	m_ID = getZone()->m_text_id_count;	
}

CMapText::~CMapText()
{
	if (m_linkElement)
	{
		if (m_linkElement->getElementType()==ROOM)
		{
			((CMapRoom *)m_linkElement)->textRemove();
		}

		if (m_linkElement->getElementType()==ZONE)
		{
			((CMapZone *)m_linkElement)->textRemove();
		}
	}
}

/** This is used to return the actual cords in the view of the cursor
  * @return the cords */
QPoint CMapText::getCursorCords(void)
{
	return m_cursorOffset + getLowPos();
}

void CMapText::dragPaint(QPoint offset,QPainter *p,CMapZone *zone)
{
	QPoint pos(getX()+offset.x(),getY()+offset.y());
	paintElementResize(p,pos,getSize(),zone);
}

void CMapText::lowerPaint(QPainter *,CMapZone *)
{
	// Do nothing as the text is not visiable on the lower level 
}

void CMapText::higherPaint(QPainter *,CMapZone *)
{
	// Do nothing as the text is not visiable on the upper level 
}

/** This is used to convert a Actual size to it's closet font size
  * @param size The actual size of the font
  * @param font The text font
  * @return The closet matching font size */
int CMapText::getActualToFontSize(QSize size,QFont font,QStringList *text)
{
	QFont tmpFont = font;
	int result = 1;
	tmpFont.setPointSize(result);
	QSize fontSize = QSize(1,1);

	//FIXME_jp: Check this does not go into infinate loop because there is
	//          no upper limit check.
	while (fontSize.width() < size.width() && fontSize.height() < size.height())
	{
		result+=10;
		tmpFont.setPointSize(result);
		QFontMetrics fm(tmpFont);	
		int tmpWidth = 0;
		for (QStringList::iterator it = text->begin(); it != text->end(); ++it)
		{
			if (fm.width(*it) > tmpWidth)
				tmpWidth = fm.width(*it);
		}

		fontSize = QSize(tmpWidth,fm.height());
	}

	while ((fontSize.width() > size.width() || fontSize.height() > size.height()) && result > 1)
	{
		result--;
		tmpFont.setPointSize(result);
		QFontMetrics fm(tmpFont);	
		int tmpWidth = 0;
		for (QStringList::iterator it = text->begin(); it != text->end(); ++it)
		{
			if (fm.width(*it) > tmpWidth)
				tmpWidth = fm.width(*it);
		}

		fontSize = QSize(tmpWidth,fm.height());
	}

	return result;
}

/** Used to paint the text on the map
  * @param p The painter used to paint the text
  * @param zone The zone the text is being painted in
  */
void CMapText::paint(QPainter *p,CMapZone *zone)
{
	getScale(getFont(),&m_text,getSize(),&m_xscale,&m_yscale);
	CMapElement::paint(p,zone);

	if (getEditMode())
	{
		p->save();

		p->translate(getX(),getY());
		p->scale(m_xscale,m_yscale);

		QFontMetrics fm(m_font);

		p->setPen(getManager()->getMapData()->defaultTextColor);
		p->setBrush(getManager()->getMapData()->defaultTextColor);
		
		int x = m_cursorOffset.x();
		int y1 = m_cursorOffset.y();
		int y2 = m_cursorOffset.y()-fm.height();
		p->drawLine(x ,y1,x ,y2 );

		p->restore();
	}
}
                                                                                                        	
/** Used to paint the element at a given location and size
  * @param p The painer to paint the element to
  * @param pos The position to paint the elmenet
  * @param size The size the element should be draw
  * @param zone The current zone being viewed */
void CMapText::paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *)
{
	paintText(p,m_col,pos,m_font,&m_text,size);
}

/** This is used to paint the text. It is capable of painting multiline
  * of text. It is static so that it can be called with out creating a instance of CMapText
  * @param p The painter used to paint the text
  * @param col The color to paint the text
  * @param pos The position to paint the text
  * @param font The Font used to paint the text
  * @param text A point to the text list
  * @param size The size of the text */
void CMapText::paintText(QPainter *p,QColor col,QPoint pos,QFont font,QStringList *text,QSize size)
{
	double xscale;
	double yscale;
	QFont tmpFont = font;
	int fontSize = getActualToFontSize(size,font,text);

	tmpFont.setPointSize(fontSize);

	p->save();
	getScale(tmpFont,text,size,&xscale,&yscale);
	p->translate(pos.x(),pos.y());
	p->scale(xscale,yscale);
	paintText(p,col,QPoint(0,0),tmpFont,text);
	p->restore();
}

/** This is used to paint the text. It is capable of painting multiline of text */
void CMapText::paintText(QPainter *p,QColor col,QPoint pos,QFont font,QStringList *text)
{
	QFontMetrics fm(font);
	pos.setY(pos.y()+fm.height()-fm.descent());

	p->setPen(col);
	p->setBrush(col);

	p->setFont(font);

	for (QStringList::iterator it = text->begin(); it != text->end(); ++it)
	{
		p->drawText(pos,*it,-1);

		pos.setY(pos.y()+fm.height());
	}
}

/** This is used to set the cursor position within the text element
  * @param pos The cursor position
  */
void CMapText::setCursor(QPoint pos)
{
	if (pos.y() > (int)m_text.count())
	{
		// Set the cursor to the last position
		m_cursorPos.setX(QString(m_text.last()).length());
		m_cursorPos.setY(m_text.count());
	}
	else
	{
		m_cursorPos = pos;
	}

	setActualCursorPosition();
}

/**
 * This method is used to calcualte the scale that the text should be scale by
 * @param text A pointer to the text list
 * @param requiredSize The size of the text
 * @param xscale Used to return the x axis scale value
 * @param yscale Used to return the y axis scale value
 */
void CMapText::getScale(QFont font,QStringList *text,QSize requiredSize,double *xscale,double *yscale)
{
	QFontMetrics fm(font);
	int tmpWidth = 0;
	for (QStringList::iterator it = text->begin(); it != text->end(); ++it)
	{
		if (fm.width(*it) > tmpWidth)
			tmpWidth = fm.width(*it);
	}
	int tmpHeight = fm.height()*text->count();

	*xscale = ((double)requiredSize.width())/((double)tmpWidth);
	*yscale = ((double)requiredSize.height())/((double)tmpHeight);
}

/** This is used to conver a offset from the element orgin into a cursor position
  * @param offset The offset of the curosr */
QPoint CMapText::convertOffsetToCursor(QPoint offset)
{
	QFontMetrics fm(m_font);

	double m_xscale;
	double m_yscale;

	getScale(getFont(),&m_text,getSize(),&m_xscale,&m_yscale);

	int y = (int)(offset.y()  / (fm.height() * m_yscale));
	int x = 0;

	QString s = m_text.at(y);

	if (s.length()>0)
	{
		bool found = false;

		for (int pos = 0 ; pos <=(int)s.length() ; pos ++)
		{
			int charWidth =fm.width(s.left(pos));
			if (charWidth * m_xscale >offset.x())
			{
				x = pos -1;
				found = true;
				break;
			}
		}

		if (!found)
		{
			x = (int)((double)fm.width(s) * m_xscale);
		}
	}

	return QPoint(x,y+1);
}

/** This is used to convert mouse cordinates into a cursor position */
QPoint CMapText::convertPosToCursor(QPoint mousePos)
{
	mousePos-=getLowPos();

	return convertOffsetToCursor(mousePos);
}

/** This method is called when the element looses it's edit mode */
void CMapText::editModeUnsetEvent(void)
{
	kDebug() << "CMapText::editModeUnsetEvent";
	//FIXME_jp: Make sure elements are deleted

	if (m_text.count()==0)
		emit deleteElement((CMapElement *)this,true);
	else
		if (QString(m_text.first()).trimmed().length()==0 && m_text.count() == 1)
			emit deleteElement((CMapElement *)this,true);

	// If the text is bigger than the curent size the make the bound box bigger
	QFontMetrics fm(m_font);
	int width = 0;
	int height = 0;

	for (QStringList::iterator it = m_text.begin(); it != m_text.end(); ++it)
	{
		if (fm.width(*it)>width)
			width = fm.width(*it);

		height+=fm.height();
	}

	QRect rect = getRect();
	if (rect.width()<width)
		rect.setWidth(width);
	if (rect.height()<height)
		rect.setHeight(height);
	setRect(rect);

	// Update cursor position
	setActualCursorPosition();

	// Update any elements linked with this one (zones/rooms with labels)
	updateLinkElements();
}

/** This method is called when the element gains it's edit mode */
void CMapText::editModeSetEvent(void)
{
	m_orgText = getText();
}

/** This method is called to insert a string at the cursor position */
void CMapText::insertString(QString s)
{
	QFontMetrics fm(m_font);
	//int oldHeight = (int)((fm.height() * text.count()) * yscale);
	QString str = m_text.at(m_cursorPos.y()-1);
	QString newStr;

	if ((int)str.length()>m_cursorPos.x())
	{
		newStr = str.left(m_cursorPos.x()) + s + str.right(str.length() - (m_cursorPos.x()));
	}
	else
	{
		newStr = str.left(m_cursorPos.x()) + s;
	}

	m_text.removeAt(m_cursorPos.y()-1);
	m_text.insert(m_cursorPos.y()-1,newStr);
	m_cursorPos.setX(m_cursorPos.x()+s.length());
	setActualCursorPosition();

	int newWidth = 	(int)((fm.width(str) * m_xscale) + (fm.width(s) * m_xscale));
	int newHeight = (int)((fm.height() * m_text.count()) * m_yscale);
	// FIXME_jp: Calc new height too

	if (newWidth>getWidth())
		setWidth(newWidth);

	if (newHeight>getHeight())
		setHeight(newHeight);
}

/** This will delete a character behind the cursor */
void CMapText::deleteChar(void)
{

	QString str = m_text.at(m_cursorPos.y()-1);
	if ((int)str.length()>m_cursorPos.x())
	{
		str.remove(m_cursorPos.x(),1);
		m_text.removeAt(m_cursorPos.y()-1);
		m_text.insert(m_cursorPos.y()-1,str);
	}
	else
	{
		if (m_cursorPos.y() < (int)m_text.count())
		{
			QString str2 = m_text.at(m_cursorPos.y());
			m_text.removeAt(m_cursorPos.y());
			m_text.removeAt(m_cursorPos.y()-1);
			m_text.insert(m_cursorPos.y()-1,str + str2);
		}
	}
}

/** This will delete a character infront of the cursor */
void CMapText::backspace(void)
{
	QFontMetrics fm(m_font);

	if (m_cursorPos.x()==0)
	{
		if (m_cursorPos.y()>1)
		{
			QString str = m_text.at(m_cursorPos.y()-1);
			m_text.removeAt(m_cursorPos.y()-1);
			QString str2 = m_text.at(m_cursorPos.y()-2);
			m_text.removeAt(m_cursorPos.y()-2);
			m_text.insert(m_cursorPos.y()-2,str2 + str);
			setCursor(QPoint(str2.length(),m_cursorPos.y()-1));
		}
	}
	else
	{
		QString str = m_text.at(m_cursorPos.y()-1);
		if (str.length()>0)
		{
			str.remove(m_cursorPos.x()-1,1);
			m_text.removeAt(m_cursorPos.y()-1);
			m_text.insert(m_cursorPos.y()-1,str);
			m_cursorPos.setX(m_cursorPos.x()-1);
			setActualCursorPosition();
/*
			for (char *s = text.frist();s!=0; s= text.next())
			{
			}
			if (getWidth() == oldWidth())
*/
		}
	}
}

/** this will insert a caridge return at the cursor */
void CMapText::insertCR(void)
{
	QString str = m_text.at(m_cursorPos.y()-1);
	m_text.removeAt(m_cursorPos.y()-1);
	m_text.insert(m_cursorPos.y()-1,str.left(m_cursorPos.x()));
	m_text.insert(m_cursorPos.y(),str.right(str.length() - m_cursorPos.x()));
	m_cursorPos.setX(0);
	m_cursorPos.setY(m_cursorPos.y()+1);
	setActualCursorPosition();
}

/** Move the cursor up */
void CMapText::cursorUp(void)
{
	if (m_cursorPos.y()>1)
	{
		QFontMetrics fm(m_font);
		int y = m_cursorOffset.y()-fm.height()-fm.height();
		setCursor(convertOffsetToCursor(QPoint(m_cursorOffset.x(),y)));
	}
}

/** Move the cursor down */
void CMapText::cursorDown(void)
{
	if (m_cursorPos.y()<(int)m_text.count())
	{
		QFontMetrics fm(m_font);
		int y = m_cursorOffset.y();

		QPoint p = convertOffsetToCursor(QPoint(m_cursorOffset.x(),y));

		setCursor(p);
	}
}

/** Move the cursor right */
void CMapText::cursorRight(void)
{
	int x = m_cursorPos.x();
	QString str = m_text.at(m_cursorPos.y()-1);
	if (x<(int)str.length())
	{
		m_cursorPos.setX(x+1);
		setActualCursorPosition();
	}
}

/** Move the cursor left */
void CMapText::cursorLeft(void)
{
	int x = m_cursorPos.x();
	if (x>0)
	{
		m_cursorPos.setX(x-1);
		setActualCursorPosition();
	}
}

/** Move the cursor to the end */
void CMapText::cursorEnd(void)
{
	QString str = m_text.at(m_cursorPos.y()-1);
	m_cursorPos.setX(str.length());
	setActualCursorPosition();
}


/** This method is called to restore the orginal text of the element. When editing, if
  * esc is pressed, then this method is called to restore the text to the value it had
  * before editing */
void CMapText::restoreText(void)
{
	setText(m_orgText);
}

/** This used internaly to calculate the offset of the cursor */
void CMapText::setActualCursorPosition(void)
{
	QFontMetrics fm(m_font);
	int y = fm.height() * m_cursorPos.y();
        int x = 0;
        if ((m_text.count() >= m_cursorPos.y()) && (m_cursorPos.y() > 0)) {
	  QString s = m_text.at(m_cursorPos.y()-1);
	  x = fm.width(s.left(m_cursorPos.x()));
        }

	m_cursorOffset.setX(x);
	m_cursorOffset.setY(y);
}

/** This is used to get the cursor position
  * @return The cursor position
  */
QPoint CMapText::getCursor(void)
{
	return m_cursorPos;
}

/** This method is over ridden from CMapElement::paint. It is used
  * draw the text on the map.
  * @param p A pointer to the paint device the text is to be drawn on
  */
void CMapText::setColor(QColor color)
{
	m_col = color;
}

/** This is used to return the color of the text
  * @return The color of the text
  */
QColor CMapText::getColor(void)
{
	return m_col;
}

/** Used to set the font of the text. This font is used when drawing the text.
  * @param f The required font of the text
  */
void CMapText::setFont(QFont f)
{
	m_font = f;

	setTextSize();
}

/** This menthod will return the font used to display the text
  * @return The font used to display text
  */
QFont CMapText::getFont(void)
{
	return m_font;
}

/** Gets the text of the text element
  * @return The text
  */
QString CMapText::getText(void)
{
  return m_text.join ("\n");
}

/** Sets the text of the text element
  * @param str The new string that the text element should be set to
  */
void CMapText::setText(QString str)
{
	stringToList(str,&m_text);

	setTextSize();
}

/** This method is used to convert a text string into a text list
  * @param str The string of text
  * @param textList The list to add the text to
  */
void CMapText::stringToList(QString str,QStringList *textList)
{
	textList->clear();
	if (str.isEmpty())
	{
		textList->append("");
	}
	else
	{
		int oldIndex = 0;
		int index = str.find('\n');

		while(index!=-1)
		{
			textList->append(str.mid(oldIndex,index-oldIndex ));

			oldIndex = index +1 ;
			index = str.find('\n',index+1);
		}

		textList->append(str.right(str.length()-oldIndex));
	}

}

/** This method is used to update the size of the text element */
void CMapText::setTextSize(void)
{
	QFontMetrics fm(m_font);
	int width = 0;
	int height = 0;

	for (QStringList::iterator it = m_text.begin(); it != m_text.end(); ++it)
	{
		if (fm.width(*it)>width)
			width = fm.width(*it);

		height+=fm.height();
	}

	if (height < 10)
		height = 10;

	if (width < 10)
		width = 10;

	QRect rect = getRect();
	rect.setWidth(width);
	rect.setHeight(height);
	setRect(rect);
	setActualCursorPosition();
}

void CMapText::updateLinkElements(void)
{
	if (m_linkElement)
	{
		if (m_linkElement->getElementType()==ROOM)
		{
			CMapRoom *room = (CMapRoom *)m_linkElement;
			room->setLabel(QString(m_text.first()));
			room->setLabelPosition(room->getLabelPosition());
		}
		if (m_linkElement->getElementType()==ZONE)
		{
			CMapZone *zone = (CMapZone *)m_linkElement;
			zone->setLabel(QString(m_text.first()));
			zone->setLabelPosition(zone->getLabelPosition());
		}
	}
}

/** This is used to create  a new copy of the text element and return
  * a pointer to the new text element
  * @return A pointer to the copy of the text element.
  */
CMapElement *CMapText::copy(void)
{
	CMapText *newText =  new CMapText (getText(),getFont(),getColor(),getManager(),getLowPos(),getLevel());
	return newText;
}

/** Used to load the properties of the element from a list of properties */
void CMapText::loadProperties(KConfigGroup properties)
{
	CMapElement::loadProperties(properties);

	setText(properties.readEntry("Text",getText()));
	QColor color=getColor();
	color=properties.readEntry("Color",color);
	setColor(color);
	QFont font = getFont();
	font = properties.readEntry("Font",font);
	setFont(font);

	if (properties.hasKey("LinkedType"))
	{
		CMapLevel *level = getManager()->findLevel(properties.readEntry("LinkedLevel",-1));

		if (level)
		{
			elementTyp typ = (elementTyp)properties.readEntry("LinkedType",(int)OTHER);
			if (typ == ROOM)
			{
				CMapRoom *room = level->findRoom(properties.readEntry("LinkedID",-1));
				room->setLabelPosition((CMapRoom::labelPosTyp)properties.readEntry("LabelPos",(int)CMapRoom::HIDE),this);
			}

			if (typ == ZONE)
			{
				CMapZone *zone = getManager()->findZone(properties.readEntry("LinkedID",-1));
				zone->setLabelPosition((CMapZone::labelPosTyp)properties.readEntry("LabelPos",(int)CMapZone::HIDE),this);
			}
		}
	}

	setTextID(properties.readEntry("TextID",m_ID));
}

/** Used to save the properties of the element to a list of properties */
void CMapText::saveProperties(KConfigGroup properties)
{
	CMapElement::saveProperties(properties);
	properties.writeEntry("Text",getText());
	properties.writeEntry("Color",getColor());
	properties.writeEntry("Font",getFont());
	properties.writeEntry("TextID",getTextID());	

	if (m_linkElement)
	{
		properties.writeEntry("LinkedType",(int)m_linkElement->getElementType());

		if (m_linkElement->getElementType()==ZONE)
		{
			CMapZone *zone = (CMapZone *)m_linkElement;
			properties.writeEntry("LinkedLevel",zone->getLevel()->getLevelID());
			properties.writeEntry("LinkedID",zone->getZoneID());
			properties.writeEntry("LabelPos",(int)zone->getLabelPosition());
		}

		if (m_linkElement->getElementType()==ROOM)
		{
			CMapRoom *room = (CMapRoom *)m_linkElement;
			properties.writeEntry("LinkedLevel",room->getLevel()->getLevelID());
			properties.writeEntry("LinkedID",room->getRoomID());
			properties.writeEntry("LabelPos",(int)room->getLabelPosition());
		}
	}
}

/** Used to save the element as an XML object
  * @param properties The XML object to save the properties too
  * @param doc The XML Document */
void CMapText::saveQDomElement(QDomDocument *doc,QDomElement *properties)
{
	writeColor(doc,properties,"Color",getColor());	

	CMapElement::saveQDomElement(doc,properties);
	properties->setAttribute("Text",getText());
	properties->setAttribute("Font",getFont().toString());
	properties->setAttribute("TextID",getTextID());
}

/** Used to load the properties from a XML object
  * @param properties The XML object to load the properties from */
void CMapText::loadQDomElement(QDomElement *properties)
{
	CMapElement::loadQDomElement(properties);

	setColor(readColor(properties,"Color",getColor()));
	setText(properties->attribute("Text",getText()));
	setTextID(readInt(properties,"TextID",getTextID()));

	QFont font;
	font.fromString(properties->attribute("Font"));
	setFont(font);
}

void CMapText::setTextID(unsigned int id)
{
	if (id > getZone()->m_text_id_count)
	  getZone()->m_text_id_count = id;

	m_ID = id;
}
	
