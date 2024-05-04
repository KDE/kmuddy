//
// C++ Implementation: cpattern
//
// Description: Pattern matching.
//
/*
Copyright 2007-2011 Tomas Mecir <kmuddy@kmuddy.com>

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

#include "cpattern.h"

#include <QRegularExpression>

struct cPattern::Private {
  /** pattern */
  QString pattern;
  /** matching type */
  cPattern::PatternType type;
  /** case-sensitive comparison */
  Qt::CaseSensitivity cs;
  /** whole words only */
  bool wholewords;

  /** last text that has been successfully compared */
  QString lasttext;
  /** prefix/suffix parts of that text (before/after matched part) */
  QString prefix, suffix;
  /** position/length of matching part */
  int lastpos, lastlen;
  /** list of back-references (if doing regexp compare) */
  QStringList backreflist;
  /** positions of backreferences */
  int *backrefpos;

  /** creating regexp objects on each trigger matching is SLOW - pre-parse them */
  QRegularExpression regexp;
};

cPattern::cPattern (const QString &pattern, PatternType pt)
{
  d = new Private;

  d->cs = Qt::CaseSensitive;
  d->wholewords = true;
  d->regexp.setCaseSensitivity (d->cs);
  d->backrefpos = nullptr;
  d->lastlen = d->lastpos = 0;

  d->pattern = pattern;
  d->type = pt;
  if (d->type == regexp)
    d->regexp.setPattern (pattern);
}

cPattern::~cPattern ()
{
  if (d->backrefpos != nullptr)
    delete[] d->backrefpos;
  delete d;
}

void cPattern::setPattern (const QString &pattern)
{
  d->pattern = pattern;
  if (d->type == regexp)
    d->regexp.setPattern (pattern);
}

QString cPattern::pattern () const
{
  return d->pattern;
}

void cPattern::setMatching (PatternType pt) {
  d->type = pt;
  if (d->type == regexp)
    d->regexp.setPattern (d->pattern);
}

cPattern::PatternType cPattern::matching () const
{
  return d->type;
}

void cPattern::setCaseSensitive (bool cs)
{
  d->cs = cs ? Qt::CaseSensitive : Qt::CaseInsensitive;
  d->regexp.setCaseSensitivity (d->cs);
}

bool cPattern::caseSensitive () const
{
  return d->cs;
}

void cPattern::setWholeWords (bool ww)
{
  d->wholewords = ww;
}

bool cPattern::wholeWords () const
{
  return d->wholewords;
}

bool cPattern::match (const QString &text, int matchingPos)
{
  //do NOTHING if my d->pattern has zero length...
  if (d->pattern.length() == 0)
    return false;

  if (d->backrefpos != nullptr)
    delete[] d->backrefpos;
  d->backrefpos = nullptr;

  bool matched = false;
  int n;
  
  switch (d->type) {
    case exact:
      if (matchingPos != 0)
        matched = false;
      else if (d->cs)
        matched = (text == d->pattern);
      else
        matched = (text.toLower() == d->pattern.toLower());
      if (matched)
      {
        d->prefix = d->suffix = "";
        d->lastpos = 0;
        d->lastlen = d->pattern.length ();
      }
      break;
    case substring:
      n = text.indexOf (d->pattern, matchingPos, d->cs);
      matched = (n != -1);
      if (matched)
      {
        d->prefix = text.left (n);
        d->suffix = text.right (text.length() - (n + d->pattern.length()));
        d->lastpos = n;
        d->lastlen = d->pattern.length ();
      }
      break;
    case begin:
      if (matchingPos != 0)
        matched = false;
      else if (d->cs)
        matched = text.startsWith (d->pattern);
      else
        matched = text.toLower().startsWith (d->pattern.toLower());
      if (matched)
      {
        d->prefix = "";
        d->suffix = text.right (text.length() - d->pattern.length());
        d->lastpos = 0;
        d->lastlen = d->pattern.length ();
      }
      break;
    case end:
      if (matchingPos != 0)
        matched = false;
      else if (d->cs)
        matched = text.endsWith (d->pattern);
      else
        matched = text.toLower().endsWith (d->pattern.toLower());
      if (matched)
      {
        d->prefix = text.left (text.length() - d->pattern.length());
        d->suffix = "";
        d->lastpos = text.length() - d->pattern.length();
        d->lastlen = d->pattern.length ();
      }
      break;
    case regexp:
      //regexp's case-sensitivity is set in constructor and in function
      // setCaseSensitive
      n = d->regexp.indexIn (text, matchingPos);
      if (n != -1)    //MATCH!
      {
        matched = true;
        d->lastpos = n;
        d->lastlen = d->regexp.matchedLength();
        d->prefix = text.left (n);
        d->suffix = text.right (text.length() - (n + d->lastlen));
        d->backreflist.clear ();
        d->backreflist = d->regexp.capturedTexts ();
        //positions of back-references
        int npos = d->backreflist.count();
        d->backrefpos = new int[npos];
        for (int i = 0; i < npos; i++)
          d->backrefpos[i] = d->regexp.pos (i);
      }
      break;
  };

  // whole words only ?
  if (matched && d->wholewords) {
    int len = d->prefix.length();
    if ((len > 0) && (!d->prefix[len-1].isSpace ()))
      matched = false;
    len = d->suffix.length();
    if ((len > 0) && (!d->suffix[0].isSpace ()))
      matched = false;
  }

  // return the result...
  if (matched)
  {
    d->lasttext = text;
    return true;
  }
  else
    return false;
}

QString cPattern::getLastText () const
{
  return d->lasttext;
}

QString cPattern::getPrefix () const
{
  return d->prefix;
}

QString cPattern::getSuffix () const
{
  return d->suffix;
}

int cPattern::getLastPos () const
{
  return d->lastpos;
}

int cPattern::getLastLength () const
{
  return d->lastlen;
}

QStringList cPattern::getBackRefList () const
{
  return d->backreflist;
}

int cPattern::getBackRefPos (int which) const
{
  if ((which < 0) || (which >= d->backreflist.count())) return -1;
  return d->backrefpos[which];
}

void cPattern::variablePosition (const QString &varname, int *start, int *len)
{
  if ((!start) || (!len)) return;  // sanity check

  // by default, report that we found nothing
  *start = -1;
  *len = 0;

  bool ok;
  int number = varname.toInt (&ok);
  if (ok)   //it was a number
  {
    // must be a valid backref
    if (d->type != regexp) return;
    if (number >= (int) d->backreflist.count()) return;
    *start = getBackRefPos (number);
    *len = d->backreflist[number].length();
    return;
  }
  // prefixtrim and suffixtrim exist for compatibility with KMuddy <= 0.8
  // TODO: remove them sometimes after 1.0
  if ((varname == "prefix") || (varname == "prefixtrim")) {
    *start = 0;
    // locate first non-whitespace
    int length = d->prefix.length();
    while ((*start < length) && d->prefix[*start].isSpace())
      (*start)++;
    *len = d->prefix.trimmed().length();
  } else if ((varname == "suffix") || (varname == "suffixtrim")) {
    // locate first non-whitespace
    int end;
    for (end = d->suffix.length() - 1; end >= 0; --end)
      if (!d->suffix[end].isSpace()) break;
    *len = d->suffix.trimmed ().length();
    *start = end + 1 - *len;
  } else if (varname == "prefixfull") {
    *start = 0;
    *len = d->prefix.length();
  } else if (varname == "suffixfull") {
    *len = d->suffix.length();
    *start = d->lasttext.length() - *len;
  } else if (varname == "matched") {
    *start = d->lastpos;
    *len = d->lastlen;
  } else if (varname == "line") {
    *start = 0;
    *len = d->lasttext.length();
  }
}

QString cPattern::getVariable (const QString &varname, const QString &def) const
{
  //look if we know the name; return the real string if we do...
  QString result;
  bool ok;
  int number = varname.toInt (&ok);
  if (ok)   //it was a number
  {
    if (d->type == regexp)
    {
      if (number >= (int) d->backreflist.count())
        result = QString();
      else
        result = d->backreflist[number];
    }
    else
      result = def;
  }
  else
  {
    // prefixtrim and suffixtrim exist for compatibility with KMuddy <= 0.8
    // TODO: remove them sometimes after 1.0
    if ((varname == "prefix") || (varname == "prefixtrim"))
      result = d->prefix.trimmed ();
    else if ((varname == "suffix") || (varname == "suffixtrim"))
      result = d->suffix.trimmed ();
    else if (varname == "prefixfull")
      result = d->prefix;
    else if (varname == "suffixfull")
      result = d->suffix;
    else if (varname == "matched")
      result = d->lasttext.mid (d->lastpos, d->lastlen);
    else if (varname == "line")
      result = d->lasttext;
    else
      result = def;
  }
  return result;
}

// Pseudo-variables could in theory be done using local variables in a command queue,
// the problem is that aliases need them too, thus we cannot expand like that ...
// Hence we need to have this thingie ...
void cPattern::expandPseudoVariables (QString &string) const
{
  QString newstring = "";
  int len = string.length ();
  bool invar = false;
  bool inpar = false; //variable name is in parentheses '(' and ')'
  QString varname;
  for (int i = 0; i < len; i++)
  {
    QChar ch = string[i];

    //support things like $$$a correctly (when mixing pseudo and normal vars)
    if (invar && (!inpar) && (ch == '$') && (varname.isEmpty()))
    {
      invar = false;
      newstring += ch;
    }

    if (!invar)
    {
      if (ch == '$')
      {
        invar = true;
        inpar = false;
        varname = "";
      }
      else
        newstring += ch;
    }
    else
    {
      if ((varname == "") && (ch == '(') && (!inpar))
        inpar = true;
      else
        if (!(ch.isLetterOrNumber ()))  //end of pseudo-variable name
      {
        invar = false;

        if (inpar)
        {
          inpar = false;
          if (ch == ')')
            newstring += getVariable (varname, "$(" + varname + ")");
          else
            newstring += "$(" + varname + ((ch == '$') ? QString() : QString(ch));
        }
        else
        {
          newstring += getVariable (varname, "$" + varname);
          if (ch != '$')
            newstring += ch;
        }
        if (ch == '$')  //new variable follows immediately
        {
          invar = true;
          varname = "";
        }
      }
      else
        varname += ch;
    }
  }

  if (invar) {    //if a variable ends the string
    if (inpar)
      newstring += "$(" + varname;  //no ending parenthesis - no variable
    else
      newstring += getVariable (varname, "$"+varname);
  }

  //apply changes!
  string = newstring;
}

QMap<QString, QVariant> cPattern::scriptVariables ()
{
  QMap<QString, QVariant> res;
  res["prefix"] = getVariable("prefix");
  res["prefixfull"] = getVariable("prefixfull");
  res["suffix"] = getVariable("suffix");
  res["suffixfull"] = getVariable("suffixfull");
  res["matched"] = getVariable("matched");
  res["line"] = getVariable("line");
  if (d->type == regexp) res["matches"] = d->backreflist;
  return res;
}

