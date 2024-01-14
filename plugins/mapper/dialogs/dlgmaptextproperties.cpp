/***************************************************************************
                          dlgmaptextproperties.cpp  -  description
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

#include "dlgmaptextproperties.h"

#include <qfile.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpalette.h>
#include <qfontdatabase.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qfontmetrics.h>
#include <QScrollArea>
#include <QPixmap>
#include <QVBoxLayout>
#include <QDebug>

#include <kcolorbutton.h>
#include <knuminput.h>
#include <KLocalizedString>

#include "../cmaptext.h"
#include "../cmapmanager.h"
#include "../cmapview.h"
#include "../cmapcmdelementcreate.h"
#include "../cmapcmdelementdelete.h"
#include "../cmapcmdelementproperties.h"
#include "../cmapcmdgroup.h"

#include "../cmappluginbase.h"
#include "../cmappropertiespanebase.h"

CMapTextPreview::CMapTextPreview(CMapManager *manager,QWidget *parent)
	: QWidget(parent)
{
  setAttribute (Qt::WA_StaticContents);
  buffer = nullptr;
  mapManager = manager;
}

CMapTextPreview::~CMapTextPreview()
{
	if (buffer)
		delete buffer;
}

void CMapTextPreview::drawContents(QPainter *paint,int , int , int, int )
{
	QRect drawArea(0,0,width(),height());
	
	// delete the buffer only when we need one with a different size
	if (buffer && (buffer->size() != drawArea.size()))
	{
		delete buffer;
		buffer = nullptr;
	}

	if (!buffer)
	{
		buffer = new QPixmap(drawArea.size());
	}

	QPainter p;

	p.begin(buffer);

	if (mapManager->getActiveView()->getCurrentlyViewedZone()->getUseDefaultBackground())
	{
		p.fillRect(drawArea,mapManager->getMapData()->backgroundColor);
	}
	else
	{
		p.fillRect(drawArea,mapManager->getActiveView()->getCurrentlyViewedZone()->getBackgroundColor());
	}

	QStringList textList;
	CMapText::stringToList(text,&textList);
	CMapText::paintText(&p,color,QPoint(0,0),font,&textList,size);

	paint->drawPixmap(0,0,*buffer);
}


DlgMapTextProperties::DlgMapTextProperties(CMapManager *manager,CMapText *textElement,QWidget *parent) : QDialog(parent)
{
  setupUi (this);

	text = textElement;
	mapManager = manager;
	QVBoxLayout *vbox = new QVBoxLayout((QWidget *)fraPreview);
	textScrollView = new CMapTextPreview(mapManager,fraPreview);
        QScrollArea *textScrollArea = new QScrollArea (fraPreview);
        textScrollArea->setWidget (textScrollView);
	vbox->addWidget( textScrollArea);
	textScrollView->show();
	fillFamilyList();
	setFont(text->getFont());
	QString width = QString::number (text->getWidth());
	QString height = QString::number (text->getHeight());
	txtText->setText(text->getText());
	txtWidth->setText(width);
	txtHeight->setText(height);
	cmdColor->setColor(text->getColor());

	//FIXME_jp: set txtText background to background color of the map

	// Get the extension panels from the plugins
	QList<CMapPropertiesPaneBase *> paneList = mapManager->createPropertyPanes(TEXT,(CMapElement*)textElement,(QWidget *)TextTabs);
	foreach (CMapPropertiesPaneBase *pane, paneList)
	{
		TextTabs->addTab(pane,pane->getTitle());
		connect(cmdOk,SIGNAL(clicked()),pane,SLOT(slotOk()));
		connect(cmdCancel,SIGNAL(clicked()),pane,SLOT(slotCancel()));
	}

	slotUpdatePreview();
}

DlgMapTextProperties::~DlgMapTextProperties()
{
}

void DlgMapTextProperties::fillFamilyList(void)
{
  lstFamily->insertItems (0, QFontDatabase().families());
}

void DlgMapTextProperties::setFont(QFont font)
{
	textFont = font;
	
	QString family = font.family();
	family = family.trimmed();
	QString size = QString::number(font.pointSize());
	
	for (int i = 0 ; i<lstFamily->count();i++)
	{
		QString s = lstFamily->item(i)->text();
		if (s == family)
		{
			lstFamily->setCurrentRow(i);
			break;
		}
	}	

	chkBold->setChecked(font.bold());
	chkItalic->setChecked(font.italic());
	lstFamily->scrollToItem(lstFamily->currentItem());
	slotUpdatePreview();
}

void DlgMapTextProperties::slotSetSize(void)
{
	qDebug() << "CMapTextPreview::slotSetSize1 ";
	int fontSize = txtFontSize->text().toInt();
	textFont.setPointSize(fontSize);
	QFontMetrics fm(textFont);	
	QString width;
	QString height;
	QStringList textList;
	CMapText::stringToList(txtText->toPlainText(),&textList);
	int tmpWidth = 0;
	for (QStringList::iterator it = textList.begin(); it != textList.end(); ++it)
	{
		if (fm.width(*it) > tmpWidth)
			tmpWidth = fm.width(*it);
    }

	QString width = QString::number(tmpWidth);
	QString height = QString::number(fm.height() * textList.count());
	txtWidth->setText(width);
	txtHeight->setText(height);
	txtFontSize->setText("");
	slotUpdatePreview();
}

void DlgMapTextProperties::slotBoldClicked(void)
{
	textFont.setBold(chkBold->isChecked());
	slotUpdatePreview();
}

void DlgMapTextProperties::slotItalicClicked(void)
{
	textFont.setItalic(chkItalic->isChecked());
	slotUpdatePreview();
}

void DlgMapTextProperties::slotFamilySelected()						
{
	textFont.setFamily(lstFamily->currentItem()->text());
	slotUpdatePreview();
}

void DlgMapTextProperties::slotColorChanged(const QColor &color)
{
	textColor = color;
	slotUpdatePreview();
}

void DlgMapTextProperties::slotAccept()
{
	CMapCmdElementProperties *command = new CMapCmdElementProperties(mapManager,i18n("Changed Room Properties"),text);

	QStringList textList;
	int width = txtWidth->text().toInt();
	int height = txtHeight->text().toInt();

	command->compare("Text",text->getText(),txtText->toPlainText());
	command->compare("Color",text->getColor(),textColor);
	command->compare("Font",text->getFont(),textFont);
	command->compare("Size",text->getSize(),QSize(width,height));

	mapManager->addCommand(command);
}

void DlgMapTextProperties::slotUpdatePreview()
{
	int gridWidth = mapManager->getMapData()->gridSize.width();
	int gridHeight = mapManager->getMapData()->gridSize.height();
	int width =txtWidth->text().toInt();
	if (width<gridWidth)
		width = gridWidth;
	int height = txtHeight->text().toInt();
	if (height < gridHeight)
		height = 20;

	textScrollView->setFont(textFont);
	textScrollView->setColor(textColor);
	textScrollView->setSize(QSize(width,height));
	textScrollView->setText(txtText->toPlainText());
	textScrollView->resize(txtWidth->text().toInt(),txtHeight->text().toInt());
	textScrollView->update();
}

#include "moc_dlgmaptextproperties.cpp"
