/***************************************************************************
                          cconsole.cpp  -  main displaying widget
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : So Jun 22 2002
    copyright            : (C) 2002-2007 by Tomas Mecir
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

#define CCONSOLE_CPP

#include "cconsole.h"

#include "cactionmanager.h"
#include "cansiparser.h"
#include "ctextchunk.h"

#include <QAction>
#include <QClipboard>
#include <QEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QToolTip>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kmenu.h>
#include <ktoolinvocation.h>

#include <ctype.h>

class cANSIParser;

#define BLINK_INTERVAL 1000

cHistoryBuffer::cHistoryBuffer (int size)
{
  _size = size;
  _items = 0;
  curidx = 0;
  typedef cTextChunk *ucTextChunk;
  buffer = new ucTextChunk[_size];
  for (int i = 0; i < _size; i++)
    buffer[i] = 0;
}

cHistoryBuffer::~cHistoryBuffer ()
{
  for (int i = 0; i < _size; i++)
  {
    delete buffer[i];
    buffer[i] = 0;
  }
  delete[] buffer;
}

void cHistoryBuffer::add (cTextChunk *chunk)
{
  if (_items < _size)
    _items++;
  else
  {
    delete buffer[curidx];
    buffer[curidx] = 0;
  }
  buffer[curidx] = chunk;
  curidx++;
  if (curidx >= _size) curidx = 0;
}

cTextChunk * cHistoryBuffer::operator[] (int idx)
{
  return (_items < _size) ? buffer[idx] : buffer[(curidx + idx) % _size];
}

void cHistoryBuffer::flush ()
{
  for (int i = 0; i < _size; i++)
  {
    delete buffer[i];
    buffer[i] = 0;
  }
  _items = 0;
  curidx = 0;
}


int cConsole::initHistory = 1000;

inline void swap (int &a, int &b)
{
  int tmp = a;
  a = b;
  b = tmp;
}

cConsole::cConsole (bool auxiliary, QWidget *parent, const char *name) :
      QTableView (parent, name)
{
//  setAttribute (Qt::WA_StaticContents);
//  setAttribute (Qt::WA_NoBackground);

  sess = -1;
  aux = auxiliary;
  aconvisible = false;
  
  bgcolor = Qt::black;
  usedrows = 0;
  currow = -1;
  currowpos = -1;
  wrappos = 0;
  
  currows = curcols = 0;
  charCount = charWidth = 0;
  
  if (!aux)
    historySize = initHistory;  //set history size; won't be modified if
    //user changes that setting in global prefs, that'll only affect new
    //connections
  else
    historySize = 100;        //aux. console does not need that big history
  QTableView::setNumCols (1);

  wordWrapping = true;
  indentValue = 0;
  
  //prepare the cache
  for (int i = 0; i < MAXCACHE; i++)
  {
    cacheEntry[i] = 0;
    cacheBlinkOn[i] = 0;
    cacheBlinkOff[i] = 0;
  }

  //blinking
  blinkPhase = true;
  blinking = false;
  blinkTimer = 0;
  
  buffer = new cHistoryBuffer (historySize);
  
  selected = false;
  canselect = false;

  repaintCount = 10;
  newlineCounter = 0;
  
  //background color
  QColor defbkcolor = Qt::black;
  QPalette pal = palette();
  pal.setColor (backgroundRole(), defbkcolor);
  pal.setColor (QPalette::Base, defbkcolor);
  setPalette (pal);
  viewport()->setPalette (pal);
  setBackgroundRole (QPalette::Base);
  viewport()->setBackgroundRole (QPalette::Base);

  //size policy
  QSizePolicy qsp (QSizePolicy::Expanding, QSizePolicy::Expanding);
  setSizePolicy (qsp);
  
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

  //create our auxiliary console if needed
  if (!aux)
  {
    aconsole = new cConsole (true, this);
    aconsole->hide ();
    aconvisible = false;
    aconsize = 25;  //aconsole covers 25% by default
    
    //connect some auxiliary console's signals to ours, so that links work correctly
    connect (aconsole, SIGNAL (sendCommand (const QString &)), this,
        SIGNAL (sendCommand (const QString &)));
    connect (aconsole, SIGNAL (promptCommand (const QString &)), this,
        SIGNAL (promptCommand (const QString &)));
  }
  else
    aconsole = 0;

  //initialize menu-link variables
  menuChunk = 0;
  linkMenu = 0;

  //set default system fixed font
  setFont (KGlobalSettings::fixedFont ()); //default system fixed font

  //set the I-beam cursor
  viewport()->setCursor (Qt::IBeamCursor);
  ishandcursor = false;
  
  //we want to receive mouse-move messages all the time
  setMouseTracking (true);
  viewport()->setMouseTracking (true);

  //fix output
  fixupOutput ();
}

cConsole::~cConsole()
{
  if (blinkTimer != 0)
  {
    blinkTimer->stop();
    delete blinkTimer;
  }
  emptyCache ();
  if (!aux)
    delete aconsole;
  delete buffer;
}

void cConsole::setSession (int s)
{
  sess = s;
  if (!aux)
    aconsole->setSession (s);
}

cConsole *cConsole::auxConsole ()
{
  if (aux)
    return 0;
  else
    return aconsole;
}

void cConsole::setFont (QFont f)
{
  myfont = f;
  QFontMetrics fm (myfont);
  descent = fm.descent ();
  charWidth = fm.width ("m");
  setCellHeight (fm.height () + 2);

  ///fix output window
  fixupOutput ();
  
  //and repaint everything
  repaintContents (false);
  if (!aux)
    aconsole->setFont (f);
}

void cConsole::setDefaultBkColor (QColor color)
{
  bgcolor = color;
  QPalette pal = palette();
  pal.setColor (backgroundRole(), bgcolor);
  pal.setColor (QPalette::Base, bgcolor);
  setPalette (pal);
  viewport()->setPalette (pal);
  emptyCache ();
  repaintContents (false);
  if (!aux)
    aconsole->setDefaultBkColor (color);
}

void cConsole::setWordWrapping (bool val)
{
  wordWrapping = val;
  if (!aux)
    aconsole->setWordWrapping (val);
}

void cConsole::setIndentation (int val)
{
  //indentation must not exceed 10 characters
  if ((val >= 0) && (val <= 10))
    indentValue = val;
  if (!aux)
    aconsole->setIndentation (val);
}

void cConsole::setWrapPos (int wp)
{
  wrappos = wp;
  if (!aux)
    aconsole->setWrapPos (wp);
}

void cConsole::setEnableBlinking (bool value)
{
  //look if this changes blinking status
  if (blinking == value)
    return;
  
  //if so, update blinking timer status
  blinking = value;
  if (blinking)
  {
    if (blinkTimer == 0)
    {
      blinkTimer = new QTimer (this);
      connect (blinkTimer, SIGNAL (timeout()), this, SLOT (blinkTimerTimeout ()));
    }
    //blinking will occur once per second
    blinkTimer->start (BLINK_INTERVAL);
    //we start with characters displayed
    blinkPhase = true;
  }
  else
  {
    if (blinkTimer != 0)
    {
      blinkPhase = true;
      blinkTimer->stop ();
      //if status goes from enabled to disabled, we may need to repaint - do it!
      updateContents ();
    }
  }  

  if (!aux)
    aconsole->setEnableBlinking (value);
}

QStringList cConsole::words (QString prefix, int minLength)
{
  QStringList lst;
  int row = usedrows - 100, nHits=0;
  if (row < 0) row = 0;

  for (; row < usedrows; ++row)  
  {
    cTextChunk *chunk = (*buffer)[row];
    lst += chunk->words (minLength);
  }
  // no sorting - we want to offer words in order of appearance
  // lst.sort ();

  //get rid of words that don't meet our requirements
  QStringList::iterator it = lst.begin();
  while (it != lst.end()) {
    // by Magnus Lundborg and me:
    if ((*it).startsWith (prefix, Qt::CaseInsensitive)) {
      int prevIndex = lst.indexOf(*it);
      /* If there is a previous occurence, but it is the last of the "hits" do
	 not remove that one just remove this one instead. */
      if (prevIndex == nHits-1)
	{
	  it=lst.erase (it);
	  continue;
	}
      if (prevIndex != -1 && prevIndex != nHits-1)
	{
	  QStringList::iterator prev = lst.begin() + prevIndex;
	  if (prev != it)
	    {
	      // there is a previous word - delete it
	      lst.erase (prev);
	      --nHits;
	    }
	}
      // move on
      ++it;
      ++nHits;
    }
    else
      // the word does not match - delete it
      it = lst.erase (it);
  }
  return lst;
}

// TODO: looks like this is currently unused ...
void cConsole::clear ()
{
  emptyCache ();
  
  buffer->flush ();
  
  int oldHistorySize = historySize;
  
  //update history size...
  if (!aux)
    historySize = initHistory;
  else
    historySize = 100;        //aux. console does not need that big history

  aconsize = 25;  //aconsole covers 25% by default

  usedrows = 0;
  currow = -1;
  currowpos = -1;
  
  setNumRows (0);

  if (oldHistorySize != historySize)
  {
    //reallocate memory
    delete buffer;
    buffer = new cHistoryBuffer(historySize);
  }
   
  repaintContents ();
  if (!aux)
    aconsole->clear ();

  //fix output
  fixupOutput ();

  //this processEvents() MUST be here! - otherwise aconsole gets shown on
  //second+ connection... (problem with scrollbar's maxvalue)
  (KApplication::kApplication ())->processEvents ();
  if (!aux)
    sliderChanged (0);  //this one is needed as well
}

void cConsole::addLine (cTextChunk *chunk)
{
  addNewText (chunk, true);
}

void cConsole::addText (cTextChunk *chunk)
{
  addNewText (chunk, false);
}

void cConsole::addNewText (cTextChunk *chunk, bool endTheLine)
{
  if (!chunk)
  {
    if (endTheLine)
      endLine ();
    return;
  }

  //wrap position
  int wp = wrappos;
  if ((wrappos < 20) || (wrappos > charCount))
    wp = charCount;

  //disable repainting
  setUpdatesEnabled (false);

  pendingLineMove = 0;

  //create a duplicate, as we cannot delete the original chunk
  cTextChunk *storedChunk = chunk;
  chunk = chunk->duplicate ();

  //first of all, if we're writing the very first string
  if (currow == -1)
    endLine ();    //prepare first line

  //we'll go line after line (visible line, not received line), trying to
  //find wrap-position, then we copy that part of the string to another
  //string and display it
  cTextChunk *nextchunk = 0;
  do {
    //delete current line from the redraw cache
    for (int j = MAXCACHE - 1; j >= 0; j--)
      if ((cacheBlinkOn[j] != 0) && (cacheEntry[j] == currow))
        deleteCacheEntry (j);
    
    nextchunk = chunk->splitLine (wp - currowpos, wordWrapping, indentValue, true);
    (*buffer)[currow]->append (chunk);
    currowpos = (*buffer)[currow]->startAttr().startpos + (*buffer)[currow]->length();
    
    if (nextchunk == 0) //last part of chunk
    {
      if (endTheLine)   //we should end the line -> do it!
        endLine ();
      else
        if (currowpos >= wp - 1)
          //otherwise end the line if we've reached its end
          endLine ();
    }
    else
      endLine ();   //goin' to new line
    chunk = nextchunk;
  } while (nextchunk);

  //increase number of rows if needed; it's not in endLine for speed's sake
  if (numRows() < currow + 1)
    setNumRows (currow + 1);

  //ensure cell visibility if needed; it's not in endLine for speed's sake
  if (aux || (!aconvisible))
    ensureCellVisible (currow, 0);
  else
    if ((currow == historySize - 1) && pendingLineMove)
      //contents have moved pendingLineMove rows up
      setContentsPos (0, contentsY()-(pendingLineMove*cellHeight()));

  //enable repainting here
  setUpdatesEnabled (true); 

  //request repaint here
  //If we've added less than 10 lines since last repaint , we request an update, which will be
  //performed when the app returns to main event queue.
  //If, however, >=10 lines have been added since last repaint, we ask for an instant repaint.
  //The purpose of this policy is to prevent the "black-out" effect, that can happen when lot
  //of lines is received at once - the text gets blacked out while we process the text, then
  //it gets filled with latest text after text adding is done.
  //UPDATE: not 10 lines, but a configurable amount
  if (repaintCount)
  {
    if (newlineCounter < repaintCount)
      updateContents ();
    else
      repaintContents ();
  }
  else
  {
    newlineCounter = 0;
    updateContents ();
  }

  if (!aux)
    aconsole->addNewText (storedChunk, endTheLine);
}

void cConsole::handleBufferShift()
{
  //move selection if needed
  if (selected)
  {
    selrow1--;
    selrow2--;

    if (selrow1 < 0) selrow1 = 0;
    if (selrow2 < 0) selrow2 = 0;
  }

  //no cache for aconsole
  if (!aux)
  {
    for (int i = 0; i < MAXCACHE; i++)
    {
      if (cacheBlinkOn[i] != 0)
      {
        cacheEntry[i]--;
        if (cacheEntry[i] < 0)
          deleteCacheEntry (i);
      }
    }
  }

  //shift buffer - done automatically by cHistoryBuffer :-)
}

void cConsole::endLine ()
{
  currowpos = 0;
  pendingLineMove++;
  newlineCounter++;
  if (currow < historySize - 1)
    currow++;
  else
    handleBufferShift ();

  //add a new empty line
  cTextChunk *chunk = new cTextChunk (this);
  buffer->add (chunk);

  usedrows = currow + 1;
}

void cConsole::forceBeginOfLine ()
{
  if (currowpos > 0)
    endLine ();
  if (!aux)
    aconsole->forceBeginOfLine ();
}

void cConsole::dumpBuffer (bool fromcurrent, FILE *file, char dumpType)
{
  //file must be opened for writing!
  //headers must also be written in case of HTML dump...
  
  //get visible range
  int y = contentsY();
  int row = rowAt (y);
  int start = fromcurrent ? row : 0;

  cANSIParser *ap = dynamic_cast<cANSIParser *>(cActionManager::self()->object ("ansiparser", sess));
  
  for (int i = start; i <= currow; i++)
  {
    //proceed line by line
    cTextChunk *chunk = (*buffer)[i];
    if (chunk)
      switch (dumpType) {
        case TRANSCRIPT_PLAIN: fputs (chunk->toText().toLocal8Bit(), file); break;
        case TRANSCRIPT_ANSI: fputs (chunk->toAnsi(ap).toLocal8Bit(), file); break;
        case TRANSCRIPT_HTML: fputs (chunk->toHTML().toLocal8Bit(), file); break;
      };
  }
}

void cConsole::tryUpdateHistorySize ()
{
  if (currow == -1)
    clear ();
}

void cConsole::paintCell (QPainter *p, int row, int)
{
  //reset newline counter
  newlineCounter = 0;
  
  if (row < usedrows)     //only the first "usedrows" rows contain data
  {
    int cw = QTableView::cellWidth ();
    int ch = cellHeight ();

    QPixmap *pix = 0, *pix2 = 0;

    //if this line is selected (partially selected), do NOT use cache!
    bool forceNoCache = false;
    int ss1, ss2, sp1, sp2;
    ss1 = selrow1; ss2 = selrow2;
    sp1 = selrowpos1; sp2 = selrowpos2;
    if (ss1 > ss2)
    {
      swap (ss1, ss2);
      swap (sp1, sp2);
    }
    if ((ss1 == ss2) && (sp1 > sp2))
      swap (sp1, sp2);
    if (selected && (ss1 <= row) && (ss2 >= row))
      forceNoCache = true;
    if (aux)  //no cache for auxiliary console
      forceNoCache = true;
      
    //try to find the pixmap in the cache first...
    bool foundInCache = false;
    if (!forceNoCache)
      for (int i = 0; i < MAXCACHE; i++)
        if ((cacheBlinkOn[i] != NULL) && (cacheEntry[i] == row))
        {
          foundInCache = true;
          if (blinking)
          {
            pix = cacheBlinkOn[i];
            pix2 = cacheBlinkOff[i];
          }
          else
            pix = cacheBlinkOn[i];
          break;
        }
            
    //look if there are any blinking chars
    bool hasblink = hasBlink (row);
    //if search fails, we have to draw that line :(
    //if we'll be putting it into cache and there are blinking chars in the
    //line, then we'll construct both blinkOn and blinkOff line
    if (!foundInCache)
    {
      //We create both entries even if blinking is disabled, because user
      //can enable it at any time. This can have impact on performance -
      //maybe I'll change this behavior?
      
      pix = new QPixmap (cw, ch);
      pix->fill (bgcolor);
      QPainter *p2 = new QPainter (pix);

      QPainter *p2b = 0;
      if (hasblink)
      {
        //pix2 and p2b represent pixmap with hidden blinking letters
        pix2 = new QPixmap (cw, ch);
        pix2->fill (bgcolor);
        p2b = new QPainter (pix2);
      }
      
      //prepare arguments and paint the row!
      cTextChunk *chunk = (*buffer)[row];
      int selstart = -1;
      int sellen = 0;
      if (selected && (ss1 <= row) && (ss2 >= row))
      {
        selstart = (ss1 == row) ? sp1 : 0;
        sellen = (ss2 == row) ? (sp2 - selstart + 1) : (charCount - selstart);
      }
      chunk->paint (charCount, selstart, sellen, charWidth, cellHeight() - descent - 1, p2, p2b);
      
      p2->end ();
      delete p2;
      if (hasblink)
      {
        p2b->end();
        delete p2b;
      }

      //Good. Now add this pixmap to the cache (if allowed).
      if (!forceNoCache)
      {
        int idx = -1;
        for (int i = 0; i < MAXCACHE; i++)
          if (cacheBlinkOn[i] == 0)
          {
            idx = i;
            break;
          }
        if (idx == -1)   //cache is FULL - create space!
        {
          deleteCacheEntry (0);
          for (int i = 0; i < MAXCACHE - 1; i++)
          {
            cacheBlinkOn[i] = cacheBlinkOn[i+1];
            cacheBlinkOff[i] = cacheBlinkOff[i+1];
            cacheEntry[i] = cacheEntry[i+1];
          }
          //we'll add a new entry at the free position
          idx = MAXCACHE - 1;
        }  
        cacheEntry[idx] = row;
        cacheBlinkOn[idx] = pix;
        if (hasblink)
          cacheBlinkOff[idx] = pix2;
        else
          cacheBlinkOff[idx] = pix;
      }
    }
    
    if (blinking && hasblink)
      p->drawPixmap (QPoint (0, 0), blinkPhase ? (*pix) : (*pix2),
            QRect (0, 0, cw, ch));
    else
      p->drawPixmap (QPoint (0, 0), *pix, QRect (0, 0, cw, ch));

    //if pix is not in the cache, then we have to delete it!
    //the same for pix2
    if (forceNoCache)
    {
      delete pix;
      if (hasblink)
        delete pix2;
    }
  }
  //else we do nothing...
  else
    return;
}

void cConsole::fixupOutput ()
{
  //cache is no longer valid
  emptyCache ();

  //compute new number of columns
  QSize wsize = viewportSize (width(), height());
  int newcols = wsize.width() / charWidth;
  int spaceforrows = wsize.height() / cellHeight ();
  QTableView::setCellWidth (newcols * charWidth);

  //columns are handled first... no manipulation needed anymore, as the paint routines are
  //more intelligent now :D
  if (newcols != charCount)
    charCount = newcols;

  //rows don't need much manipulation...
  setNumRows (usedrows);

  if (!aux)
  {
    int value = verticalScrollBar()->value ();
    int maxval = verticalScrollBar()->maximum ();
    if ((value != maxval) && (!aconvisible))  //resize led to contents shift - move to bottom
      verticalScrollBar()->setValue (verticalScrollBar()->maximum());
  }  

  //set auxiliary console geometry, if needed
  if (!aux)
  {
    //geometry is relative to parent widget - that's the console object
    aconsole->setGeometry (0, height() - height() * aconsize / 100,
    width() - verticalScrollBar()->width(), height() * aconsize / 100);
          //height()*(100-aconsize)/100 can NOT be used in param 2,
          //as it may cause one-pixel shift, because integer division
          //is not accurate (A*3/4 + A/4 is not always A)
    aconsole->fixupOutput();
  }

  //inform others about the changes; telnet with NAWS should be connected here...
  if (!aux)
  {
    currows = spaceforrows;
    curcols = newcols;
    emit dimensionsChanged (newcols, spaceforrows);
  }

  //update the widget
  viewport()->update ();
  updateContents ();
}

void cConsole::resizeEvent (QResizeEvent *e)
{
  //let the parent process it first
  QTableView::resizeEvent (e);

  //fixup output window to reflect changes
  fixupOutput ();
}

void cConsole::updateRowRange (int r1, int r2)
{
  int rr1 = r1;
  int rr2 = r2;
  if (rr1 > rr2)
    swap (rr1, rr2);
  for (int i = rr1; i <= rr2; i++)
    updateCell (i, 0);
}

void cConsole::mousePressEvent (QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    //clear old selection first
    if (selected)
    {
      selected = false;
      updateRowRange (selrow1, selrow2);
    }

    if (!triple_click_time.isNull()
        && triple_click_time.elapsed() < QApplication::doubleClickInterval())
    {
      mouseTripleClickEvent (e);
      //nothing more to do - we mustn't proceed, as further code would destroy our selection
      return;
    }
    if (numRows() == 0)
      return;

    int inrow = rowAt (e->y());
    int incol = e->x() / charWidth;
    if (incol >= charCount)
      incol = charCount;
    if (incol < 0)
      incol = 0;
    if (inrow < 0)
      inrow = 0;
    if (inrow >= numRows())
      inrow = numRows() - 1;
    //do not crash if widget is still empty
    if ((inrow < 0) || (incol < 0))
      return;
    previousr = selrow2 = selrow1 = inrow;
    previousc = selrowpos2 = selrowpos1 = incol;
    canselect = true;
    selected = false;
  }
  else
    if (e->button() == Qt::MidButton)
    {
      if (aux)
        //auxiliary console won't process this, it will leave it for main
        //console (it's a parent of aconsole)
        e->ignore();
      else
      {
        //middle button scrolls to bottom, leaving split-screen mode
        verticalScrollBar()->setValue (verticalScrollBar()->maximum());
        e->accept();
      }
    }
}

void cConsole::mouseReleaseEvent (QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    canselect = false;
    
    //click on a link?
    int inrow = rowAt (e->y());
    int incol = e->x() / charWidth;
    //do not crash if widget is still empty
    if ((inrow < 0) || (incol < 0) || (numRows() == 0))
      return;
    cTextChunk *chunk = (*buffer)[inrow];
    if (chunk)
    {
      chunkItem *item = chunk->itemAt (incol);
      if (item && (item->type() == CHUNK_LINK))
        activateLink ((chunkLink *) item, e->globalPos());
    }
    
  }
  /*
  else if (e->button() == Qt::MidButton) {
    //TODO: maybe Ctrl+MidButton to scroll down, MidButton to paste ?
    QString command = (KApplication::kApplication())->clipboard()->text(QClipboard::Selection);
    addCommand(command);
  }
  */
}

void cConsole::mouseMoveEvent (QMouseEvent *e)
{
  //changing cursor over links...
  int inrow = rowAt (e->y());
  int incol = e->x() / charWidth;
  //do not crash if widget is still empty
  if ((inrow < 0) || (incol < 0) || (numRows() == 0))
    return;
  cTextChunk *chunk = (*buffer)[inrow];
  if (chunk)
  {
    chunkItem *item = chunk->itemAt (incol);
    if (item && (item->type() == CHUNK_LINK))
      //we're over some link -> we show a hand
      viewport()->setCursor (Qt::PointingHandCursor);
    else
      viewport()->setCursor (Qt::IBeamCursor);
  }
  if (canselect)
  {
    if (incol >= charCount)
      incol = charCount;
    if (incol < 0)
      incol = 0;
    if (inrow < 0)
      inrow = 0;
    if (inrow >= numRows())
      inrow = numRows() - 1;
    previousr = selrow2;
    previousc = selrowpos2;
    selrow2 = inrow;
    selrowpos2 = incol;
    if ((inrow != previousr) || (incol != previousc))
      //moved mouse to another cell while pressing button
    {
      selected = true;

      //update all selected cells:
      updateRowRange (previousr, selrow2);

      //put selected text to mouse selection...
      addSelectionToClipboard (QClipboard::Selection);
    }
  }
}

void cConsole::mouseDoubleClickEvent (QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    triple_click_time.start();
    //clear old selection first - there should be none, as mousePress handler
    //has already been called, but I want to be sure...
    
    //FIXME: Why on earth are we checking/clearing selected again - waste!?
    //Same issue with triple click - to get to these, you have to go
    //through mousePress - in what instance would it ever be possible
    //that selected is _not_ cleared?
    // (Tomas) Well, you just never know... ;)
    if (selected)
    {
      selected = false;
      updateRowRange (selrow1, selrow2);
      (KApplication::kApplication())->processEvents ();
    }

    int inrow = rowAt (e->y());
    int incol = e->x() / charWidth;
    if (incol >= charCount)
      incol = charCount;
    if (incol < 0)
      incol = 0;
    if (inrow < 0)
      inrow = 0;
    if (inrow >= numRows())
      inrow = numRows() - 1;
    //do not crash if widget is still empty
    if ((inrow < 0) || (incol < 0))
      return;  
    previousr = selrow2 = selrow1 = inrow;
    canselect = false;
    
    //fetch the string on that row
    QString s = (*buffer)[inrow]->plainText ();
    if (incol >= s.length())
      return;
    int len = s.length();
    
    //find the whole word, if we've clicked in one... More than one row
    //can never be selected.
    QChar ch = s[incol];
    if (ch.isLetterOrNumber() || (ch == '_'))
    {
      int index;
      //find the beginning
      index = incol;
      while (ch.isLetterOrNumber() || (ch == '_'))
      {
        if (index == 0)
          break;
        ch = s[--index];
      }
      if (!(ch.isLetterOrNumber() || (ch == '_')))
        index++;
      selrowpos1 = (index >= 0) ? index : 0;  //set beginning of selection
      //and the end of the text
      index = incol;
      ch = s[incol];
      while (ch.isLetterOrNumber() || (ch == '_'))
      {
        if (index == len - 1)
          break;
        ch = s[++index];
      }
      if (!(ch.isLetterOrNumber() || (ch == '_')))
        index--;
      selrowpos2 = (index < len) ? index : len;
      //set end of selection

      selected = true;

      //update all selected cells:
      updateRowRange (selrow1, selrow2);

      //put selected text to mouse selection...
      addSelectionToClipboard (QClipboard::Selection);
    }
    else
    {
      previousc = selrowpos2 = selrowpos1 = incol;
      selected = false;
    }
  }
}
//Select entire text line on triple click
void cConsole::mouseTripleClickEvent (QMouseEvent *e)
{
  //FIXME: Why on earth are we checking selected again - waste!?
  if (selected) {
    selected = false;
    updateRowRange(selrow1, selrow2);
    (KApplication::kApplication())->processEvents();
  }

  int inrow = rowAt(e->y());

  // ensure that the row number is correct - usually only needed in the
  // beginning, when there are very little or no rows
  if (numRows() == 0)
    return;
  if (inrow < 0)
    inrow = 0;
  if (inrow >= numRows())
    inrow = numRows() - 1;

  
  QString textLine = (* buffer) [inrow]->plainText();

  if (!textLine.isNull()) {
    //select the whole line!
    
    previousr = selrow2 = selrow1 = inrow;
    canselect = false;
    selected = true;
    selrowpos1 = 0;
    selrowpos2 = textLine.length() - 1;
  
    //update all selected cells:
    updateRowRange (selrow1, selrow2);
    
    //put selected text to mouse selection...
    addSelectionToClipboard (QClipboard::Selection);
  }
}

void cConsole::wheelEvent (QWheelEvent *e)
{
  /*
  Our parent processes whell event correctly, but there's one problem.
  If this is an auxiliary console, it shouldn't be scrollable, but the
  default wheelEvent implementation doesn't know it and scrolls.

  My solution is to process whelel event for the standard console by
  default handler and to ignore it by aconsole. Then it gets to the parent
  widget (main console), which processes it. It means that even if you
  use the wheel in aconsole region, main console gets scrolled, which is
  exactly what we want.
  */
  if (!aux)
    QTableView::wheelEvent (e);
  else
    e->ignore ();
}

bool cConsole::event (QEvent *e)
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    QPoint point = helpEvent->pos();
  
    //we'll show some tooltip...
    int inrow = rowAt (contentsY() + point.y());
    if ((inrow >= 0) && (inrow < numRows())) {
      //get the timestamp
      QString tipText = (*buffer)[inrow]->getTimeStamp();
      //show the tip!
      QToolTip::showText (helpEvent->globalPos(), tipText, this);
    } else
      QToolTip::hideText();
  }
  return QWidget::event(e);
}

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

void cConsole::addSelectionToClipboard (QClipboard::Mode clipboardMode)
{
  if (selected)
  {
    QString text = "";
    int r1 = selrow1;
    int r2 = selrow2;
    int c1 = selrowpos1;
    int c2 = selrowpos2;
    int start = r1 * charCount + c1;
    int end = r2 * charCount + c2;
    if (start > end)
    {
      swap (r1, r2);
      swap (c1, c2);
    }
    for (int i = r1; i <= r2; i++)
    {
      QString line = (*buffer)[i]->plainText();
      int len = line.length();
      if (i == r1) line.remove (0, c1);
      if (i == r2) line.truncate (line.length() - (len - c2 - 1));
      //add a \n, unless it's a last line
      if (i < r2)
        line += '\n';
      //and add this line to the resulting text
      text += line;
    }
    //now I have the whole text - put it to the clipboard!
    if (text.length() > 0 && !text.isNull())
    {
      QClipboard *clip = (KApplication::kApplication())->clipboard ();

      clip->setText(text,clipboardMode);
      //If text is being copied into Global Clipboard (ie non-selection)
      //this clears the selected text from the screen
      if (clipboardMode == QClipboard::Clipboard) {
        selected = false;
        updateRowRange (selrow1, selrow2);
      }
    }
  }
}

void cConsole::expireNamedLinks (const QString &name)
{
  //this may be a bit slow if used too often...

  //get visible range
  int y1 = contentsY();
  int y2 = y1 + contentsHeight() - 1;
  int row1 = rowAt (y1);
  int row2 = rowAt (y2);
  
  for (int i = 0; i <= currow; i++)
  {
    //proceed line by line
    cTextChunk *chunk = (*buffer)[i];
    bool expired = chunk->expireNamedLinks (name);
    if (expired)
    {
      //this line must be deleted from the drawing cache, if it's there
      deleteLineFromCache (i);
      //repaint row if it's visible
      if ((row1 <= i) && (row2 >= i))
        updateCell (i, 0);
    }
  }
}


//the following functions are not implemented using {add/subtract}{Line/Page},
//because pageStep cannot be redefined in QGridView (no idea why). So I
//have to do this manually...

void cConsole::lineUp ()
{
  QScrollBar *s = verticalScrollBar ();
  s->setValue (s->value() - cellHeight ());
}

void cConsole::lineDown ()
{
  QScrollBar *s = verticalScrollBar ();
  s->setValue (s->value() + cellHeight ());
}

void cConsole::pageUp ()
{
  QScrollBar *s = verticalScrollBar ();
  s->setValue (s->value() - (height() - height() / 4) * 9/10);
}

void cConsole::pageDown ()
{
  QScrollBar *s = verticalScrollBar ();
  s->setValue (s->value() + (height() - height() / 4) * 9/10);
}

void cConsole::aconUp ()
{
  //85% is max size
  if (aconsize <= 80)
    aconsize += 5;
  else
    return;
  
  //set auxiliary console geometry, if needed
  if (!aux)
  {
    //geometry is relative to parent widget - that's the console object
    aconsole->setGeometry (0, height() - height() * aconsize / 100,
    width() - verticalScrollBar()->width(), height() * aconsize / 100);
          //height()*(100-aconsize)/100 can NOT be used in param 2,
          //as it may cause one-pixel shift, because integer division
          //is not accurate (A*3/4 + A/4 is not always A)
    //no need to move contents here, because it'll be done automatically
    // (we're always at the very bottom)
  }
}

void cConsole::aconDown ()
{
  int oldaconsize = aconsize;
  //5% is min size
  if (aconsize >= 10)
    aconsize -= 5;
  else
    return;
  
  //set auxiliary console geometry, if needed
  if (!aux)
  {
    int newy = height() - height() * aconsize / 100;
    int oldy = height() - height() * oldaconsize / 100;

    //geometry is relative to parent widget - that's the console object
    aconsole->setGeometry (0, newy,
    width() - verticalScrollBar()->width(), height() * aconsize / 100);
          //height()*(100-aconsize)/100 can NOT be used in param 2,
          //as it may cause one-pixel shift, because integer division
          //is not accurate (A*3/4 + A/4 is not always A)

    //we also need to move contents

    aconsole->scrollBy (0, newy - oldy);
  }
}

void cConsole::sliderChanged (int value)
{
  int maxval = verticalScrollBar()->maximum ();
  if ((value == maxval) && (aconvisible))
  {
    aconsole->hide ();
    aconvisible = false;
    repaintContents (false);
  }
  if ((value != maxval) && (!aconvisible))
  {
    aconsole->show ();
    aconvisible = true;
  }
}

void cConsole::deleteLineFromCache (int line)
{
  if ((line < 0) || (line > currow))
    return;
  for (int i = 0; i < MAXCACHE; i++)
    if (cacheEntry[i] == line)
      deleteCacheEntry (i);
}

void cConsole::deleteCacheEntry (int which)
{
  if ((which < 0) || (which >= MAXCACHE))
    return;
  if (aux)   //no cache for aconsole
    return;
  if (cacheBlinkOn[which] != 0)
  {
    cacheEntry[which] = 0;
    //no blinking here
    if ((cacheBlinkOn[which] == cacheBlinkOff[which]) || (cacheBlinkOff[which] == 0))
    {
      delete cacheBlinkOn[which];
      cacheBlinkOff[which] = 0;
      cacheBlinkOn[which] = 0;
    }
    else
    {
      delete cacheBlinkOff[which];
      delete cacheBlinkOn[which];
      cacheBlinkOff[which] = 0;
      cacheBlinkOn[which] = 0;
    }
  }
}

void cConsole::emptyCache ()
{
  if (aux) //no cache for aconsole
    return;
  for (int i = 0; i < MAXCACHE; i++)
    deleteCacheEntry (i);
}

bool cConsole::hasBlink (int row)
{
  if ((row < 0) || (row >= usedrows))
    return false;
  cTextChunk *chunk = (*buffer)[row];
  if (chunk == 0)  //that row does not exist yet
    return false;
  bool hb = false;
  list<chunkItem *> entries = chunk->entries();
  list<chunkItem *>::iterator it;
  for (it = entries.begin(); it != entries.end(); ++it)
    if ((*it)->type() == CHUNK_ATTRIB)
      if (((chunkAttrib *) (*it))->attrib() & ATTRIB_BLINK)
      {
        hb = true;
        break;
      }
  return hb;
}

void cConsole::forceEmitSize ()
{
  emit dimensionsChanged (curcols, currows);
}

void cConsole::blinkTimerTimeout ()
{
  //switch state
  blinkPhase = blinkPhase ? false : true;
  
  //get visible range
  int y1 = contentsY();
  int y2 = y1 + contentsHeight() - 1;
  int row1 = rowAt (y1);
  int row2 = rowAt (y2);

  //for each row
  for (int i = row1; i < row2; i++)
  {
    //look if it contains blinking characters
    if (hasBlink (i))
    //if it does, redraw it
      updateCell (i, 0);
  }
}

#include "cconsole.moc"

