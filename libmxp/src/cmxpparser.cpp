/***************************************************************************
 *   Copyright (C) 2004 by Tomas Mecir                                     *
 *   kmuddy@kmuddy.org                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 ***************************************************************************/

#include "cmxpparser.h"

#include "celementmanager.h"
#include "cmxpstate.h"
#include "cresulthandler.h"

cMXPParser::cMXPParser (cMXPState *st, cElementManager *elm, cResultHandler *res)
{
  state = st;
  elements = elm;
  results = res;
  
  pstate = pText;
  wasBackslashR = false;
}

cMXPParser::~cMXPParser ()
{
}

void cMXPParser::reset ()
{
  str = "";
  pstate = pText;
  wasBackslashR = false;
  chunks.clear();
}

void cMXPParser::parse (const string &text)
{
  //WARNING: examine this function only at your own risk!
  //it is advised to have a look at the simpleParse() function first - it's similar
  //to this one, but much simpler...
  if (text.empty())
    return;
  string::const_iterator it;
  for (it = text.begin(); it != text.end(); ++it)
  {
    char c = *it;
    
// Looks like number of brain-dead servers that send out \n\r is bigger than the
// number of servers that send out \r alone - the latter maybe don't exist at
// all. Hence, with this commented out, we can't handle the \r-only ones,
// but \n\r works.
/*
    //handle \r not followed by \n - treated as a newline
    if (wasBackslashR && (c != '\n'))
    {
      //"str" now certainly is empty, so we needn't care about that
      //report new-line
      elements->gotNewLine();
      state->gotNewLine();
    }
*/
    wasBackslashR = false;
    
    //we need current mode - parsing in LOCKED mode is limited
    //mode is retrieved in every iteration to ensure that it's always up-to-date
    mxpMode mode = state->getMXPMode();
    switch (pstate) {
      case pText: {
        //tags not recognized in LOCKED mode...
        if ((c == '\e') || ((mode != lockedMode) && (c == '<')) || (c == '\n') || (c == '\r'))
        {
          //end of text - got newline / ANSI seq / start of tag
          if (!str.empty())
          {
            state->gotText (str);
            str = "";
          }
          if (c == '\e')
            pstate = pAnsiSeq;
          if ((c == '<') && (mode != lockedMode))
            pstate = pTag;
          if (c == '\n')
          {
            //report new-line
            elements->gotNewLine();
            state->gotNewLine();
          }
          if (c == '\r')
            wasBackslashR = true;
        }
        else
          str += c;  //add new character to the text... 
        break;
      };
      case pAnsiSeq: {
        if ((c == '\e') || (c == '\n') || (c == '\r'))
        {
          //the same as in pTag section...
          results->addToList (results->createError ("Received unfinished ANSI sequence!"));
          str = "";
          if (c == '\e')
            pstate = pAnsiSeq;
          if (c == '\n')
          {
            //report new-line
            elements->gotNewLine();
            state->gotNewLine();
            pstate = pText;
          }
          if (c == '\r')
          {
            pstate = pText;
            wasBackslashR = true;
          }
        }
        else
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
        {
          //ANSI sequence ends...
          if (c == 'z')  //line tag
          {
            if (str.empty())
            {
              //invalid sequence
              str = "\ez";
            }
            else
            {
              //process this sequence
              int len = str.length();
              int num = 0;
              for (int i = 1; i < len; i++)  //str[0] is '[', which is SKIPPED
              {
                char cc = str[i];
                if (cc == ';')  //this shouldn't happen, but some MUD might want to use it...
                {
                  if ((num >= 0) && (num <= 99))  //ensure that number lies in correct range
                  {
                    state->gotLineTag (num);
                    elements->gotLineTag (num);
                  }
                  else
                    results->addToList (results->createError ("Received invalid line tag!"));
                  num = 0;
                }
                else
                  num = num * 10 + (cc - 48);  //48 is the code of '0'
              }
              //report last line tag (and usually the only one)
              if ((num >= 0) && (num <= 99))  //ensure that number lies in correct range
              {
                state->gotLineTag (num);
                elements->gotLineTag (num);
              }
              else
                results->addToList (results->createError ("Received invalid line tag!"));
              str = "";
            }
          }
          else  //something else
          {
            //'\e' and c are not in the string - add them there
            str = '\e' + str + c;
          } 
          pstate = pText;
        }
        else
        if (c == '[')  //this one is valid, but only at the beginning
        {
          if (str.empty())
            str += c;
          else
          {
            //'[' in the middle of ANSI seq => not an ANSI seq...
            pstate = pText;
            str = '\e' + str + c;
          }
        }
        else
        if ((c == ';') || ((c >= '0') && (c <= '9')))  //correct char, unless str is empty
          if (!str.empty())
            str += c;  //here we go...
          else
          {
            //ANSI seq must start with [ - therefore this is not an ANSI sequence after all
            pstate = pText;
            str += '\e';
            str += c;
          }
        else
        //incorrect character...
        {
          str = '\e' + str + c;
          pstate = pText;
        }
        break;
      };
      case pTag: {
        if (c == '>')
        {
          elements->gotTag (str);
          str = "";
          pstate = pText;
        }
        else
        if ((c == '"') || (c == '\''))
        {
          pstate = pQuotedParam;
          quoteChar = c;
          str += c;
        }
        else if ((c == '\e') || (c == '\n') || (c == '\r'))
        {
          //handle incorrectly terminated tag and continue parsing...
          results->addToList (results->createError ("Received unfinished tag <" + str));
          str = "";
          if (c == '\e')
            pstate = pAnsiSeq;
          if (c == '\n')
          {
            //report new-line
            elements->gotNewLine();
            state->gotNewLine();
            pstate = pText;
          }
          if (c == '\r')
          {
            pstate = pText;
            wasBackslashR = true;
          }
        }
        else if (str == "!--")  //comment
        {
          str += c;
          pstate = pComment;
        }
        else
          str += c;
        break;
      };
      case pComment: {
        if (c == '>')
        {
          int l = str.length();
          if ((str[l-2] == '-') && (str[l-1] == '-')) //okay, comment ends
          {
            str = "";
            pstate = pText;
          }
          else
            str += c;
        }
        else if ((c == '\e') || (c == '\n') || (c == '\r'))
        {
          //handle incorrectly terminated comment and continue parsing...
          results->addToList (results->createError ("Received an unfinished comment!"));
          str = "";
          if (c == '\e')
            pstate = pAnsiSeq;
          if (c == '\n')
          {
            //report new-line
            elements->gotNewLine();
            state->gotNewLine();
            pstate = pText;
          }
          if (c == '\r')
          {
            pstate = pText;
            wasBackslashR = true;
          }
        }
        else
          str += c;
        break;
      };
      case pQuotedParam: {
        if (c == quoteChar)
        {
          //quoted parameter ends... this simple approach will work correctly for correct
          //tags, it may treat incorrect quotes as correct, but element manager will take care
          //of that
          pstate = pTag;
          str += c;
        }
        else
        if ((c == '\e') || (c == '\n') || (c == '\r'))
        {
          //the same as in pTag section...
          results->addToList (results->createError ("Received unfinished tag <" + str));
          str = "";
          if (c == '\e')
            pstate = pAnsiSeq;
          if (c == '\n')
          {
            //report new-line
            elements->gotNewLine();
            state->gotNewLine();
            pstate = pText;
          }
          if (c == '\r')
          {
            pstate = pText;
            wasBackslashR = true;
          }
        }
        else
          str += c;
        break;
      };
    };
  }
  //report remaining text, if any (needed to improve speed of text displaying and to handle
  //prompts correctly)
  if ((pstate == pText) && (!str.empty()))
  {
    state->gotText (str);
    str = "";
  }
}

void cMXPParser::simpleParse (const string &text)
//simple parsing - only text and tags - no newlines, no ANSI sequences, no line tags
//simpler version of parse() function above
{
  if (text.empty())
    return;
  chunk ch;
  string::const_iterator it;
  pstate = pText;
  str = "";
  for (it = text.begin(); it != text.end(); ++it)
  {
    char c = *it;
    switch (pstate) {
      case pText: {
        if (c == '<')
        {
          //end of text - got start of tag
          if (!str.empty())
          {
            ch.chk = chunkText;
            ch.text = str;
            chunks.push_back (ch);
            str = "";
          }
          pstate = pTag;
        }
        else
          str += c;  //add new character to the text... 
        break;
      };
      case pTag: {
        if (c == '>')
        {
          ch.chk = chunkTag;
          ch.text = str;
          chunks.push_back (ch);
          str = "";
          pstate = pText;
        }
        else
        if ((c == '"') || (c == '\''))
        {
          pstate = pQuotedParam;
          quoteChar = c;
          str += c;
        }
        else
          str += c;
        break;
      };
      case pQuotedParam: {
        if (c == quoteChar)
        {
          //quoted parameter ends... this simple approach will work correctly for correct
          //tags, it may treat incorrect quotes as correct, but element manager will take care
          //of that
          pstate = pTag;
          str += c;
        }
        else
          str += c;
        break;
      };
    };
  }
  //unfinished things...
  if (pstate == pText)
  {
    ch.chk = chunkText;
    ch.text = str;
    chunks.push_back (ch);
  }
  if ((pstate == pTag) || (pstate == pQuotedParam))
  {
    ch.chk = chunkError;
    ch.text = "Tag definition contains unfinished tag <" + str;
    chunks.push_back (ch);
  }
  str = "";
}

bool cMXPParser::hasNext()
{
  return chunks.empty() ? false : true;
}

chunk cMXPParser::getNext()
{
  if (!hasNext())
  {
    chunk nochunk;
    nochunk.chk = chunkNone;
    return nochunk;
  }
  chunk ch = chunks.front();
  chunks.pop_front();
  return ch;
}
