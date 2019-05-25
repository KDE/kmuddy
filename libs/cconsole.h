/***************************************************************************
                          cconsole.h  -  main displaying widget
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So Jun 22 2017
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

#ifndef CCONSOLE_H
#define CCONSOLE_H

#define TRANSCRIPT_PLAIN 2
#define TRANSCRIPT_ANSI 3
#define TRANSCRIPT_HTML 1 

#include <kmuddy_export.h>

#include <QClipboard>
#include <QFile>
#include <QGraphicsView>
#include <QStringList>
class cTextChunk;

//maximum cache size
#define MAXCACHE 50

/**
Main console - displays output of the MUD server...
This is a new version of the displaying widget, replacing the old QTableView-based widget.
*/

class KMUDDY_EXPORT cConsole : public QGraphicsView {
   Q_OBJECT
public: 
  cConsole(QWidget *parent = Q_NULLPTR);
  ~cConsole();
  void setSession (int s);

  /** set font */
  void setFont (QFont f);
  /** get currently used font */
  QFont font ();
  
  void setDefaultBkColor (QColor color);
  QColor defaultBkColor ();

  void setIndentation (int val);
  void setEnableBlinking (bool value);  
  
  /** return current number of rows */
  int curRows();
  /** return current number of columns */
  int curCols();

  /** Is the scroll text visible? */
  void setScrollTextVisible (bool vis);
  /** Size of the secondary console shown while scrolling */
  void setScrollTextSize (int aconsize);

  /** forces amission of dimensionsChanged signal; used by toolbar hiding
  functions, where this fails for unknown reasons */
  void forceEmitSize ();
  /** dump all history buffer to that file */
  void dumpBuffer (bool fromcurrent, QFile &file, char dumpType);
  void setHistorySize (int size);

  /** How many lines in total does the console have currently? */
  int totalLines();
  QStringList words (QString prefix, int minLength = 3);
  /** clear the widget */
  void clear ();
  /** adds line to the widget */
  void addLine (cTextChunk *chunk);
  /** as addLine, but does not end line */
  void addText (cTextChunk *chunk);
  /** ensure that our current position is at beginning of a line */
  void forceBeginOfLine ();

  /** expire all links with a given name, or all named links if no name given */
  void expireNamedLinks (const QString &name = QString());
  
public slots:
  /** adds selection to clipboard (adding to mouse selection buffer is done
      automatically) */
  void addSelectionToClipboard (QClipboard::Mode clipboardMode);
  void linkHovered (const QString &link);
  void linkActivated (const QString &link);

  /** shifting it around (SHIFT+keys) */
  void lineUp ();
  void lineDown ();
  void pageUp ();
  void pageDown ();

signals:
  void dimensionsChanged (int cols, int rows);
  void sendCommand (const QString &command);
  void promptCommand (const QString &command);

protected slots:
  void sliderChanged (int val);
  void sceneChanged (const QList<QRectF> &region = QList<QRectF>());

protected:
  void resizeEvent (QResizeEvent *e) override;
  bool viewportEvent(QEvent *event) override;
  virtual void scrollContentsBy (int dx, int dy) override;
  /** called when resizing and when changing font */
  void fixupOutput (bool sizeChanged = false);
  void adjustScrollBack ();

  void addNewText (cTextChunk *chunk, bool endTheLine);

  class Private;
  Private *d;
};

#endif
