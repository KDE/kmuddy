/***************************************************************************
                          cconsole.cpp  -  main displaying widget v2
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So Dec 21 2017
    copyright            : (C) 2002-2017 by Tomas Mecir
    email                : mecirt@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cconsole.h"

#include "cactionmanager.h"
#include "ctextchunk.h"

#include <QAbstractTextDocumentLayout>
#include <QAction>
#include <QFontDatabase>
#include <QGraphicsTextItem>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextDocument>

#include <KActionCollection>

class cTextOutputItem : public QGraphicsTextItem {
public:
  cTextOutputItem(bool sec) {
    isSecondary = sec;
    percentHeight = sec ? 25 : 0;
  }

  virtual QRectF boundingRect() const override {
    QGraphicsScene *sc = scene();
    if (sc->views().isEmpty()) return QRectF (0, 0, 0, 0);
    QGraphicsView *view = sc->views().first();
    double w = view->viewport()->width();
    double h = view->viewport()->height();
    if (isSecondary) h = h * percentHeight / 100;
    else h = max (h, document()->documentLayout()->documentSize().height());
    return QRectF (0, 0, w, h);
  }

  void setPercentHeight (int ph) {
    percentHeight = ph; updateSize();
  }

  void updateSize () {
    prepareGeometryChange();
  }

protected:
  bool isSecondary;
  int percentHeight;
};

class cConsole::Private {
  QGraphicsScene scene;
  cTextOutputItem *mainText, *scrollText;
  QTextDocument *text;

  QColor bgcolor;
  QFont font;
  int sess;
  int charWidth, charHeight;
  bool wantNewLine;
  bool atBottom;

  friend class cConsole;
};




cConsole::cConsole(QWidget *parent) : QGraphicsView(parent) {
  d = new Private;
  d->sess = -1;
  d->charWidth = 12;
  d->charHeight = 12;
  d->wantNewLine = false;
  d->atBottom = true;

  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

  //and connect() slider so that aconsole is shown/hidden as needed
  connect (verticalScrollBar (), SIGNAL (sliderMoved (int)), this, SLOT (sliderChanged (int)));
  connect (verticalScrollBar (), SIGNAL (valueChanged (int)), this, SLOT (sliderChanged (int)));
    
  //size policy
  QSizePolicy qsp (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setSizePolicy (qsp);

  d->text = new QTextDocument;
  QString stylesheet = "body { color: " + QColor (Qt::lightGray).name() + "; } ";
  d->text->setDefaultStyleSheet (stylesheet);
  QTextOption opt;
  opt.setWrapMode (QTextOption::WrapAtWordBoundaryOrAnywhere);
  d->text->setDefaultTextOption (opt);

  d->mainText = new cTextOutputItem (false);
  d->scrollText = new cTextOutputItem (true);
  d->scene.addItem (d->mainText);
  d->scene.addItem (d->scrollText);

  d->mainText->setDocument (d->text);
  d->mainText->setFiltersChildEvents (true);
  d->mainText->setPos (0, 0);

  d->scrollText->setDocument (d->text);
  d->scrollText->setParentItem (d->mainText);
  d->scrollText->setFocusProxy (d->mainText);
  d->scrollText->setVisible (false);

  setScene (&d->scene);
  d->scene.setFocusItem (d->mainText);
  connect (&d->scene, &QGraphicsScene::changed, this, &cConsole::sceneChanged);

  //background color
  d->bgcolor = Qt::black;
  QPalette pal = palette();
  pal.setColor (backgroundRole(), d->bgcolor);
  pal.setColor (QPalette::Base, d->bgcolor);
  setPalette (pal);
  setBackgroundRole (QPalette::Base);

  //context menu
  setContextMenuPolicy (Qt::ActionsContextMenu);
  KActionCollection *acol = cActionManager::self()->getACol();
  QAction *showmenubar = acol->action ("ShowMenuBar");
  QAction *fullscreenmode = acol->action ("SetFullScreen");
  QAction *clipcopy = acol->action ("ClipboardCopy");
  QAction *pastemenu = acol->action ("PasteMenu");
  QAction *sep1 = new QAction (this);
  QAction *sep2 = new QAction (this);
  sep1->setSeparator (true);
  sep2->setSeparator (true);
  if (clipcopy) addAction (clipcopy);
  if (pastemenu) addAction (pastemenu);
  addAction (sep1);
  if (showmenubar) addAction (showmenubar);
  addAction (sep2);
  if (fullscreenmode) addAction (fullscreenmode);

  setFont (QFontDatabase::systemFont (QFontDatabase::FixedFont)); //default system fixed font
  setCursor (Qt::IBeamCursor);

  fixupOutput();
}

cConsole::~cConsole() {
  delete d->scrollText;
  delete d->mainText;
  delete d->text;
  // TODO
  delete d;
}

void cConsole::setSession (int s) {
  d->sess = s;
}

void cConsole::setFont (QFont f) {
  d->font = f;
  d->text->setDefaultFont (d->font);

  QFontMetrics fm (f);
  d->charWidth = fm.width ("m");
  d->charHeight = fm.lineSpacing();

  fixupOutput();
}

QFont cConsole::font () {
  return d->font;
}

void cConsole::setDefaultBkColor (QColor color) {
  d->bgcolor = color;
  QPalette pal = palette();
  pal.setColor (backgroundRole(), d->bgcolor);
  pal.setColor (QPalette::Base, d->bgcolor);
  setPalette (pal);
  update();
}

QColor cConsole::defaultBkColor () {
  return d->bgcolor;
}

void cConsole::setScrollTextVisible (bool vis)
{
  d->scrollText->setVisible (vis);
}

void cConsole::sliderChanged (int val)
{
  int maxval = verticalScrollBar()->maximum ();
  d->atBottom = (val >= maxval);
  bool vis = (val < maxval);
  setScrollTextVisible (vis);
}

void cConsole::sceneChanged (const QList<QRectF> &region)
{
  // move back to the bottom if we were
  if (!d->atBottom) return;
  QScrollBar *sb  = verticalScrollBar();
  sb->setValue (sb->maximum());
}

void cConsole::setScrollTextSize (int aconsize)
{
  d->scrollText->setPercentHeight (aconsize);
}

void cConsole::setIndentation (int val) {
  // TODO
}

void cConsole::setEnableBlinking (bool value) {
  // TODO
}  

int cConsole::curRows() {
  return height() / d->charHeight;
}

int cConsole::curCols() {
  if (d->charWidth <= 0) return 0;
  return width() / d->charWidth;
}

void cConsole::setRepaintCount (int val) {
  // TODO
}

void cConsole::forceEmitSize () {
  emit dimensionsChanged (curCols(), curRows());
}

void cConsole::dumpBuffer (bool fromcurrent, QFile &file, char dumpType) {
  // TODO
}

void cConsole::tryUpdateHistorySize () {
  // TODO
}

void cConsole::setInitialHistorySize (int size) {
  // TODO
}

QStringList cConsole::words (QString prefix, int minLength) {
  QStringList res;

  // TODO

  return res;
}

void cConsole::clear () {
  d->text->clear();
  update();
}

void cConsole::addLine (cTextChunk *chunk) {
  addNewText (chunk, true);
}

void cConsole::addText (cTextChunk *chunk) {
  addNewText (chunk, false);
}

#include <QDebug>
void cConsole::addNewText (cTextChunk *chunk, bool endTheLine)
{
  QTextCursor cursor (d->text);
  cursor.movePosition (QTextCursor::End);
  if (chunk) {
    if (d->wantNewLine) {
      cursor.insertBlock ();
      d->wantNewLine = false;
    }

    cursor.insertHtml (chunk->toHTML());
  }
  if (endTheLine) d->wantNewLine = true;

  d->mainText->updateSize();
  d->scrollText->updateSize();
}

void cConsole::forceBeginOfLine () {
  addNewText (nullptr, true);
}

void cConsole::expireNamedLinks (const QString &name) {
  // TODO
}

void cConsole::addSelectionToClipboard (QClipboard::Mode clipboardMode) {
  // TODO
}

void cConsole::lineUp () {
  // TODO
}

void cConsole::lineDown () {
  // TODO
}

void cConsole::pageUp () {
  // TODO
}

void cConsole::pageDown () {
  // TODO
}

void cConsole::resizeEvent (QResizeEvent *)
{
  fixupOutput();
}

void cConsole::fixupOutput ()
{
  d->mainText->updateSize();
  d->scrollText->updateSize();

  forceEmitSize ();
}


/*
TODO SIGNALS - these must be emitted
void sendCommand (const QString &command);  -- in activateLink
void promptCommand (const QString &command);  -- in activateLink
*/


