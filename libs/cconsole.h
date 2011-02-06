/***************************************************************************
                          cconsole.h  -  main displaying widget
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So Jun 22 2002
    copyright            : (C) 2002-2004 by Tomas Mecir
    email                : kmuddy@kmuddy.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define TRANSCRIPT_PLAIN 2
#define TRANSCRIPT_ANSI 3
#define TRANSCRIPT_HTML 1 

#ifndef CCONSOLE_H
#define CCONSOLE_H

#include <q3gridview.h>
#include <qclipboard.h>
#include <qdatetime.h>

#include <stdio.h>

#include <kmuddy_export.h>

class QEvent;
class QPainter;
class QPixmap;
class KMenu;
class KToggleAction;

class cTextChunk;
class chunkLink;

/**
Class cHistoryBuffer - holds the whole buffer, rotates it when needed and provides access to it.
Inspired by a similar class from Alex Bache.
*/

class KMUDDY_EXPORT cHistoryBuffer {
 public:
  /** create the cyclic buffer of a given size */
  cHistoryBuffer (int size);
  /** destructor */
  ~cHistoryBuffer ();
  /** current number of items in the buffer */
  int items () { return _items; };
  /** current size of the buffer */
  int size () { return _size; };
  /** add a new line to the buffer */
  void add (cTextChunk *chunk);
  /** Returns one line in the buffer.
  Returns cTextChunk*, not cTextChunk*& - so that we can modify the chunk contents, but we
  cannot assign another chunk to any given position,*/
  cTextChunk * operator[] (int idx);
  /** flush the buffer */
  void flush ();
 protected:
  int _size, _items;
  /** position in the cyclic buffer, where the NEXT added item will go */
  int curidx;
  /** the actual buffer */
  cTextChunk **buffer;
};

//maximum cache size
#define MAXCACHE 50

/**
Main console - displays output of the MUD server...
Inherits QGridView - has one column and one row per line (each cell contains
one line of text - removes flicker)

Includes another cConsole object - used to display last few lines of output
while scrolling the main console...
  *@author Tomas Mecir
  */

class KMUDDY_EXPORT cConsole : public Q3GridView {
   Q_OBJECT
public: 
  /** auxiliary console is used to display last few lines when scrolling... */
  cConsole(bool auxiliary=false, QWidget *parent=0, const char *name=0);
  ~cConsole();
  void setSession (int s);

  /** get auxiliary console */
  cConsole *auxConsole ();

  /** set font */
  void setFont (QFont f);
  /** get currently used font */
  QFont font () { return myfont; };
  
  void setDefaultBkColor (QColor color);
  QColor defaultBkColor () { return bgcolor; };

  void setWordWrapping (bool val);
  void setIndentation (int val);
  
  void setEnableBlinking (bool value);  
  
  int wrapPos() { return wrappos; };
  void setWrapPos (int wp);

  int numCols () { return charCount; };
  void setNumCols (int numcols) { charCount = numcols; };
  int cellWidth () { return charWidth; };
  void setCellWidth (int) { /* do nothing! */ };

  /** return current number of rows */
  int curRows() { return currows; };
  /** return current number of columns */
  int curCols() { return curcols; };

  /** number of lines until a forced repaint */
  void setRepaintCount (int val) { repaintCount = val; };

  /** forces amission of dimensionsChanged signal; used by toolbar hiding
  functions, where this fails for unknown reasons */
  void forceEmitSize ();
  /** dump all history buffer to that file */
  void dumpBuffer (bool fromcurrent, FILE *file, char dumpType);
  void tryUpdateHistorySize ();
  static void setInitialHistorySize (int size) { initHistory = size; };

  QStringList words (QString prefix, int minLength = 3);
  /** clear the widget */
  void clear ();
  /** adds line to the widget */
  void addLine (cTextChunk *chunk);
  /** as addLine, but does not end line */
  void addText (cTextChunk *chunk);
  /** ensure that our current position is at beginning of a line */
  void forceBeginOfLine ();
  /** adds selection to clipboard (adding to mouse selection buffer is done
      automatically) */

  /** expire all links with a given name, or all named links if no name given */
  void expireNamedLinks (const QString &name = QString());
  
public slots:
  void addSelectionToClipboard (QClipboard::Mode clipboardMode);

  /** shifting it around (SHIFT+keys) */
  void lineUp ();
  void lineDown ();
  void pageUp ();
  void pageDown ();

  /** resizing aconsole (CTRL+keys) */
  void aconUp ();
  void aconDown ();
signals:
  void dimensionsChanged (int cols, int rows);
  void sendCommand (const QString &command);
  void promptCommand (const QString &command);
protected slots:
  void sliderChanged (int value);
  void blinkTimerTimeout ();
  /** handler for the link menu */
  void linkMenuItemHandler (QAction *item);
protected:
  /** called when a cell is to be painted */
  void paintCell (QPainter *p, int row, int);
  /** resize event */
  void resizeEvent (QResizeEvent *e);
  /** called when resizing and when changing font */
  void fixupOutput ();
  /** updates a range of cells - used in events below */
  void updateRowRange (int r1, int r2);
 
  /** mouse press event */
  void contentsMousePressEvent (QMouseEvent *e);
  /** mouse release event */
  void contentsMouseReleaseEvent (QMouseEvent *e);
  /** mouse move event */
  void contentsMouseMoveEvent (QMouseEvent *e);
  /** mouse double-click event */
  void contentsMouseDoubleClickEvent (QMouseEvent *e);
  /** mouse triple-click event */
  void mouseTripleClickEvent (QMouseEvent *e);
  /** disables scrolling aux.console using mouse wheel */
  void wheelEvent (QWheelEvent *e);
  /** Event catch-all, used to catch the tooltip event. */
  bool event (QEvent *e);

  /** activate a link - called when user clicks on it */
  void activateLink (chunkLink *link, const QPoint &point);
    
  /** add new text; called by addLine and addText */
  void addNewText (cTextChunk *chunk, bool endTheLine);
  /** handles necessary updates when new line needs to be created */
  void handleBufferShift ();
  /** end current line */
  void endLine ();
  /** make the redrawing cache empty */
  void emptyCache ();
  /** delete one line (=row) from the redrawing cache */
  void deleteLineFromCache (int line);
  /** delete one item from the cache */  
  void deleteCacheEntry (int which);

  /** are there blinking characters on that line? */
  bool hasBlink (int row);
    
  QPixmap *cacheBlinkOn[MAXCACHE], *cacheBlinkOff[MAXCACHE];
  int cacheEntry[MAXCACHE];
  /** ON = blinking text shown, OFF = blinking text hidden */
  bool blinkPhase;
  QTimer *blinkTimer;
 
  KToggleAction *fullscreenmode, *showmenubar;

  //triple click time
  QTime triple_click_time;

  /** background color */
  QColor bgcolor;
  
  /** wordwrapping / indentation / wrap position */
  bool wordWrapping;
  int indentValue, wrappos;

  /** new-line counters - used to prevent the "black-out" effect */
  int repaintCount, newlineCounter;
  
  /** blinking ON/OFF */
  bool blinking;
  
  /** currently used font */
  QFont myfont;
  /** descent of the currently used font, needed for drawing */
  int descent;
  /** width of one character */
  int charWidth;
  /** number of characters in one line */
  int charCount;

  /** number of added rows during current addNewText call */
  int pendingLineMove;

  /** current console size */
  int currows, curcols;
  
  /** session */
  int sess;
  
  /** our auxiliary console */
  cConsole *aconsole;

  /** are WE the auxiliary console? */
  bool aux;
  /** is the auxiliary console visible? */
  bool aconvisible;
  /** size of auxiliary console (in percents) */
  int aconsize;

  //is the hand cursor displayed (over a link)?
  bool ishandcursor;
  
  /** the link menu */
  chunkLink *menuChunk;
  KMenu *linkMenu;
  
  //selection
  int selrow1, selrow2, selrowpos1, selrowpos2;
  int previousr, previousc;
  bool selected, canselect;

  /** initial history size */
  static int initHistory;

  //THE DATA
  int usedrows;
  int currow;
  int currowpos;
  int historySize;

  cHistoryBuffer *buffer;
};

#endif
