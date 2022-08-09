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
#include <QTextCharFormat>
#include <QTextCursor>

#include <list>
#include <kmuddy_export.h>

using namespace std;

class cANSIParser;
class cConsole;
class cTextChunk;

class QPainter;

// struct paintStatus;

/** one item in a cTextChunk chunk - abstract base class */

class KMUDDY_EXPORT chunkItem {
 public:
  chunkItem () : startpos(0), _chunk(nullptr) {}
  virtual ~chunkItem() {};
  virtual int type() = 0;
  int startPos () { return startpos; }
  void setStartPos (int sp) { startpos = sp; }
  virtual int length() = 0;
  virtual chunkItem *split (int) { return nullptr; }
  virtual void trimLeft () {};
  virtual chunkItem *duplicate() = 0;

  virtual void replace (int, int, const QString &) {};
  
  //painting the text...
//  virtual void paint (QPainter *painter, paintStatus *ps) = 0;
    
  //output to transcript...
  /** plain-text output */
  virtual QString toText () { return QString(); };
  /** output to plain-text with ANSI sequences */
  virtual QString toAnsi (cANSIParser *) { return QString(); };
  /** output to HTML, suffix can be used to provide closing tags if needed */
  virtual QString toHTML (QString &) { return QString(); };

  /** Insert self into a text document at the QTextCursor's position */
  virtual void insertToDocument (QTextCursor &, QTextCharFormat &) = 0;
 protected:
//  void paintText (const QString &text, QPainter *painter, QFont font, QColor fg, QColor bg, paintStatus *ps);
  int startpos;
  cTextChunk *_chunk;
  friend class cTextChunk;
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
//  void paint (int length, int selstart, int sellen, int charWidth, int charHeight, QPainter *painter, QPainter *blinkpainter = 0);
  
  //output to transcript...
  /** plain-text output */
  QString toText ();
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *ap);
  /** output to HTML */
  QString toHTML ();

  void insertToDocument (QTextCursor &cursor);
  //get timestamp in a textual form
  QDateTime getTimeStamp ();
  
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
//  paintStatus *pstatus;
};


//chunkItem-derived classes...

#define CHUNK_TEXT 1
class KMUDDY_EXPORT chunkText : public chunkItem {
 public:
  int type() override { return CHUNK_TEXT; };
  
  const QString &text() { return _text; }
  void setText (const QString &t) { _text = t; }
  int length() override { return _text.length(); }
    //pos is index of last index that will remain in this item
  chunkItem *split (int pos) override;
  chunkItem *duplicate() override;
  void trimLeft () override;
  void replace (int pos, int len, const QString &newtext) override;

  void insertToDocument (QTextCursor &cursor, QTextCharFormat &format) override { cursor.insertText (_text, format); };

  //painting
//  virtual void paint (QPainter *painter, paintStatus *ps) override;
  
  //output to transcript...
  /** plain-text output */
  QString toText () override { return _text; };
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *) override {return _text; };
  /** output to HTML, suffix can be used to provide closing tags if needed */
  QString toHTML (QString &) override;
 protected:
  QString _text;
};

#define CHUNK_FG 2
class KMUDDY_EXPORT chunkFg : public chunkItem {
 public:
  int type() override { return CHUNK_FG; };
  
  QColor fg() { return _fg; }
  void setFg (QColor fgc) { _fg = fgc; }
  int length() override { return 0; }
  chunkItem *duplicate() override;
 
  //painting
//  virtual void paint (QPainter *painter, paintStatus *ps) override;
  
  void insertToDocument (QTextCursor &, QTextCharFormat &format) override { setFormat (format, _fg); };

  //output to transcript...
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *ap) override;
  /** output to HTML, suffix can be used to provide closing tags if needed */
  QString toHTML (QString &suffix) override;

  static void setFormat (QTextCharFormat &format, QColor color);
  static QString constructAnsi (QColor color, cANSIParser *ap);
  static QString constructHTML (QColor color, QString &suffix);
 protected:
  QColor _fg;
};

#define CHUNK_BG 3
class KMUDDY_EXPORT chunkBg : public chunkItem {
 public:
  int type() override { return CHUNK_BG; };
  
  QColor bg() { return _bg; }
  void setBg (QColor bgc) { _bg = bgc; }
  int length() override { return 0; }
  chunkItem *duplicate() override;
 
  void insertToDocument (QTextCursor &, QTextCharFormat &format) override { setFormat (format, _bg); };

  //painting
//  virtual void paint (QPainter *painter, paintStatus *ps) override;
  
  //output to transcript...
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *ap) override;
  /** output to HTML, suffix can be used to provide closing tags if needed */
  QString toHTML (QString &suffix) override;
  
  static void setFormat (QTextCharFormat &format, QColor color);
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
  int type() override { return CHUNK_ATTRIB; };
  int attrib() { return _attrib; }
  void setAttrib (int a) { _attrib = a; }
  int length() override { return 0; }
  chunkItem *duplicate() override;
 
  //painting
//  virtual void paint (QPainter *painter, paintStatus *ps) override;

  void insertToDocument (QTextCursor &, QTextCharFormat &format) override { setFormat (format, _attrib); };

  //output to transcript...
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *) override;
  
  //no HTML output here - we cannot handle closing tags properly without too much hassle

  static void setFormat (QTextCharFormat &format, int attrib);
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
  int type() override { return CHUNK_LINK; };

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
  
  int length() override { return _text.length(); }
  chunkItem *split (int pos) override;
  chunkItem *duplicate () override;
  void trimLeft () override;
  void replace (int pos, int len, const QString &newtext) override;
 
  const list<menuItem> &menu() { return _menu; };
  //painting
//  virtual void paint (QPainter *painter, paintStatus *ps) override;

  void insertToDocument (QTextCursor &cursor, QTextCharFormat &format) override;

  //output to transcript...
  /** output to plain-text with ANSI sequences */
  QString toAnsi (cANSIParser *ap) override;
  /** output to HTML, suffix can be used to provide closing tags if needed */
  QString toHTML (QString &suffix) override;
 protected:
  QString _name, _target, _text, _hint;
  bool _iscommand, _toprompt, _ismenu;
  
  list<menuItem> _menu;
  
  static QColor linkColor;
};


#endif
