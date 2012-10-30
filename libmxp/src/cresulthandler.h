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
#ifndef CRESULTHANDLER_H
#define CRESULTHANDLER_H

#include <list>
#include <string>

using namespace std;

#include "libmxp.h"

/**
This class handles a list of results, that are to be sent to the application.

@author Tomas Mecir
*/

class cResultHandler {
public:
  /** constructor */
  cResultHandler ();
  /** destructor */
  ~cResultHandler ();

  /** return next result */
  mxpResult *nextResult ();
  /** do we have any results pending? */
  bool haveResults ();

  /** delete last returned result */
  void deleteReturned ();

  void addToList (mxpResult *res);

  /** reset the class by deleting all pending results */
  void reset ();
  
  //result creation methods coming now
  //the string->char* conversion takes place here

  /** type 0 */
  mxpResult *createNothing ();
  /** type 1 */
  mxpResult *createText (const string &text);
  /** type 2 */
  mxpResult *createLineTag (int tag);
  /** type 3 */
  mxpResult *createFlag (bool begin, const string &flag);
  /** type 4 */
  mxpResult *createVariable (const string &name, const string &value, bool erase = false);
  /** type 5 */
  mxpResult *createFormatting (unsigned char mask, unsigned char attributes, RGB fg, RGB bg,
      const string &font, int size);
  /** type 6 */
  mxpResult *createLink (const string &name, const string &url, const string &text,
      const string &hint);
  /** type 7 */
  mxpResult *createSendLink (const string &name, const string &command, const string &text,
      const string &hint, bool prompt, bool ismenu);
  /** type 8 */
  mxpResult *createExpire (const string &name);
  /** type 9 */
  mxpResult *createSendThis (const string &command);
  /** type 10 */
  mxpResult *createHorizLine ();
  /** type 11 */
  mxpResult *createSound (bool isSOUND, const string &fname, int vol, int count, int priority,
      bool contifrereq, const string &type, const string &url);
  /** type 12 */
  mxpResult *createWindow (const string &name, const string &title, int left, int top,
      int width, int height, bool scrolling, bool floating);
  /** type 13 */
  mxpResult *createInternalWindow (const string &name, const string &title, alignType align,
      bool scrolling);
  /** type 14 */
  mxpResult *createCloseWindow (const string &name);
  /** type 15 */
  mxpResult *createSetWindow (const string &name);
  /** type 16 */
  mxpResult *createMoveCursor (int x, int y);
  /** type 17 */
  mxpResult *createEraseText (bool restofframe);
  /** type 18 */
  mxpResult *createRelocate (const string &server, int port);
  /** type 19 */
  mxpResult *createSendLogin (bool username);
  /** type 20 */
  mxpResult *createImage (const string &fname, const string &url, const string &type, int height,
      int width, int hspace, int vspace, alignType align);
  /** type 21 */
  mxpResult *createImageMap (const string &name);
  /** type 22 */
  mxpResult *createGauge (const string &variable, const string &maxvariable,
      const string &caption, RGB color);
  /** type 23 */
  mxpResult *createStat (const string &variable, const string &maxvariable,
      const string &caption);

  /** type -1 */
  mxpResult *createError (const string &error);
  /** type -2 */
  mxpResult *createWarning (const string &warning);
  
  /** delete this result */
  void deleteResult (mxpResult *res);
protected:

  /** result that was most recently sent to the app */
  mxpResult *returnedResult;
  list<mxpResult *> results;
};

#endif
