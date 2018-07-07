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
#include <QApplication>
#include <QAction>
#include <QFontDatabase>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include <KActionCollection>


class cTextOutputItem : public QGraphicsTextItem {
public:
  cTextOutputItem(bool sec) {
    isSecondary = sec;
    bgcolor = Qt::black;
  }

  void setBackgroundColor (QColor color) {
    bgcolor = color;
  }

  QPainterPath opaqueArea() const override {
    return shape();
  }

  void paint (QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *w) {
    painter->setBrush (bgcolor);
    painter->drawRect (boundingRect());
    QGraphicsTextItem::paint (painter, o, w);
  }

  virtual QRectF boundingRect() const override {
    QRectF rect = scene()->sceneRect();
    // add +1 to each side to eliminate the selection box, which we do not want
    return QRectF (-1, -1, rect.width() + 2, rect.height() + 2);
  }

  void updateSize () {
    prepareGeometryChange();
  }

protected:
  bool isSecondary;
  QColor bgcolor;
};

class cScrollTextGroup : public QGraphicsItemGroup
{
public:
  cScrollTextGroup() {
    _percentHeight = 25;
    setFlag (QGraphicsItem::ItemClipsChildrenToShape);
  }

  virtual QRectF boundingRect() const override {
    QGraphicsScene *sc = scene();
    if (sc->views().isEmpty()) return QRectF (0, 0, 0, 0);
    QGraphicsView *view = sc->views().first();
    double w = view->viewport()->width();
    double h = view->viewport()->height();
    h = h * _percentHeight / 100;
    return QRectF (0, 0, w, h);
  }

  void setPercentHeight (int ph) {
    updateSize();
    _percentHeight = ph;
  }

  int percentHeight () {
    return _percentHeight;
  }

  void updateSize () {
    prepareGeometryChange();
  }

protected:
  int _percentHeight;
};

class cConsole::Private {
  QGraphicsScene scene;
  cTextOutputItem *mainText, *scrollText;
  cScrollTextGroup *scrollTextGroup;
  QTextDocument *text;

  int historySize;
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
  d->historySize = 1000;

  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

  //and connect() slider so that aconsole is shown/hidden as needed
  connect (verticalScrollBar (), SIGNAL (sliderMoved (int)), this, SLOT (sliderChanged (int)));
  connect (verticalScrollBar (), SIGNAL (valueChanged (int)), this, SLOT (sliderChanged (int)));
    
  d->text = new QTextDocument;
  QString stylesheet = "* { color: " + QColor (Qt::lightGray).name() + "; white-space: pre-wrap; } a { color: " + QColor (Qt::blue).name() + ": } ";
  d->text->setDefaultStyleSheet (stylesheet);
  QTextOption opt;
  opt.setWrapMode (QTextOption::WrapAtWordBoundaryOrAnywhere);
  d->text->setDefaultTextOption (opt);

  //size policy
  QSizePolicy qsp (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setSizePolicy (qsp);
  setFocusPolicy (Qt::NoFocus);

  // scene
  setScene (&d->scene);
  // this needs to be set immediately to prevent an endless recursion
  scene()->setSceneRect (0, 0, viewport()->width(), viewport()->height());

  d->mainText = new cTextOutputItem (false);
  d->scrollText = new cTextOutputItem (true);
  d->scrollTextGroup = new cScrollTextGroup;
  d->scene.addItem (d->mainText);
  d->scene.addItem (d->scrollTextGroup);
  d->scene.addItem (d->scrollText);

  d->mainText->setDocument (d->text);
//  d->mainText->setFiltersChildEvents (true);
  d->mainText->setPos (0, 0);
  d->mainText->setTextInteractionFlags (Qt::TextBrowserInteraction);

  d->scrollTextGroup->setParentItem (d->mainText);
  d->scrollTextGroup->setVisible (false);

  d->scrollText->setDocument (d->text);
  d->scrollText->setParentItem (d->scrollTextGroup);
  d->scrollText->setPos (0, 0);
  d->scrollText->setTextInteractionFlags (Qt::TextBrowserInteraction);

  d->scene.setFocusItem (d->mainText);
  connect (&d->scene, &QGraphicsScene::changed, this, &cConsole::sceneChanged);

  //background color
  setDefaultBkColor (Qt::black);

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
  viewport()->setCursor (Qt::IBeamCursor);

  forceBeginOfLine ();
  fixupOutput();
}

cConsole::~cConsole() {
  setScene (nullptr);  // needed to prevent crashes in the destructor

  delete d->scrollText;
  delete d->mainText;
  delete d->text;
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
  setBackgroundRole (QPalette::Base);
  d->mainText->setBackgroundColor (color);
  d->scrollText->setBackgroundColor (color);
  update();
}

QColor cConsole::defaultBkColor () {
  return d->bgcolor;
}

void cConsole::setScrollTextVisible (bool vis)
{
  d->scrollTextGroup->setVisible (vis);
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
  d->scrollTextGroup->setPercentHeight (aconsize);
  adjustScrollBack();
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

void cConsole::setHistorySize (int size) {
  d->historySize = size;
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

int cConsole::totalLines() {
  return d->text->lineCount();
}

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

  // TODO - if the buffer is full, remove old blocks/lines
  while (totalLines() > d->historySize) {
    // check the height of the first block; if removing it won't put us below the history limit, remove it
    QTextBlock fblock = d->text->firstBlock();
    int flines = fblock.lineCount();
    if (totalLines() - flines < d->historySize) break;

    int fheight = d->text->documentLayout()->blockBoundingRect (fblock).height();
    cursor = QTextCursor (fblock);
    cursor.select (QTextCursor::BlockUnderCursor);
    cursor.movePosition (QTextCursor::NextCharacter, QTextCursor::KeepAnchor);  // need to move, otherwise the empty block is kept
    cursor.removeSelectedText();

    // scroll accordingly if needed
    QScrollBar *bar = verticalScrollBar();
    if (!d->atBottom) bar->setValue (bar->value() - fheight);
  }

  fixupOutput();
}

void cConsole::forceBeginOfLine () {
  addNewText (nullptr, true);
}

void cConsole::expireNamedLinks (const QString &name) {
  // TODO
}

void cConsole::addSelectionToClipboard (QClipboard::Mode clipboardMode) {
  QString selection = d->mainText->textCursor().selectedText();
  if (!selection.size()) return;
  selection = selection.replace (QChar::ParagraphSeparator, '\n');
  QApplication::clipboard()->setText (selection, clipboardMode);
}

void cConsole::lineUp () {
  QScrollBar *bar = verticalScrollBar();
  int by = bar->singleStep();
  bar->setValue (bar->value() - by);
}

void cConsole::lineDown () {
  QScrollBar *bar = verticalScrollBar();
  int by = bar->singleStep();
  bar->setValue (bar->value() + by);
}

void cConsole::pageUp () {
  QScrollBar *bar = verticalScrollBar();
  int by = bar->pageStep() * (100 - d->scrollTextGroup->percentHeight()) / 100;  // deduct backview size
  bar->setValue (bar->value() - by);
}

void cConsole::pageDown () {
  QScrollBar *bar = verticalScrollBar();
  int by = bar->pageStep() * (100 - d->scrollTextGroup->percentHeight()) / 100;  // deduct backview size
  bar->setValue (bar->value() + by);
}

void cConsole::resizeEvent (QResizeEvent *)
{
  fixupOutput();
}

// this is needed to resize the text display at startup
bool cConsole::viewportEvent(QEvent *event)
{
  if (event->type() == QEvent::Resize)
    fixupOutput();
  return QGraphicsView::viewportEvent (event);
}


void cConsole::adjustScrollBack ()
{
  // move the scrollback to its desired position
  int h = d->scrollTextGroup->boundingRect().height();
  QPointF scenepos = mapToScene (0, height() - h);
  // don't exceed the scene height
  double diff = scenepos.y() + h - d->scene.height();
  if (diff > 0) scenepos.setY (scenepos.y() - diff);
  // and shift the text viewer
  double ty = d->scrollText->boundingRect().height() - h;

  d->scrollTextGroup->setPos (scenepos);
  d->scrollText->setPos (0, -ty);
}

void cConsole::scrollContentsBy (int dx, int dy)
{
  QGraphicsView::scrollContentsBy (dx, dy);

  adjustScrollBack();
}

void cConsole::fixupOutput ()
{
  double h = max ((qreal) viewport()->height(), d->text->documentLayout()->documentSize().height());
  scene()->setSceneRect (0, 0, viewport()->width(), h);

  sceneChanged();
  d->mainText->updateSize();
  d->scrollText->updateSize();
  adjustScrollBack ();

  forceEmitSize ();
}

/*
void cConsole::activateLink (chunkLink *link, const QPoint &point)
{
  //two modes of operation, depending on whether this is a command-link or a URL-link
  if (link->isCommand())
  {
    QString cmd = link->target();
    bool toprompt = link->toPrompt();
    bool ismenu = link->isMenu();
    if (ismenu)
    {
      //get rid of old menu, if any
      delete linkMenu;
      
      link->parseMenu();
      
      //create the menu
      menuChunk = link;
      linkMenu = new KMenu (this);
      
      //insert all the items
      list<menuItem>::const_iterator it;
      for (it = menuChunk->menu().begin(); it != menuChunk->menu().end(); ++it)
        linkMenu->addAction ((*it).caption);
      connect (linkMenu, SIGNAL (triggered (QAction *)), this, SLOT (linkMenuItemHandler (QAction *)));
      
      linkMenu->popup (point);
    }
    else
    {
      if (toprompt)
        emit promptCommand (cmd);
      else
        emit sendCommand (cmd);
    }
  }
  else
  {
    QString url = link->target();
    KToolInvocation::invokeBrowser (url);
  }
}

void cConsole::linkMenuItemHandler (QAction *item)
{
  if (!linkMenu) return;
  int idx = linkMenu->actions().indexOf (item);
  if (idx == -1) return;  // not found
  bool toprompt = menuChunk->toPrompt();
  list<menuItem>::const_iterator it;
  QString cmd;
  it = menuChunk->menu().begin();
  for (int i = 0; i < idx; i++)
    ++it;
  cmd = (*it).command;
  if (toprompt)
    emit promptCommand (cmd);
  else
    emit sendCommand (cmd);

  linkMenu = 0;
  menuChunk = 0;
}
*/


/*
TODO SIGNALS - these must be emitted
void sendCommand (const QString &command);  -- in activateLink
void promptCommand (const QString &command);  -- in activateLink
*/


