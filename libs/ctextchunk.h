//
// C++ Interface: ctextchunk
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

#ifndef CTEXTCHUNK_H
#define CTEXTCHUNK_H

#include <QColor>
#include <QDateTime>
#include <QFont>
#include <QString>

#include <list>
#include <kmuddy_export.h>

using namespace std;

class cANSIParser;
class cConsole;

class QPainter;

struct paintStatus;

/** one item in a cTextChunk chunk - abstract base class */

class KMUDDY_EXPORT chunkItem {
 public:
  virtual ~chunkItem() {};
  virtual int type() = 0;
  int startPos () { return startpos; }
  void setStartPos (int sp) { startpos = sp; }
  virtual int length() = 0;
  virtual chunkItem *split (int) { return 0; }
  virtual void trimLeft () {};
  virtual chunkItem *duplicate() = 0;

  virtual void replace (int, int, const QString &) {};
  
  //painting the text...
  virtual void paint (QPainter *painter, paintStatus *ps) = 0;
    
  //output to transcript...
  /** plain-text output */
  virtual QString toText () { return QString(); };
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *) { return QString(); };
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &) { return QString(); };
 protected:
  void paintText (const QString &text, QPainter *painter, QFont font, QColor fg, QColor bg,
      paintStatus *ps);
  int startpos;
};

/** one color change made by color triggers */

struct colorChange {
  int start, len;
  QColor fg, bg;
  bool keepfg, keepbg;
};

/** starting chunk attributes */

struct chunkStart {
  int startpos;
  QColor fg, bg;
  int attrib;
};


/**
One chunk of text, including attributes, links and all that stuff.
Contains methods to break lines and so on.

@author Tomas Mecir
*/

class KMUDDY_EXPORT cTextChunk {
public:
  /** constructor */
  cTextChunk (cConsole *_console);
  /** constructor that also adds some text */
  cTextChunk (cConsole *_console, const QString &text);
  /** destructor */
  ~cTextChunk ();
  /** get entries... */
  list<chunkItem *> entries() { return _entries; };

  /** get starting attributes */
  chunkStart startAttr () { return startattr; };
  /** set starting attributes */
  void setStartAttr (chunkStart attr) { startattr = attr; };
      
  /** append entry to the chunk */
  void appendEntry (chunkItem *entry);
  /** append chunk2 to this chunk; chunk2 will then be DELETED */
  void append (cTextChunk *chunk2);

  /** returns chunkItem at a given position */
  chunkItem *itemAt (int pos);

  /** split the line in two, this object will become line 1, method returns line 2
  Method returns 0 if current text is not long enough. */
  cTextChunk *splitLine (int idx, bool wordwrap = true, int indent = 0, bool trimSpaces = true);

  /** apply color changes made by color triggers, keep everything else intact */
  void applyColorChanges (list<colorChange> &changes);

  /** replace text at given position with a new text */
  void replace (int pos, int len, const QString newtext);
    
  /** expire all links with given name, or all named links if no name given */
  bool expireNamedLinks (const QString &name = QString());

  /** return plain-text version of this chunk */
  QString plainText ();

  QStringList words (int minLength = 3);
  
  /** length of the chunk, EXCLUDING start position */
  int length ();
  
  /** simplify the chunk, removing unneeded items and merging where possible */
  void simplify ();
  
  /** create an exact copy of this chunk */
  cTextChunk *duplicate ();

  //painting...
  void paint (int length, int selstart, int sellen,
      int charWidth, int charHeight,
      QPainter *painter, QPainter *blinkpainter = 0);
  
  //output to transcript...
  /** plain-text output */
  QString toText ();
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *ap);
  /** output to HTML */
  QString toHTML ();

  //get timestamp in a textual form
  QString getTimeStamp ();
  
  /** create one line that's all in one color */
  static cTextChunk *makeLine (const QString &text, QColor fg, QColor bg, cConsole *console);
protected:
  void init (cConsole *_console);
  void fixupStartPositions ();
  
  list<chunkItem *> _entries;
  
  /** starting attributes... */
  chunkStart startattr;
  
  /** cConsole object, used when needed */
  cConsole *console;

  /** timestamp, used in console's tooltip */
  QDateTime timestamp;
    
  /** paint status used by paint() */
  paintStatus *pstatus;
};


//chunkItem-derived classes...

#define CHUNK_TEXT 1
class KMUDDY_EXPORT chunkText : public chunkItem {
 public:
  virtual int type() { return CHUNK_TEXT; };
  
  const QString &text() { return _text; }
  void setText (const QString &t) { _text = t; }
  virtual int length() { return _text.length(); }
    //pos is index of last index that will remain in this item
  virtual chunkItem *split (int pos);
  virtual chunkItem *duplicate();
  virtual void trimLeft ();
  virtual void replace (int pos, int len, const QString &newtext);

  //painting
  virtual void paint (QPainter *painter, paintStatus *ps);
  
  //output to transcript...
  /** plain-text output */
  virtual QString toText () { return _text; };
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *) {return _text; };
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &);
 protected:
  QString _text;
};

#define CHUNK_FG 2
class KMUDDY_EXPORT chunkFg : public chunkItem {
 public:
  virtual int type() { return CHUNK_FG; };
  
  QColor fg() { return _fg; }
  void setFg (QColor fgc) { _fg = fgc; }
  virtual int length() { return 0; }
  virtual chunkItem *duplicate();
 
  //painting
  virtual void paint (QPainter *painter, paintStatus *ps);
  
  //output to transcript...
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *ap);
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &suffix);
  
  static QString constructAnsi (QColor color, cANSIParser *ap);
  static QString constructHTML (QColor color, QString &suffix);
 protected:
  QColor _fg;
};

#define CHUNK_BG 3
class KMUDDY_EXPORT chunkBg : public chunkItem {
 public:
  virtual int type() { return CHUNK_BG; };
  
  QColor bg() { return _bg; }
  void setBg (QColor bgc) { _bg = bgc; }
  virtual int length() { return 0; }
  virtual chunkItem *duplicate();
 
  //painting
  virtual void paint (QPainter *painter, paintStatus *ps);
  
  //output to transcript...
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *ap);
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &suffix);
  
  static QString constructAnsi (QColor color, cANSIParser *ap);
  static QString constructHTML (QColor color, QString &suffix);
 protected:
  QColor _bg;
};

//attributes...

#define ATTRIB_BOLD 1  
#define ATTRIB_ITALIC 2
#define ATTRIB_UNDERLINE 4
#define ATTRIB_STRIKEOUT 8
#define ATTRIB_BLINK 16
#define ATTRIB_NEGATIVE 32
#define ATTRIB_INVISIBLE 64

#define CHUNK_ATTRIB 4
class KMUDDY_EXPORT chunkAttrib : public chunkItem {
 public:
  virtual int type() { return CHUNK_ATTRIB; };
  int attrib() { return _attrib; }
  void setAttrib (int a) { _attrib = a; }
  virtual int length() { return 0; }
  virtual chunkItem *duplicate();
 
  //painting
  virtual void paint (QPainter *painter, paintStatus *ps);
  
  //output to transcript...
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *);
  
  //no HTML output here - we cannot handle closing tags properly without too much hassle
  
  static QString constructAnsi (unsigned char attrib);
 protected:
  int _attrib;
};

struct menuItem {
  QString caption;
  QString command;
};

#define CHUNK_LINK 5
class KMUDDY_EXPORT chunkLink : public chunkItem {
 public:
  virtual int type() { return CHUNK_LINK; };

  QString name() { return _name; }
  void setName (const QString &n) { _name = n; }
  QString target() { return _target; }
  void setTarget (const QString &t) { _target = t; }
  QString text() { return _text; }
  void setText (const QString &t) { _text = t; }
  QString hint() { return _hint; }
  void setHint (const QString &h) { _hint = h; }
  
  bool isCommand () { return _iscommand; };
  void setIsCommand (bool val) { _iscommand = val; };
  bool toPrompt () { return _toprompt; };
  void setToPrompt (bool val) { _toprompt = val; };
  bool isMenu () { return _ismenu; };
  void setIsMenu (bool val) { _ismenu = val; };

  /** parse menu information */
  void parseMenu ();
  
  virtual int length() { return _text.length(); }
  virtual chunkItem *split (int pos);
  virtual chunkItem *duplicate ();
  virtual void trimLeft ();
  virtual void replace (int pos, int len, const QString &newtext);
 
  const list<menuItem> &menu() { return _menu; };
  //painting
  virtual void paint (QPainter *painter, paintStatus *ps);
  
  //output to transcript...
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *ap);
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &suffix);
 protected:
  QString _name, _target, _text, _hint;
  bool _iscommand, _toprompt, _ismenu;
  
  list<menuItem> _menu;
  
  static QColor linkColor;
};


#endif
