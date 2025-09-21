//
// C++ Implementation: ctextchunk
//
// Description: 
//
/*
Copyright 2004-2011 Tomas Mecir <kmuddy@kmuddy.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ctextchunk.h"

#include "cansiparser.h"
#include "cconsole.h"

#include <QPainter>
#include <QRegularExpression>

#include <stdlib.h>

/** state variables needed to paint a row */
/*
struct paintStatus {
  int length, selstart, sellen;
  
  int charWidth, charHeight;
  int cellHeight;
  
  bool blinking, negative, invisible;
    
  //paint() won't display text if this is on - used to display blinking text
  bool dontShowText;
  QFont defaultFont, paintFont;
  QColor paintColor, fillColor;
  QColor defbkcolor;
};
*/

cTextChunk::cTextChunk (cConsole *_console)
{
  init (_console);
}

cTextChunk::cTextChunk (cConsole *_console, const QString &text)
{
  init (_console);
  chunkText *cht = new chunkText;
  cht->setText (text);
  appendEntry (cht);
}

void cTextChunk::init (cConsole *_console)
{
  startattr.startpos = 0;
  startattr.attrib = 0;
//  pstatus = new paintStatus;
  console = _console;
  //update the timestamp
  timestamp = QDateTime::currentDateTime();
}

cTextChunk::~cTextChunk ()
{
  //delete all chunks...
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    delete *it;
  _entries.clear();
//  delete pstatus;
}

void cTextChunk::appendEntry (chunkItem *entry)
{
  if (!entry) return;
  //compute starting position
  int basepos = startattr.startpos;
  if (!_entries.empty())
    basepos = _entries.back()->startPos() + _entries.back()->length();
  //add the new entry
  _entries.push_back (entry);
  //update starting position
  entry->setStartPos (basepos);
  entry->_chunk = this;
  //update the timestamp
  timestamp = QDateTime::currentDateTime();
}

void cTextChunk::append (cTextChunk *chunk2)
{
  if (_entries.empty())
    // this chunk empty - copy starting attributes from the other chunk
    setStartAttr (chunk2->startAttr());
  else {
    // this chunk not empty - copy start attributes as regular entries ..,
    QColor fg = chunk2->startAttr().fg;
    QColor bg = chunk2->startAttr().bg;
    int attrib = chunk2->startAttr().attrib;

    chunkFg *ch1 = new chunkFg;
    ch1->setFg (fg);
    chunkBg *ch2 = new chunkBg;
    ch2->setBg (bg);
    chunkAttrib *ch3 = new chunkAttrib;
    ch3->setAttrib (attrib);

    _entries.push_back (ch1);
    _entries.push_back (ch2);
    _entries.push_back (ch3);
  }
  //add entries...
  list<chunkItem *>::iterator it;
  for (it = chunk2->_entries.begin(); it != chunk2->_entries.end(); ++it)
    //add entry
    _entries.push_back (*it);
  
  fixupStartPositions ();

  //update the timestamp
  timestamp = QDateTime::currentDateTime();
  
  //finally, delete chunk2 - but clear its list first to prevent destructor from deleting items
  chunk2->_entries.clear();
  delete chunk2;
}

chunkItem *cTextChunk::itemAt (int pos)
{
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    int spos = (*it)->startPos ();
    int len = (*it)->length ();
    if ((len > 0) && (pos >= spos) && (pos < spos + len))
      break;
  }
  if (it != _entries.end())
    return *it;
  return nullptr;
}

cTextChunk *cTextChunk::splitLine (int idx, bool wordwrap, int indent,
    bool trimSpaces)
{
  //look if we need to split the line
  if (length() <= idx)
    return nullptr;

  //OKay, we need to split...
  QString text = plainText ();
  int startpos = startattr.startpos;
  
  //find position, where to split...
  int splitpos = idx - 1;
  if (wordwrap)
  {
    if (text[splitpos + 1] != ' ')  //okay if next char is not a space
    {
      while (splitpos >= startpos)
        if (text[splitpos] == ' ')  //wrap here
          break;
        else
          splitpos--;
      if (splitpos < startpos)   //no suitable wrap-position found
        splitpos = idx - 1;
    }
  }
  //splitpos now contains the last letter index that will be on line 1

  //now go, find the item that this splitpos belongs to, and split there!
  bool moving = false;
  cTextChunk *ch2 = nullptr;
  chunkStart newstartattr = startattr;
  newstartattr.startpos = indent;   //start at indentation value
  list<chunkItem *>::iterator it, moveit;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    char type = (*it)->type();
    
    //update newstartattr
    switch (type)
    {
      case CHUNK_FG: newstartattr.fg = ((chunkFg*) (*it))->fg();
        break;
      case CHUNK_BG: newstartattr.bg = ((chunkBg *) (*it))->bg();
        break;
      case CHUNK_ATTRIB: newstartattr.attrib = ((chunkAttrib *) (*it))->attrib();
        break;
    }
    
    if ((*it)->length() == 0)
      continue;    //this chunk has no length - OK
    int last = (*it)->startPos() + (*it)->length() - 1;
    
    if (last < splitpos)  //don't split here yet...
      continue;
    
    moving = true;
    //OKay, this chunk is the one to split!
    if (last == splitpos)
    {
      ++it;
      moveit = it;      
    }
    else
    {
      //split the text in two
      chunkItem *chunk;
      chunk = (*it)->split (splitpos - (*it)->startPos());
      if (chunk)
      {
        ++it;
        it = _entries.insert (it, chunk);
        moveit = it;
      }
    }
    break;
  }
  
  bool mustTrim = trimSpaces;
  if (moving) //now move remaining entries...
  {
    ch2 = new cTextChunk (console);
    ch2->setStartAttr (newstartattr);
    while (moveit != _entries.end())
    {
      if (mustTrim && (*moveit)->length() > 0)
      {
        (*moveit)->trimLeft ();
        if ((*moveit)->length())
        {
          mustTrim = false;
          //add to second list
          ch2->appendEntry (*moveit);
        }
        else
          delete *moveit;  //chunk not needed anymore
      }
      else
        //just add to second list
        ch2->appendEntry (*moveit);
      //remove from first list
      moveit = _entries.erase (moveit);
    }
  }
  
  //update the timestamp
  timestamp = QDateTime::currentDateTime();

  //return the result
  return ch2;
}


struct letterColor {
  QColor fg, bg;
};

void cTextChunk::applyColorChanges (list<colorChange> &changes)
{
  // *** this is more complex than the solution in 0.5-0.6.1, and may also be slower ***
  // *** on the other hand, it allows much better functionality, so it's worth it... ***

  if (changes.empty())
    return;
  
  //generate an array of current colors
  QString plain = plainText ();
  int len = plain.length();
  if (len == 0) return;
  letterColor *colors = new letterColor[len];
  letterColor *newcolors = new letterColor[len];
  list<chunkItem *>::iterator ii;
  QColor curfg = startattr.fg;
  QColor curbg = startattr.bg;
  for (ii = _entries.begin(); ii != _entries.end(); ++ii)
  {
    if ((*ii)->type() == CHUNK_FG)
      curfg = ((chunkFg *) *ii)->fg();
    else if ((*ii)->type() == CHUNK_BG)
      curbg = ((chunkBg *) *ii)->bg();
    
    if ((*ii)->length() == 0)
      continue;
    for (int i = (*ii)->startPos(); i < (*ii)->startPos() + (*ii)->length(); i++)
    {
      colors[i].fg = curfg;
      colors[i].bg = curbg;
    }
  }
  //generate an array of new colors, and replace color settings with new ones if needed
  for (int i = 0; i < len; i++)
  {
    newcolors[i].fg = colors[i].fg;
    newcolors[i].bg = colors[i].bg;
  }
  
  list<colorChange>::iterator it;
  for (it = changes.begin(); it != changes.end(); ++it)
  {
    for (int i = (*it).start; i < (*it).start + (*it).len; i++)
      if (i < len)
      {
        if ((*it).keepfg)
          newcolors[i].fg = colors[i].fg;
        else
          newcolors[i].fg = (*it).fg;
        if ((*it).keepbg)
          newcolors[i].bg = colors[i].bg;
        else
          newcolors[i].bg = (*it).bg;
      }
  }
  // OK. Now we have a list of colors of all letters to play with :D
  
  //delete all color changes from the old string
  ii = _entries.begin();
  while (ii != _entries.end())
  {
    //erase FG/BG, keep the rest
    if (((*ii)->type() == CHUNK_FG) || ((*ii)->type() == CHUNK_BG))
    {
      delete *ii;
      ii = _entries.erase (ii);
    }
    else
      ++ii;
  }
  //update initial color settings
  startattr.fg = newcolors[0].fg;
  startattr.bg = newcolors[0].bg;
  
  //generate list of color-change item chunks
  list<chunkItem *> colorchanges;
  curfg = startattr.fg;
  curbg = startattr.bg;
  for (int i = 1; i < len; i++)
  {
    if (newcolors[i].fg != curfg)
    {
      curfg = newcolors[i].fg;
      chunkFg *chfg = new chunkFg;
      chfg->setFg (curfg);
      chfg->setStartPos (startattr.startpos + i);
      colorchanges.push_back (chfg);
    }
    if (newcolors[i].bg != curbg)
    {
      curbg = newcolors[i].bg;
      chunkBg *chbg = new chunkBg;
      chbg->setBg (curbg);
      chbg->setStartPos (startattr.startpos + i);
      colorchanges.push_back (chbg);
    }
  }
  
  //fill in the generated color changes
  list<chunkItem *>::iterator ii2 = colorchanges.begin ();
  //only if there is at least one such color change (initial color is not counted into this)
  if (ii2 != colorchanges.end())
  {
    ii = _entries.begin();
    while ((ii != _entries.end()) && (ii2 != colorchanges.end()))
    {
      int pos = (*ii)->startPos ();
      int len = (*ii)->length ();
      int chpos = (*ii2)->startPos();
      if (len == 0)  //zero-length items are not interesting here, so we just skip them
      {
        ++ii;
        continue;
      }
      if (chpos >= pos + len)  //this item won't be affected...
      {
        ++ii;
        continue;
      }
      //if we are here, then this item WILL be affected
      if (chpos > pos)  //we need to split the text if this one is true
      {
        chunkItem *item = (*ii)->split (chpos - pos - 1);
        if (item)
        {
          ++ii;
          _entries.insert (ii, item);  //insert BEFORE iterator position
          --ii;  //okay, now we are between the two splitted chunk items
        }
      }
      
      //now we insert the colorization commands and then we can move on to the next one
      pos = (*ii)->startPos ();
      while ((ii2 != colorchanges.end()) && ((*ii2)->startPos() == pos))
      {
        chunkItem *item = *ii2;
        _entries.insert (ii, item);  //insert colorization command before our chunk item...
        ++ii2;
      }
    }
  }
  
  //simplify the result
  simplify ();
  
  //free up memory
  delete[] colors;
  delete[] newcolors;

  //update the timestamp
  timestamp = QDateTime::currentDateTime();
}

void cTextChunk::replace (int pos, int len, const QString newtext)
{
  list<chunkItem *>::iterator startit, endit;
  int spos, l;
  
  //step 1: prepare replacement chunk
  chunkText *chunk = nullptr;
  if (!newtext.isEmpty())
  {
    chunk = new chunkText;
    chunk->setText (newtext);
    chunk->setStartPos (pos);
  }

  //step 2: find position for the new chunk
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    spos = (*it)->startPos ();
    l = (*it)->length ();
    if (l && (pos >= spos) && (pos <= spos+l))  //chunk of non-zero length containing pos
    {
      //alright, this is the starting position!
      startit = it;
      break;
    }
  }
  if (it == _entries.end())  //too far - appending to the very end
  {
    appendEntry (chunk);
    return;
  }
  
  //step 3: find final position
  for (; it != _entries.end(); ++it)
  {
    spos = (*it)->startPos ();
    l = (*it)->length ();
    if (pos+len <= spos+l)
    {
      //alright, this is the ending position!
      endit = it;
      break;
    }
  }
  if (it == _entries.end())
    endit = (_entries.end())--;  //can do that - would stop at step 2 if the list were empty

  //step 4: handle (quite common) situation, where starting and ending chunks are the same
  if (startit == endit)
  {
    (*startit)->replace (pos - (*startit)->startPos(), len, newtext);
    delete chunk;  // not needed after all :)

    fixupStartPositions ();
    return;
  }
  
  //step 5: split margin chunks if needed
  //starting...
  spos = (*startit)->startPos ();
  if (pos > spos)
  {
    chunkItem *ch = (*startit)->split (pos - spos - 1);
    startit++;
    if (ch)
      delete ch;  //this one is a part of the replaced text, hence we don't need it anymore
  }
  spos = (*endit)->startPos ();
  l = (*endit)->length ();
  if (pos+len < spos+l)
  {
    chunkItem *ch = (*endit)->split (pos+len - spos - 1);
    endit++;
    endit = _entries.insert (endit, ch);
    endit--;
  }
  
  //step 6: put replacement chunk to place
  if (chunk) _entries.insert (startit, chunk);
  
  //step 7: delete old chunks
  endit++;
  it = startit;
  while (it != endit)
  {
    delete *it;
    it = _entries.erase (it);
  }
  
  fixupStartPositions ();
}

bool cTextChunk::expireNamedLinks (const QString &name)
{
  //did any link expire?
  bool expired = false;
  
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    if ((*it)->type() == CHUNK_LINK)
    {
      chunkLink *chl = (chunkLink *) *it;
      if (chl->name().isEmpty())
        continue;  //nothing is the link has no name
      
      if ((name == chl->name()) || (name == QString()))
      {
        //this link should expire
        chunkText *cht = new chunkText;  //create a static text
        cht->setText (chl->text());
        cht->setStartPos (chl->startPos ());
        delete *it;  //delete the link
        it = _entries.erase (it);  //remove the link from the list
        it = _entries.insert (it, cht);  //add static text to its place, make iterator point to it
      }
    }
  
  return expired;
}

QString cTextChunk::plainText ()
{
  QString s;
  //start with some spaces
  if (startattr.startpos)
    s.fill (' ', startattr.startpos);
  //then add all the texts...
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    if ((*it)->type() == CHUNK_TEXT)
      s += ((chunkText *) *it)->text();
    if ((*it)->type() == CHUNK_LINK)
      s += ((chunkLink *) *it)->text();
  }
  return s;
}

QStringList cTextChunk::words (int minLength)
{
  QString t = plainText();
  //create the list of words
  // Regexp splitting of words to remove special characters. Added by Magnus Lundborg 051005
  QStringList res = t.split (QRegularExpression("[\\s\\.\\,\\(\\)\\[\\]\\?\\!\\:\\;\"\']"));
  //remove words that are too short
  QStringList::iterator it = res.begin ();
  while (it != res.end())
    if ((*it).length() < minLength)
      it = res.erase (it);
    else
      ++it;
  return res;
}

int cTextChunk::length ()
{
  int len = 0;
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    len += (*it)->length();
  return len;
}

void cTextChunk::simplify ()
{
  //This is just an optimalization method - i.e., everything will work without it, only that
  //it might be slower...
  
  //NOTHING HERE YET
  
}

cTextChunk *cTextChunk::duplicate ()
{
  cTextChunk *chunk = new cTextChunk (console);
  chunk->setStartAttr (startattr);
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    chunkItem *item = (*it)->duplicate ();
    chunk->_entries.push_back (item);
  }
  chunk->fixupStartPositions ();
  return chunk;
}

// TODO - we don't need this any more
/*
void cTextChunk::paint (int length, int selstart, int sellen,
    int charWidth, int charHeight,
    QPainter *painter, QPainter *blinkpainter)
{
  //prepare the paintStruct structure
  pstatus->length = length;
  pstatus->selstart = selstart;
  pstatus->sellen = sellen;
  pstatus->charWidth = charWidth;
  pstatus->charHeight = charHeight;
  pstatus->cellHeight = console->cellHeight();
  pstatus->dontShowText = false;
  pstatus->blinking = false;
  pstatus->negative = false;
  pstatus->invisible = false;
  
  //fill in initial attributes
  pstatus->paintColor = startattr.fg;
  pstatus->fillColor = startattr.bg;
  pstatus->defbkcolor = console->defaultBkColor();
  pstatus->paintFont = console->font();
  pstatus->defaultFont = console->font();

  if (startattr.attrib & ATTRIB_BOLD)
    pstatus->paintFont.setBold (true);
  if (startattr.attrib & ATTRIB_ITALIC)
    pstatus->paintFont.setItalic (true);
  if (startattr.attrib & ATTRIB_UNDERLINE)
    pstatus->paintFont.setUnderline (true);
  if (startattr.attrib & ATTRIB_STRIKEOUT)
    pstatus->paintFont.setStrikeOut (true);
  if (startattr.attrib & ATTRIB_BLINK)
    pstatus->blinking = true;
  if (startattr.attrib & ATTRIB_NEGATIVE)
    pstatus->negative = true;
  if (startattr.attrib & ATTRIB_INVISIBLE)
    pstatus->invisible = true;
  
  //go and paint! (space 0..startpos is NOT painted as of now!)
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    pstatus->dontShowText = false;
    painter->setBackground (pstatus->defbkcolor);
    (*it)->paint (painter, pstatus);
    if (blinkpainter)
    {
      if (pstatus->blinking)
        pstatus->dontShowText = true;
      blinkpainter->setBackground (pstatus->defbkcolor);
      (*it)->paint (blinkpainter, pstatus);
    }
  }
}
*/

QString cTextChunk::toText ()
{
  QString s;
  //start with some spaces
  if (startattr.startpos)
    s.fill (' ', startattr.startpos);
  //then add all the texts...
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    s += (*it)->toText();
  return s;
}

QString cTextChunk::toAnsi (cANSIParser *ap)
{
  QString s;
  
  //starting attributes
  s += "\x1b[0m";
  s += chunkFg::constructAnsi (startattr.fg, ap);
  s += chunkBg::constructAnsi (startattr.bg, ap);
  s += chunkAttrib::constructAnsi (startattr.attrib);
  
  //some spaces if needed
  if (startattr.startpos)
    s.fill (' ', startattr.startpos);
  //then add all the texts...
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    s += (*it)->toAnsi (ap);
  return s;
}

QString cTextChunk::toHTML ()
{
  QString s, suffix;

  //starting attributes
  s += chunkFg::constructHTML (startattr.fg, suffix);
  s += chunkBg::constructHTML (startattr.bg, suffix);
  
  //some spaces if needed
  if (startattr.startpos)
    s.fill (' ', startattr.startpos);
  //then add all the texts...
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
    s += (*it)->toHTML (suffix);
  s += suffix;
  return "<p>" + s + "</p>";
}

void cTextChunk::insertToDocument (QTextCursor &cursor)
{
  QTextCharFormat format;
  chunkFg::setFormat (format, startattr.fg);
  chunkBg::setFormat (format, startattr.bg);
  chunkAttrib::setFormat (format, startattr.attrib);

/*  I think this isn't needed anymore ...
  if (startattr.startpos)
  {
    QString s;
    s.fill (' ', startattr.startpos);
    cursor.insertText (s, format);
  }
*/

  for (chunkItem *item : _entries)
    item->insertToDocument (cursor, format);
}


cTextChunk *cTextChunk::makeLine (const QString &text, QColor fg, QColor bg, cConsole *console)
{
  cTextChunk *chunk = new cTextChunk (console);
  chunkStart startattr;
  startattr.attrib = 0;
  startattr.startpos = 0;
  startattr.fg = fg;
  startattr.bg = bg;
  
  chunk->setStartAttr (startattr);
  chunkFg *chfg = new chunkFg;
  chfg->setFg (fg);
  chunkBg *chbg = new chunkBg;
  chbg->setBg (bg);
  chunkText *cht = new chunkText;
  cht->setText (text);
  
  chfg->setStartPos (0);
  chbg->setStartPos (0);
  cht->setStartPos (0);
  chunk->appendEntry (chfg);
  chunk->appendEntry (chbg);
  chunk->appendEntry (cht);
  return chunk;
}

QDateTime cTextChunk::getTimeStamp ()
{
  return timestamp;
}

void cTextChunk::fixupStartPositions ()
{
  int pos = startattr.startpos;
  list<chunkItem *>::iterator it;
  for (it = _entries.begin(); it != _entries.end(); ++it)
  {
    (*it)->setStartPos (pos);
    pos += (*it)->length ();
  }
}

// --- chunkItem-derived stuff starts here ---

QString chunkText::toHTML (QString &)
{
  QString res;
  for (int i = 0; i < _text.length(); ++i) {
    if (_text[i] == '<')
      res += "&lt;";
    else if (_text[i] == '>')
      res += "&gt;";
    else if (_text[i] == '&')
      res += "&amp;";
    else
      res += _text[i];
  }
  return res;
}

chunkItem *chunkText::split (int pos)
{
  if ((pos < 0) || (pos >= length()-1))
    return nullptr;    //do NOTHING
  chunkText *cht = new chunkText;
  int l1 = pos + 1;
  int l2 = _text.length() - l1;
  cht->setText (_text.right (l2));
  setText (_text.left (l1));
  cht->setStartPos (startPos() + l1);
  return cht;
}

chunkItem *chunkLink::split (int pos)
{
  if ((pos < 0) || (pos >= length()-1))
    return nullptr;    //do NOTHING
  chunkLink *chl = (chunkLink *) duplicate ();
  int l1 = pos + 1;
  int l2 = _text.length() - l1;
  chl->setText (_text.right (l2));
  setText (_text.left (l1));
  chl->setStartPos (startPos() + l1);
  
  return chl;
}

chunkItem *chunkText::duplicate()
{
  chunkText *item = new chunkText;
  item->startpos = startpos;
  item->_text = _text;
  return item;
}

chunkItem *chunkFg::duplicate()
{
  chunkFg *item = new chunkFg;
  item->startpos = startpos;
  item->_fg = _fg;
  return item;
}

chunkItem *chunkBg::duplicate()
{
  chunkBg *item = new chunkBg;
  item->startpos = startpos;
  item->_bg = _bg;
  return item;
}

chunkItem *chunkAttrib::duplicate()
{
  chunkAttrib *item = new chunkAttrib;
  item->startpos = startpos;
  item->_attrib = _attrib;
  return item;
}

chunkItem *chunkLink::duplicate()
{
  chunkLink *item = new chunkLink;
  item->startpos = startpos;
  item->_name = _name;
  item->_target = _target;
  item->_text = _text;
  item->_hint = _hint;
  item->_iscommand = _iscommand;
  item->_toprompt = _toprompt;
  item->_ismenu = _ismenu;
  return item;
}

void chunkText::trimLeft ()
{
  int len;
  int tlen = _text.length();
  for (len = 0; len < tlen; ++len)
    if (!_text[len].isSpace())
      break;
  if (len)
    _text.remove (0, len);
}

void chunkLink::trimLeft ()
{
  int len;
  int tlen = _text.length();
  for (len = 0; len < tlen; ++len)
    if (!_text[len].isSpace())
      break;
  if (len)
    _text.remove (0, len);
}

void chunkText::replace (int pos, int len, const QString &newtext)
{
  _text = _text.replace (pos, len, newtext);
}

void chunkLink::replace (int pos, int len, const QString &newtext)
{
  _text = _text.replace (pos, len, newtext);
}

void chunkLink::parseMenu ()
{
  _menu.clear();
  if (!isMenu())
    return;

  QStringList targets = _target.split ('|', Qt::KeepEmptyParts);
  QStringList hints = _hint.split ('|', Qt::KeepEmptyParts);
  //first hint is the main hint, what follows (if anything) are hints for individual commands
  if (!hints.empty())
  {
    _hint = hints.first();
    hints.pop_front ();
  }
  
  //now fill up the relevant structures
  QStringList::iterator it1, it2;
  it2 = hints.begin();
  for (it1 = targets.begin(); it1 != targets.end(); ++it1)
  {
    menuItem mi;
    mi.command = *it1;
    if (it2 != hints.end())
      mi.caption = *it2;
    else
      //no hint given - command used
      mi.caption = mi.command;
    
    _menu.push_back (mi);
    if (it2 != hints.end()) ++it2;
  }
}

/*
void chunkItem::paintText (const QString &text, QPainter *painter, QFont font,
    QColor fg, QColor bg, paintStatus *ps)
{
  //draw the text, including selection if needed...
  int endpos = startpos + text.length() - 1;
  int endsel = ps->selstart + ps->sellen - 1;
  QString t = text;
  if (endpos + 1 > ps->length)
    //text won't fit into console - trim it!
    t.truncate (ps->length - startpos);
  
  if ((ps->selstart == -1) || (ps->sellen == 0) ||
      (endpos < ps->selstart) || (startpos > endsel))
  {
    //simple case - no selection in this chunk...
    painter->setPen (fg);
    painter->setBackground (bg);
    painter->setFont (font);
    if (bg != ps->defbkcolor)
      //only paint if non-default background color is to be used
      painter->fillRect (ps->charWidth * startpos, 0, ps->charWidth * t.length(),
          ps->cellHeight, bg);
    if (((!ps->blinking) || (!ps->dontShowText)) && (!ps->invisible))
      painter->drawText (ps->charWidth * startpos, ps->charHeight, t);
  }
  else
  {
    //some of this text is selected
    QString t1, t2, t3;  //t1 = before, t2 = selected, t3 = after
    int len1, len3;
    if (ps->selstart <= startpos)
      len1 = 0;
    else
      len1 = ps->selstart - startpos;
    if (endsel >= endpos)
      len3 = 0;
    else
      len3 = endpos - endsel;
    if (len1) t1 = t.left (len1);
    if (len3) t3 = t.right (len3);
    t2 = t.mid (len1, t.length() - len1 - len3);
    
    //good, now draw each part of the text...
    if (len1)
    {
      painter->setPen (fg);
      painter->setBackground (bg);
      painter->setFont (font);
      if (bg != ps->defbkcolor)
        //only paint if non-default background color is to be used
        painter->fillRect (ps->charWidth * startpos, 0, ps->charWidth * len1,
            ps->cellHeight, bg);
      if (((!ps->blinking) || (!ps->dontShowText)) && (!ps->invisible))
        painter->drawText (ps->charWidth * startpos, ps->charHeight, t1);
    }
    
    //we KNOW that t2 is not empty :D
    //this is the selection, painted in reversed colors...
    painter->setPen (bg);  //REVERSE colors
    painter->setBackground (fg);  //REVERSE colors
    painter->setFont (font);
    if (fg != ps->defbkcolor)
      //only paint if non-default background color is to be used
      // (this is the selection, so the condition will usually be true, but hey...)
      painter->fillRect (ps->charWidth * (startpos + len1), 0, ps->charWidth * t2.length(),
          ps->cellHeight, fg);
    if (((!ps->blinking) || (!ps->dontShowText)) && (!ps->invisible))
      painter->drawText (ps->charWidth * (startpos + len1), ps->charHeight, t2);
    
    if (len3)
    {
      painter->setPen (fg);
      painter->setBackground (bg);
      painter->setFont (font);
      if (bg != ps->defbkcolor)
        //only paint if non-default background color is to be used
        painter->fillRect (ps->charWidth * (startpos+len1+t2.length()), 0, ps->charWidth * len3,
            ps->cellHeight, bg);
      if (((!ps->blinking) || (!ps->dontShowText)) && (!ps->invisible))
        painter->drawText (ps->charWidth * (startpos + len1 + t2.length()), ps->charHeight, t3);
    }
  }
}

void chunkText::paint (QPainter *painter, paintStatus *ps)
{
  if (startpos >= ps->length)  //we're past console width
    return;
  QColor paintColor = ps->paintColor;
  QColor fillColor = ps->fillColor;
  if (ps->negative)  //negative image - invert colors
  {
    paintColor = ps->fillColor;
    fillColor = ps->paintColor;
  }
  
  //okay, paint the text!
  paintText (_text, painter, ps->paintFont, paintColor, fillColor, ps);
}

void chunkFg::paint (QPainter *, paintStatus *ps)
{
  if (startpos >= ps->length)  //we're past console width
    return;

  //set text color
  ps->paintColor = _fg;
}

void chunkBg::paint (QPainter *, paintStatus *ps)
{
  if (startpos >= ps->length)  //we're past console width
    return;

  //set text background
  ps->fillColor = _bg;
}

void chunkAttrib::paint (QPainter *, paintStatus *ps)
{
  if (startpos >= ps->length)  //we're past console width
    return;

  //set font back to default one...
  ps->paintFont = ps->defaultFont;
  ps->blinking = false;
  ps->negative = false;
  ps->invisible = false;

  //set some font attributes and stuff
  if (_attrib & ATTRIB_BOLD)
    ps->paintFont.setBold (true);
  if (_attrib & ATTRIB_ITALIC)
    ps->paintFont.setItalic (true);
  if (_attrib & ATTRIB_UNDERLINE)
    ps->paintFont.setUnderline (true);
  if (_attrib & ATTRIB_STRIKEOUT)
    ps->paintFont.setStrikeOut (true);
  if (_attrib & ATTRIB_BLINK)
    ps->blinking = true;
  if (_attrib & ATTRIB_NEGATIVE)
    ps->negative = true;
  if (_attrib & ATTRIB_INVISIBLE)
    ps->invisible = true;
}

void chunkLink::paint (QPainter *painter, paintStatus *ps)
{
  if (startpos >= ps->length)  //we're past console width
    return;
  QColor paintColor = linkColor;
  QColor fillColor = ps->fillColor;
  if (ps->negative)  //negative image - invert colors
  {
    paintColor = ps->fillColor;
    fillColor = ps->paintColor;
  }
  
  QFont font = ps->paintFont;
  font.setUnderline (true);
  
  //okay, paint the text!
  paintText (_text, painter, font, paintColor, fillColor, ps);
}
*/

QString chunkFg::constructAnsi (QColor color, cANSIParser *ap)
{
  QColor colors[16];
  for (int i = 0; i < 16; i++)
  {
    colors[i] = ap->color (i);
    if (colors[i] == color)    //exactly this color
    {
      QString s = "\x1b[";
      if (i >= 8)
        s += "1;";
      s += QString::number (30 + i % 8);
      s += "m";
      return s;
    }
  }
  
  //no exact color match - this color did not originate as an ANSI sequence...
  //we need to find the best substitute for the color...
  int subst = 0;
  int bestmatch = abs (colors[0].red() - color.red()) + abs (colors[0].green() - color.green()) +
        abs (colors[0].blue() - color.blue());
  for (int i = 1; i < 16; i++)
  {
    int match = abs (colors[i].red() - color.red()) + abs (colors[i].green() - color.green()) +
          abs (colors[i].blue() - color.blue());
    if (match < bestmatch)
    {
      subst = i;
      match = bestmatch;
    }
  }
      
  QString s = "\x1b[";
  if (subst >= 8)
    s += "1;";
  s += QString::number (30 + subst % 8);
  s += "m";
  return s;
}

QString chunkBg::constructAnsi (QColor color, cANSIParser *ap)
{
  //similar to chunkFg::constructANSI, only that it only uses the first 8 colors...
  
  QColor colors[8];
  for (int i = 0; i < 8; i++)
  {
    colors[i] = ap->color (i);
    if (colors[i] == color)    //exactly this color
    {
      QString s = "\x1b[";
      s += QString::number (40 + i);
      s += "m";
      return s;
    }
  }
  
  //no exact color match - this color did not originate as an ANSI sequence...
  //we need to find the best substitute for the color...
  int subst = 0;
  int bestmatch = abs (colors[0].red() - color.red()) + abs (colors[0].green() - color.green()) +
        abs (colors[0].blue() - color.blue());
  for (int i = 1; i < 8; i++)
  {
    int match = abs (colors[i].red() - color.red()) + abs (colors[i].green() - color.green()) +
          abs (colors[i].blue() - color.blue());
    if (match < bestmatch)
    {
      subst = i;
      match = bestmatch;
    }
  }
      
  QString s = "\x1b[";
  s += QString::number (40 + subst);
  s += "m";
  return s;
}

QString chunkAttrib::constructAnsi (unsigned char attrib)
{
  //not very effective, eh? :D
  QString s;
  if (attrib & ATTRIB_BOLD)
    s += "\x1b[1m";  //bold
  else
    s += "\x1b[22m";  //bold off
/*
  // All these attributes are now DISABLED
  // The reason is that they add quite a lot of unnecessary stuff into
  // transcript files - might be re-enabled if needed, but some output
  // optimization might be necessary (things like don't disable italics
  // if we know that it's off, and so)

  if (attrib & ATTRIB_ITALIC)
    s += "\x1b[3m";  //italic
  else
    s += "\x1b[23m";  //italic off
  if (attrib & ATTRIB_UNDERLINE)
    s += "\x1b[4m";  //underline
  else
    s += "\x1b[24m";  //underline off
  if (attrib & ATTRIB_STRIKEOUT)
    s += "\x1b[9m";  //strikeout
  else
    s += "\x1b[29m";  //strikeout off
  if (attrib & ATTRIB_BLINK)
    s += "\x1b[5m";  //blink
  else
    s += "\x1b[25m";  //blink off
  if (attrib & ATTRIB_NEGATIVE)
    s += "\x1b[5m";  //negative
  else
    s += "\x1b[25m";  //negative off
  if (attrib & ATTRIB_INVISIBLE)
    s += "\x1b[7m";  //invisible
  else
    s += "\x1b[27m";  //invisible off
*/
  return s;
}

QString chunkFg::toAnsi (cANSIParser *ap)
{
  return chunkFg::constructAnsi (_fg, ap);
}

QString chunkBg::toAnsi (cANSIParser *ap)
{
  return chunkBg::constructAnsi (_bg, ap);
}

QString chunkAttrib::toAnsi (cANSIParser *)
{
  return chunkAttrib::constructAnsi (_attrib);
}

QString chunkLink::toAnsi (cANSIParser *ap)
{
  QString s = chunkFg::constructAnsi (linkColor, ap);
  s += _text;
  return s;
}

QString chunkFg::constructHTML (QColor color, QString &suffix)
{
  suffix = "</font>" + suffix;
  return "<font style=\"color: " + color.name() + "\">";
}

QString chunkBg::constructHTML (QColor, QString &)
{
  //NOTHING HERE AS OF NOW!
  return QString();
}

QString chunkFg::toHTML (QString &suffix)
{
  return chunkFg::constructHTML (_fg, suffix);
}

QString chunkBg::toHTML (QString &suffix)
{
  return chunkBg::constructHTML (_bg, suffix);
}

QColor chunkLink::linkColor = Qt::blue;
QString chunkLink::toHTML (QString &)
{
  QString rel = _iscommand ? "command" : "link";
  QString href = rel + " " + (_toprompt ? "prompt" : "send") + " " + _target;
  return "<a rel=\"" + rel + "\" href=\"" + href + "\">" + _text + "</a>";
}

void chunkLink::insertToDocument (QTextCursor &cursor, QTextCharFormat &format) {
  QTextCharFormat linkformat = format;
  linkformat.setAnchor (true);
  QString href = QString(_iscommand ? "command" : "link") + " " + (_toprompt ? "prompt" : "send") + " " + _target;
  linkformat.setAnchorHref (href);
  if (linkformat.foreground().color() == _chunk->startAttr().fg)
    linkformat.setForeground (linkColor);
  cursor.insertText (_text, linkformat);
}

void chunkFg::setFormat (QTextCharFormat &format, QColor color)
{
  format.setForeground (color);
}

void chunkBg::setFormat (QTextCharFormat &format, QColor color)
{
  format.setBackground (color);
}

void chunkAttrib::setFormat (QTextCharFormat &format, int attrib)
{
  format.setFontWeight ((attrib & ATTRIB_BOLD) ? QFont::Bold : QFont::Normal);
  format.setFontItalic (attrib & ATTRIB_ITALIC);
  format.setFontUnderline (attrib & ATTRIB_UNDERLINE);
  format.setFontStrikeOut (attrib & ATTRIB_STRIKEOUT);
  // blink, negative, and invisible are currently not supported
}


