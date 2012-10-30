/***************************************************************************
 *   Copyright (C) 2004 by Tomas Mecir                                     *
 *   kmuddy@kmuddy.org                                                     *
 *                                                                         *
 *   This program is free software you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 ***************************************************************************/

#include "cresulthandler.h"

#include <string.h>

cResultHandler::cResultHandler ()
{
  returnedResult = 0;
}


cResultHandler::~cResultHandler ()
{
  reset ();
}

void cResultHandler::reset ()
{
  deleteReturned ();
  list<mxpResult *>::iterator it;
  for (it = results.begin(); it != results.end(); ++it)
    deleteResult (*it);
  results.clear ();
}

mxpResult *cResultHandler::nextResult ()
{
  if (returnedResult)
    deleteReturned ();
  returnedResult = results.front ();
  results.pop_front ();
  return returnedResult;
}

bool cResultHandler::haveResults ()
{
  return results.empty() ? false : true;
}

void cResultHandler::deleteReturned ()
{
  if (returnedResult)
    deleteResult (returnedResult);
  returnedResult = 0;
}


//result creation methods coming now
//the string->char* conversion takes place here

/** type 0 */
mxpResult *cResultHandler::createNothing ()
{
  mxpResult *res = new mxpResult;
  res->type = 0;
  res->data = 0;

  return res;
}

/** type 1 */
mxpResult *cResultHandler::createText (const string &text)
{
  mxpResult *res = new mxpResult;
  res->type = 1;
  char *str = 0;
  if (!text.empty())
  {
    str = new char[text.length() + 1];
    strcpy (str, text.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type 2 */
mxpResult *cResultHandler::createLineTag (int tag)
{
  mxpResult *res = new mxpResult;
  res->type = 2;
  int *tg = new int;
  *tg = tag;

  res->data = (void *) tg;

  return res;
}

/** type 3 */
mxpResult *cResultHandler::createFlag (bool begin, const string &flag)
{
  mxpResult *res = new mxpResult;
  res->type = 3;
  flagStruct *fs = new flagStruct;
  fs->begin = begin;
  fs->name = 0;
  if (!flag.empty())
  {
    fs->name = new char[flag.length() + 1];
    strcpy (fs->name, flag.c_str());
  }

  res->data = (void *) fs;

  return res;
}

/** type 4 */
mxpResult *cResultHandler::createVariable (const string &name, const string &value, bool erase)
{
  mxpResult *res = new mxpResult;
  res->type = 4;
  varStruct *vs = new varStruct;
  vs->name = 0;
  if (!name.empty())
  {
    vs->name = new char[name.length() + 1];
    strcpy (vs->name, name.c_str());
  }
  vs->value = 0;
  if (!value.empty())
  {
    vs->value = new char[value.length() + 1];
    strcpy (vs->value, value.c_str());
  }
  vs->erase = erase;

  res->data = (void *) vs;

  return res;
}

/** type 5 */
mxpResult *cResultHandler::createFormatting (unsigned char mask, unsigned char attributes,
    RGB fg, RGB bg, const string &font, int size)
{
  mxpResult *res = new mxpResult;
  res->type = 5;
  formatStruct *fs = new formatStruct;
  fs->usemask = mask;
  fs->attributes = attributes;
  fs->fg = fg;
  fs->bg = bg;
  fs->size = size;
  fs->font = 0;
  if (!font.empty())
  {
    fs->font = new char[font.length() + 1];
    strcpy (fs->font, font.c_str());
  }

  res->data = (void *) fs;

  return res;
}

/** type 6 */
mxpResult *cResultHandler::createLink (const string &name, const string &url, const string &text,
    const string &hint)
{
  mxpResult *res = new mxpResult;
  res->type = 6;
  linkStruct *ls = new linkStruct;
  ls->name = ls->hint = ls->text = ls->url = 0;
  if (!name.empty())
  {
    ls->name = new char[name.length() + 1];
    strcpy (ls->name, name.c_str());
  }
  if (!hint.empty())
  {
    ls->hint = new char[hint.length() + 1];
    strcpy (ls->hint, hint.c_str());
  }
  if (!text.empty())
  {
    ls->text = new char[text.length() + 1];
    strcpy (ls->text, text.c_str());
  }
  if (!url.empty())
  {
    ls->url = new char[url.length() + 1];
    strcpy (ls->url, url.c_str());
  }

  res->data = (void *) ls;

  return res;
}

/** type 7 */
mxpResult *cResultHandler::createSendLink (const string &name, const string &command,
    const string &text, const string &hint, bool prompt, bool ismenu)
{
  mxpResult *res = new mxpResult;
  res->type = 7;
  sendStruct *ss = new sendStruct;
  ss->name = ss->command = ss->hint = ss->text = 0;
  if (!name.empty())
  {
    ss->name = new char[name.length() + 1];
    strcpy (ss->name, name.c_str());
  }
  if (!command.empty())
  {
    ss->command = new char[command.length() + 1];
    strcpy (ss->command, command.c_str());
  }
  if (!hint.empty())
  {
    ss->hint = new char[hint.length() + 1];
    strcpy (ss->hint, hint.c_str());
  }
  if (!text.empty())
  {
    ss->text = new char[text.length() + 1];
    strcpy (ss->text, text.c_str());
  }
  ss->toprompt = prompt;
  ss->ismenu = ismenu;

  res->data = (void *) ss;

  return res;
}

/** type 8 */
mxpResult *cResultHandler::createExpire (const string &name)
{
  mxpResult *res = new mxpResult;
  res->type = 8;
  char *str = 0;
  if (!name.empty())
  {
    str = new char[name.length() + 1];
    strcpy (str, name.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type 9 */
mxpResult *cResultHandler::createSendThis (const string &command)
{
  mxpResult *res = new mxpResult;
  res->type = 9;
  char *str = 0;
  if (!command.empty())
  {
    str = new char[command.length() + 1];
    strcpy (str, command.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type 10 */
mxpResult *cResultHandler::createHorizLine ()
{
  mxpResult *res = new mxpResult;
  res->type = 10;
  res->data = 0;

  return res;
}

/** type 11 */
mxpResult *cResultHandler::createSound (bool isSOUND, const string &fname, int vol, int count,
    int priority, bool contifrereq, const string &type, const string &url)
{
  mxpResult *res = new mxpResult;
  res->type = 11;
  soundStruct *ss = new soundStruct;
  ss->fname = ss->type = ss->url = 0;
  if (!fname.empty())
  {
    ss->fname = new char[fname.length() + 1];
    strcpy (ss->fname, fname.c_str());
  }
  if (!type.empty())
  {
    ss->type = new char[type.length() + 1];
    strcpy (ss->type, type.c_str());
  }
  if (!url.empty())
  {
    ss->url = new char[url.length() + 1];
    strcpy (ss->url, url.c_str());
  }
  ss->isSOUND = isSOUND;
  ss->vol = vol;
  ss->repeats = count;
  ss->priority = priority;
  ss->continuemusic = contifrereq;

  res->data = (void *) ss;

  return res;
}

/** type 12 */
mxpResult *cResultHandler::createWindow (const string &name, const string &title,
    int left, int top, int width, int height, bool scrolling, bool floating)
{
  mxpResult *res = new mxpResult;
  res->type = 12;
  windowStruct *ws = new windowStruct;
  ws->name = ws->title = 0;
  if (!name.empty())
  {
    ws->name = new char[name.length() + 1];
    strcpy (ws->name, name.c_str());
  }
  if (!title.empty())
  {
    ws->title = new char[title.length() + 1];
    strcpy (ws->title, title.c_str());
  }
  ws->left = left;
  ws->top = top;
  ws->width = width;
  ws->height = height;
  ws->scrolling = scrolling;
  ws->floating = floating;

  res->data = (void *) ws;

  return res;
}

/** type 13 */
mxpResult *cResultHandler::createInternalWindow (const string &name, const string &title,
    alignType align, bool scrolling)
{
  mxpResult *res = new mxpResult;
  res->type = 13;
  internalWindowStruct *ws = new internalWindowStruct;
  ws->name = ws->title = 0;
  if (!name.empty())
  {
    ws->name = new char[name.length() + 1];
    strcpy (ws->name, name.c_str());
  }
  if (!title.empty())
  {
    ws->title = new char[title.length() + 1];
    strcpy (ws->title, title.c_str());
  }
  ws->align = align;
  ws->scrolling = scrolling;

  res->data = (void *) ws;

  return res;
}

/** type 14 */
mxpResult *cResultHandler::createCloseWindow (const string &name)
{
  mxpResult *res = new mxpResult;
  res->type = 14;
  char *str = 0;
  if (!name.empty())
  {
    str = new char[name.length() + 1];
    strcpy (str, name.c_str());
  }
  res->data = (void *) str;

  return res;
}

/** type 15 */
mxpResult *cResultHandler::createSetWindow (const string &name)
{
  mxpResult *res = new mxpResult;
  res->type = 15;
  char *str = 0;
  if (!name.empty())
  {
    str = new char[name.length() + 1];
    strcpy (str, name.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type 16 */
mxpResult *cResultHandler::createMoveCursor (int x, int y)
{
  mxpResult *res = new mxpResult;
  res->type = 16;
  moveStruct *ms = new moveStruct;
  ms->x = x;
  ms->y = y;

  res->data = (void *) ms;

  return res;
}

/** type 17 */
mxpResult *cResultHandler::createEraseText (bool restofframe)
{
  mxpResult *res = new mxpResult;
  res->type = 17;
  res->data = (void *) (restofframe ? 1 : 0);

  return res;
}

/** type 18 */
mxpResult *cResultHandler::createRelocate (const string &server, int port)
{
  mxpResult *res = new mxpResult;
  res->type = 18;
  relocateStruct *rs = new relocateStruct;
  rs->server = 0;
  if (!server.empty())
  {
    rs->server = new char[server.length() + 1];
    strcpy (rs->server, server.c_str());
  }
  rs->port = port;

  res->data = (void *) rs;

  return res;
}

/** type 19 */
mxpResult *cResultHandler::createSendLogin (bool username)
{
  mxpResult *res = new mxpResult;
  res->type = 19;
  res->data = (void *) (username ? 1 : 0);

  return res;
}

/** type 20 */
mxpResult *cResultHandler::createImage (const string &fname, const string &url, const string &type,
    int height, int width, int hspace, int vspace, alignType align)
{
  mxpResult *res = new mxpResult;
  res->type = 20;
  imageStruct *is = new imageStruct;
  is->fname = is->url = is->type = 0;
  if (!fname.empty())
  {
    is->fname = new char[fname.length() + 1];
    strcpy (is->fname, fname.c_str());
  }
  if (!url.empty())
  {
    is->url = new char[url.length() + 1];
    strcpy (is->url, url.c_str());
  }
  if (!type.empty())
  {
    is->type = new char[type.length() + 1];
    strcpy (is->type, type.c_str());
  }
  is->height = height;
  is->width = width;
  is->hspace = hspace;
  is->vspace = vspace;
  is->align = align;

  res->data = (void *) is;

  return res;
}

/** type 21 */
mxpResult *cResultHandler::createImageMap (const string &name)
{
  mxpResult *res = new mxpResult;
  res->type = 21;
  char *str = 0;
  if (!name.empty())
  {
    str = new char[name.length() + 1];
    strcpy (str, name.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type 22 */
mxpResult *cResultHandler::createGauge (const string &variable, const string &maxvariable,
    const string &caption, RGB color)
{
  mxpResult *res = new mxpResult;
  res->type = 22;
  gaugeStruct *gs = new gaugeStruct;
  gs->variable = gs->maxvariable = gs->caption = 0;
  if (!variable.empty())
  {
    gs->variable = new char[variable.length() + 1];
    strcpy (gs->variable, variable.c_str());
  }
  if (!maxvariable.empty())
  {
    gs->maxvariable = new char[maxvariable.length() + 1];
    strcpy (gs->maxvariable, maxvariable.c_str());
  }
  if (!caption.empty())
  {
    gs->caption = new char[caption.length() + 1];
    strcpy (gs->caption, caption.c_str());
  }
  gs->color = color;

  res->data = (void *) gs;

  return res;
}

/** type 23 */
mxpResult *cResultHandler::createStat (const string &variable, const string &maxvariable,
    const string &caption)
{
  mxpResult *res = new mxpResult;
  res->type = 23;
  statStruct *ss = new statStruct;
  ss->variable = ss->maxvariable = ss->caption = 0;
  if (!variable.empty())
  {
    ss->variable = new char[variable.length() + 1];
    strcpy (ss->variable, variable.c_str());
  }
  if (!maxvariable.empty())
  {
    ss->maxvariable = new char[maxvariable.length() + 1];
    strcpy (ss->maxvariable, maxvariable.c_str());
  }
  if (!caption.empty())
  {
    ss->caption = new char[caption.length() + 1];
    strcpy (ss->caption, caption.c_str());
  }

  res->data = (void *) ss;

  return res;
}


/** type -1 */
mxpResult *cResultHandler::createError (const string &error)
{
  mxpResult *res = new mxpResult;
  res->type = -1;
  char *str = 0;
  if (!error.empty())
  {
    str = new char[error.length() + 1];
    strcpy (str, error.c_str());
  }

  res->data = (void *) str;

  return res;
}

/** type -2 */
mxpResult *cResultHandler::createWarning (const string &warning)
{
  mxpResult *res = new mxpResult;
  res->type = -2;
  char *str = 0;
  if (!warning.empty())
  {
    str = new char[warning.length() + 1];
    strcpy (str, warning.c_str());
  }

  res->data = (void *) str;

  return res;
}


void cResultHandler::addToList (mxpResult *res)
{
  if (res)
    results.push_back (res);
}

/** delete this result */
void cResultHandler::deleteResult (mxpResult *res)
{
  if (!res)
    return;

  switch (res->type) {
    case 1:
    case 8:
    case 9:
    case 14:
    case 15:
    case 21:
    case -1:
    case -2:
      delete[] (char *) res->data;
      break;
    case 2:
      delete[] (int *) res->data;
      break;
    case 3: {
      flagStruct *fs = (flagStruct *) res->data;
      delete[] fs->name;
      delete fs;
      break;
    }
    case 4: {
      varStruct *vs = (varStruct *) res->data;
      delete[] vs->name;
      delete[] vs->value;
      delete vs;
      break;
    }
    case 5: {
      formatStruct *fs = (formatStruct *) res->data;
      delete[] fs->font;
      delete fs;
      break;
    }
    case 6: {
      linkStruct *ls = (linkStruct *) res->data;
      delete[] ls->name;
      delete[] ls->hint;
      delete[] ls->text;
      delete[] ls->url;
      delete ls;
      break;
    }
    case 7: {
      sendStruct *ss = (sendStruct *) res->data;
      delete[] ss->name;
      delete[] ss->command;
      delete[] ss->hint;
      delete[] ss->text;
      delete ss;
      break;
    }
    //case 10: NOTHING
    case 11: {
      soundStruct *ss = (soundStruct *) res->data;
      delete[] ss->fname;
      delete[] ss->type;
      delete[] ss->url;
      delete ss;
      break;
    }
    case 12: {
      windowStruct *ws = (windowStruct *) res->data;
      delete[] ws->name;
      delete[] ws->title;
      delete ws;
      break;
    }
    case 13: {
      internalWindowStruct *iws = (internalWindowStruct *) res->data;
      delete[] iws->name;
      delete[] iws->title;
      delete iws;
      break;
    }
    case 16: {
      moveStruct *ms = (moveStruct *) res->data;
      delete ms;
      break;
    }
    //case 17: NOTHING
    case 18: {
      relocateStruct *rs = (relocateStruct *) res->data;
      delete[] rs->server;
      delete rs;
      break;
    }
    //case 19: NOTHING
    case 20: {
      imageStruct *is = (imageStruct *) res->data;
      delete[] is->fname;
      delete[] is->url;
      delete[] is->type;
      delete is;
      break;
    }
    case 22: {
      gaugeStruct *gs = (gaugeStruct *) res->data;
      delete[] gs->variable;
      delete[] gs->maxvariable;
      delete[] gs->caption;
      delete gs;
      break;
    };
    case 23: {
      statStruct *ss = (statStruct *) res->data;
      delete[] ss->variable;
      delete[] ss->maxvariable;
      delete[] ss->caption;
      delete ss;
      break;
    };
  };
  delete res;
}
