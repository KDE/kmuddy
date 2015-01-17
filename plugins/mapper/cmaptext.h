/***************************************************************************
                               cmaptext.h
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

#ifndef CMAPTEXT_H
#define CMAPTEXT_H

#include "cmapelement.h"

/**The class used to store text data
  *@author Kmud Developer Team
  */

class QString;
class QStringList;
class QColor;
class QFont;
class QPoint;

#include "kmemconfig.h"

class KMUDDY_EXPORT CMapText : public CMapElement
{
public:
	CMapText(QString str,QFont f,QColor col,CMapManager *manager,QPoint pos,CMapLevel *level);
	CMapText(QString str,CMapManager *manager,QPoint pos,CMapLevel *level);
	~CMapText();

	elementTyp getElementType(void)             { return TEXT ; }
        virtual void setLevel(CMapLevel *level);

	/** Sets the text of the text element
	  * @param str The new string that the text element should be set to
	  */
	void setText(QString str);
	/** Gets the text of the text element
	  * @return The text
	  */
	QString getText(void);
	/** Used to set the font of the text. This font is used when drawing the text.
	  * @param f The required font of the text
	  */
	void setFont(QFont f);
	/** This menthod will return the font used to display the text
	  * @return The font used to display text
	  */
	QFont getFont(void);
	/** This is used to return the color of the text
	  * @return The color of the text
	  */
	QColor getColor(void);
	/** This method is used to set the color of the text
	  * @param color The color to set the text to
	  */
	void setColor(QColor color);

	/** Used to paint the text on the map
	  * @param p The painter used to paint the text
	  * @param zone The zone the text is being painted in
	  */
	void paint(QPainter *p,CMapZone *zone);
	void dragPaint(QPoint offset,QPainter *p,CMapZone *zone);
	void lowerPaint(QPainter *p,CMapZone *zone);
	void higherPaint(QPainter *p,CMapZone *zone);

	/** This is used to create  a new copy of the text element and return
	  * a pointer to the new text element
	  * @return A pointer to the copy of the text element.
	  */
	CMapElement *copy(void);

	void setLinkElement (CMapElement *element)   { m_linkElement = element; }
	CMapElement *getLinkElement(void)            { return m_linkElement; }
	void updateLinkElements(void);

	/** This is used to set the cursor position within the text element
	 * @param pos The cursor position
	 */
	void setCursor(QPoint pos);
	/** This is used to return the color of the text
	  * @return The color of the text
	  */
	QPoint getCursor(void);
	/** This is used to return the actual cords in the view of the cursor
	  * @return the cords */
	QPoint getCursorCords(void);
	/** Move the cursor left */
	void cursorLeft(void);
	/** Move the cursor right */
	void cursorRight(void);
	/** Move the cursor up */
	void cursorUp(void);
	/** Move the cursor down */
	void cursorDown(void);
	/** Move the cursor to the end */
	void cursorEnd(void);
	/** this will insert a caridge return at the cursor */
	void insertCR(void);
	/** This will delete a character behind the cursor */
	void deleteChar(void);
	/** This will delete a character infront of the cursor */
	void backspace(void);

	/** Used to load the properties of the element from a list of properties
	  * @param properties The properties to load into the element */
	virtual void loadProperties(KConfigGroup properties);
	/** Used to save the properties of the element to a list of properties
	  * @param properties The properties to load into the element */
	virtual void saveProperties(KConfigGroup properties);
    /** Used to save the element as an XML object
	  * @param properties The XML object to save the properties too
	  * @param doc The XML Document */
	virtual void saveQDomElement(QDomDocument *doc,QDomElement *properties);
	/** Used to load the properties from a XML object
	  * @param properties The XML object to load the properties from */
	virtual void loadQDomElement(QDomElement *properties);
	
	/** This is used to convert mouse cordinates into a cursor position
	  * @param mousePos The position of the mose */
	QPoint convertPosToCursor(QPoint mousePos);
	/** This is used to conver a offset from the element orgin into a cursor position
	  * @param offset The offset of the curosr */
	QPoint convertOffsetToCursor(QPoint offset);

	/** This method is called to insert a string at the cursor position
	  * @param s The string to insert */
	void insertString(QString s);
	/** This method is called to restore the orginal text of the element. When editing, if
	  * esc is pressed, then this method is called to restore the text to the value it had
	  * before editing */
	void restoreText(void);

	/** This is used to convert a Actual size to it's closet font size
	  * @param size The actual size of the font
	  * @param font The text font
	  * @return The closet matching font size */
	static int getActualToFontSize(QSize size,QFont font,QStringList *text);
	/** This is used to paint the text. It is capable of painting multiline
	  * of text. It is static so that it can be called with out creating a instance of CMapText
	  * @param p The painter used to paint the text
	  * @param col The color to paint the text
	  * @param pos The position to paint the text
	  * @param font The Font used to paint the text
	  * @param text A point to the text list
	  * @param size The size of the text */
	static void paintText(QPainter *p,QColor col,QPoint pos,QFont font,QStringList *text,QSize size);
	/** This method is used to convert a text string into a text list
	  * @param str The string of text
	  * @param textList The list to add the text to
	  */
	static void stringToList(QString str,QStringList *textList);
	/**
	 * This method is used to calcualte the scale that the text should be scale by
	 * @param text A pointer to the text list
	 * @param requiredSize The size of the text
	 * @param xscale Used to return the x axis scale value
	 * @param yscale Used to return the y axis scale value
	 */
	static void getScale(QFont font,QStringList *text,QSize requiredSize,double *xscale,double *yscale);
	/** This is used to get a unique ID for the text */
	unsigned int getTextID(void)                                   { return m_ID; }
	/** This is used to set the ID of the text */
	void setTextID(unsigned int id);


protected:
	/** This method is called when the element looses it's edit mode */
	virtual void editModeUnsetEvent(void);
	/** This method is called when the element gains it's edit mode */
	virtual void editModeSetEvent(void);
	/** Used to paint the element at a given location and size
	  * @param p The painer to paint the element to
	  * @param pos The position to paint the elmenet
	  * @param size The size the element should be draw
	  * @param zone The current zone being viewed */
	virtual void paintElementResize(QPainter *p,QPoint pos,QSize size,CMapZone *zone);

private:
	/** This used internaly to calculate the offset of the cursor */
	void setActualCursorPosition(void);
	/** This is used to paint the text. It is capable of painting multiline
	  * of text. It is static so that it can be called with out creating a instance of CMapText
	  * @param p The painter used to paint the text
	  * @param col The color to paint the text
	  * @param pos The position to paint the text
	  * @param font The Font used to paint the text
	  * @param text A point to the text list */
	static void paintText(QPainter *p,QColor col,QPoint pos,QFont font,QStringList *text);
	/** This method is used to update the size of the text element */
	void setTextSize(void);

private:
	/** Used to store the last x scale value that was used when painting */
	double m_xscale;
	/** Used to store the last y scale value that was used when painting */
	double m_yscale;
	QString m_orgText;
	QColor m_col;
	QStringList m_text;
	QFont m_font;
	CMapElement *m_linkElement;
	QPoint m_cursorPos;
	QPoint m_cursorOffset;
	unsigned int m_ID;
};

#endif
