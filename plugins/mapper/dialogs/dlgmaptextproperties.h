/***************************************************************************
                          dlgmaptextproperties.h  -  description
                             -------------------
    begin                : Thu Mar 8 2001
    copyright            : (C) 2001 by KMud Development Team
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

#ifndef DLGMAPTEXTPROPERTIES_H
#define DLGMAPTEXTPROPERTIES_H

#include <QDialog>
#include <qfont.h>
#include <qcolor.h>
#include <q3scrollview.h>
#include <qpixmap.h>
#include "ui_dlgmaptextpropertiesbase.h"

class CMapText;
class CMapManager;

class CMapTextPreview : public Q3ScrollView
{
public:
	CMapTextPreview(CMapManager *manager,QWidget *parent=0);
	~CMapTextPreview();

	void setColor(QColor textColor)           { color = textColor; }
	void setFont(QFont textFont)                { font = textFont; }
	void setSize(QSize textSize)                { size = textSize; }
	void setText(QString strText)               { text = strText; }
	
protected:
	void drawContents(QPainter *paint,int , int , int, int );
	//void paintEvent(QPaintEvent *e);

private:
	QColor color;
	QFont font;
	QSize size;
	QString text;
	QPixmap *buffer;
	CMapManager *mapManager;
};


/**The map text properties dialog
  *@author KMud Development Team
  */

class DlgMapTextProperties : public QDialog, private Ui::DlgMapTextPropertiesBase
{
   Q_OBJECT
public: 
	DlgMapTextProperties(CMapManager *manager,CMapText *textElement,QWidget *parent=0);
	~DlgMapTextProperties();

private:
	void fillFamilyList(void);
	void setFont(QFont font);

private slots:
	void slotSetSize(void);
	void slotFamilySelected(int index);
	void slotItalicClicked(void);
	void slotBoldClicked(void);
	void slotColorChanged(const QColor &color);
	void slotAccept();
	void slotUpdatePreview();

private:
	QColor textColor;
	QFont textFont;
	CMapText *text;
	CMapManager *mapManager;
	CMapTextPreview *textScrollView;
};

#endif
