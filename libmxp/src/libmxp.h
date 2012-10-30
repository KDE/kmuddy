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

#ifndef _LIBMXP_H
#define _LIBMXP_H


#ifdef __cplusplus
extern "C" {
#endif

enum attribs {
  Bold = 0x01,
  Italic = 0x02,
  Underline = 0x04,
  Strikeout = 0x08
};

typedef struct {
  unsigned char r, g, b;
} RGB;

typedef struct {
  int type;          //result type
  void *data;        //result data, contents depend on result type
} mxpResult;

/** PUBLIC API for the library */

typedef void * MXPHANDLER;

/** create a new MXP handler */
MXPHANDLER mxpCreateHandler ();
/** destroy a previously created MXP handler */
void mxpDestroyHandler (MXPHANDLER handler);

/** process some text with the handler */
void mxpProcessText (MXPHANDLER handler, const char *text);
/** fetch next result of the processing */
mxpResult *mxpNextResult (MXPHANDLER handler);
/** are there any more results to fetch? */
char mxpHasResults (MXPHANDLER handler);

/** switch to OPEN mode - use this immediately after creating this object, in case that
you don't want to be in LOCKED mode by default (where MXP is only used if some line tag
is received) */
void mxpSwitchToOpen (MXPHANDLER handler);

/** set default text attributes */
void mxpSetDefaultText (MXPHANDLER handler, const char *font, int size, char _bold,
    char _italic, char _underline, char _strikeout, RGB fg, RGB bg);
/** set attributes of header 1-6 (for tags <h1>-<h6> */
void mxpSetHeaderParams (MXPHANDLER handler, int which, const char *font, int size, char _bold,
    char _italic, char _underline, char _strikeout, RGB fg, RGB bg);
/** set default color for gauges */
void mxpSetDefaultGaugeColor (MXPHANDLER handler, RGB color);
/** set used non-proportional font */
void mxpSetNonProportFont (MXPHANDLER handler, const char *font);
/** set client name and version reported to the MUD */
void mxpSetClient (MXPHANDLER handler, const char *name, const char *version);
/* screen, window and font size, used by FRAME and IMAGE tags */
void mxpSetScreenProps (MXPHANDLER handler, int sx, int sy, int wx, int wy, int fx, int fy);

void mxpSupportsLink (MXPHANDLER handler, char supports);
void mxpSupportsGauge (MXPHANDLER handler, char supports);
void mxpSupportsStatus (MXPHANDLER handler, char supports);
void mxpSupportsSound (MXPHANDLER handler, char supports);
void mxpSupportsFrame (MXPHANDLER handler, char supports);
void mxpSupportsImage (MXPHANDLER handler, char supports);
void mxpSupportsRelocate (MXPHANDLER handler, char supports);


/**
MXP result types:
TODO: write the result type documentation.
*/

//for type 3
struct flagStruct {
  char begin;
  char *name;
};

//for type 4
struct varStruct {
  char *name, *value;
  char erase;
};

#define USE_BOLD 0x01
#define USE_ITALICS 0x02
#define USE_UNDERLINE 0x04
#define USE_STRIKEOUT 0x08
#define USE_FG 0x10
#define USE_BG 0x20
#define USE_FONT 0x40
#define USE_SIZE 0x80
#define USE_ALL 0xFF

//for type 5
struct formatStruct {
  unsigned char usemask;    //8-bit; which params should be applied
  unsigned char attributes;
  RGB fg, bg;
  char *font; //if NULL and it should be applied => default font should be set
  int size;
};

//for type 6
struct linkStruct {
  char *name, *url, *text, *hint;
};

//for type 7
struct sendStruct {
  char *name, *command, *text, *hint;
  char toprompt, ismenu;
};

//for type 11; also see MSP protocol
struct soundStruct {
  char isSOUND;         //1 if SOUND, 0 if MUSIC
  char *fname, *url;    //(fName and U params)
  int vol;              //volume (V param)
  int repeats;          //-1 for infinite (L param)
  int priority;         //0-100; SOUND only (P param)
  char continuemusic;   //continue without restarting if rerequested? MUSIC only (C param)
  char *type;           //sound/music type (T param)
};

//for type 12
struct windowStruct {
  char *name, *title;
  int left, top, width, height;
  char scrolling, floating;
};

//align type for internal windows and images (type Middle is only valid for images)
enum alignType {
  Left = 1,
  Right,
  Bottom,
  Top,
  Middle
};

//for type 13
struct internalWindowStruct {
  char *name, *title;
  enum alignType align;
  char scrolling;
};

//for type 16
struct moveStruct {
  int x, y;
};

//for type 18
struct relocateStruct {
  char *server;
  int port;
};

//for type 20
struct imageStruct {
  char *fname, *url, *type;
  int height, width, hspace, vspace;
  enum alignType align;
};

//for type 22
struct gaugeStruct {
  char *variable, *maxvariable, *caption;
  RGB color;
};

//for type 23
struct statStruct {
  char *variable, *maxvariable, *caption;
};


#ifdef __cplusplus
//end of extern "C"
};
#endif

#endif  //_LIBMXP_H
