/***************************************************************************
                          cmccp.cpp  -  MCCP support
                             -------------------
    begin                : Pi feb 14 2003
    copyright            : (C) 2003 by Tomas Mecir
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

#include "cmccp.h"
#include "ctelnet.h"

cMCCP::cMCCP (cTelnet *_telnet)
{
  telnet = _telnet;
  instream = false;
  reset ();
}

cMCCP::~cMCCP()
{
  //nothing here
}

/*
These two functions could be causing problems, if they disabled
decompression while the server was in the
middle of a compressed stream. But unless the server is buggy, this can
NOT happen (if we're reading this seq., then compressed data can not occur
AFTER that sequence, but everything before it has already been decompressed)
*/

void cMCCP::setMCCP1 (bool val)
{
  mccp1 = val;
  mccp2 = false;
  usemccp = val;
}

void cMCCP::setMCCP2 (bool val)
{
  mccp1 = false;
  mccp2 = val;
  usemccp = val;
}

void cMCCP::reset ()
{
  if (instream)
    inflateEnd (&stream);
  mccp1 = mccp2 = usemccp = false;
  instream = false;
  state = 0;
  stream.total_in = 0;
  stream.total_out = 0;
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  compressed = 0;
  decompressed = 0;
}

void cMCCP::prepareDecompression (char *_inBuf, char *_outBuf, int inSize, int buflen)
{
  inBuf = _inBuf;
  outBuf = _outBuf;
  len = buflen;

  stream.next_in = (Bytef *) _inBuf;
  stream.avail_in = inSize;
  stream.next_out = (Bytef *) _outBuf;
  stream.avail_out = buflen;

  //New data received, if we don't use compression, then this will be equal
  //to "decompressed" after running uncompressNext().
  compressed += inSize;
}

int cMCCP::uncompressNext ()
{
  int outSize = doUncompressNext ();
  if (outSize >= 0)
    decompressed += outSize;
  return outSize;
}

int cMCCP::doUncompressNext ()
//some telnet parsing occurs here, but only for compressed sequences
//everything else is handled by cTelnet class
//caveat: this function is a bit messy :-(
{
  int outSize = 0;

  //Only if MCCP is enabled... MCCP must be negotiated first, so we can
  //assume that if MCCP is not enabled before parsing the stream, no
  //MCCP sequences are in that stream (it could include IAC DO COMPRESS or
  //COMPRESS2 though).

  if (!usemccp)
  {
    //if MCCP is off, we simply copy input to output...
    if (stream.avail_in == 0)
      return -1;    //nothing more to read
    for (unsigned int i = 0; i < stream.avail_in; i++)
      *(stream.next_out++) = *(stream.next_in++);
    outSize = stream.avail_in;
    stream.avail_in = 0;
    return outSize;
  }

  // MCCP is used
  //mark output buffer as empty
  stream.next_out = (Bytef *) outBuf;
  stream.avail_out = len;

  //no more input?
  if (stream.avail_in == 0)
    return -1;

  //there are two basic modes - 1. not in stream, 2. in stream

  if (instream)
  {
    //ask zlib to decompress the stream
    int out = stream.avail_out;
    int zval = inflate (&stream, Z_SYNC_FLUSH);
    outSize = out - stream.avail_out;

    //if zlib says it's done, we also proceed with the next uncompressed
    //section...
    if (zval == Z_STREAM_END)
    {
      inflateEnd (&stream);
      instream = false;
      return outSize;
    }
    else
    {
      if (zval < 0)   //ERROR!!!
      {
        //TODO: implement this!!! currently we silently ignore this!
      }
      return outSize;
    }
  }
  // Still in the stream ? Return.
  //I don't use "else" so I can read both compressed and uncompressed portion
  //of the string if we have these.
  if (instream) return outSize;

  // We are not in a stream

  //we copy inBuf to outBuf byte by byte, looking if we don't find
  //the begin-compressed-stream sequence
  //the sequence looks like this:
  //IAC SB COMPRESS WILL SE for MCCP v1 (invalid telnet sequence)
  //IAC SB COMPRESS2 IAC SE for MCCP v2

  //there are some caveats: IAC IAC must not be interpreted as IAC
  //after IAC SB anything-but-COMPRESS-or-COMPRESS2 we mustn't parse
  //anything until IAC SE is received

  while (stream.avail_in > 0)
  {
    if (state < 6)
    {
      unsigned char letter = 0;
      switch (state) {
        case 0: letter = TN_IAC; break;
        case 1: letter = TN_SB; break;
        case 2: letter = mccp1 ? OPT_COMPRESS : OPT_COMPRESS2; break;
        case 3: letter =  mccp1 ? TN_WILL : TN_IAC; break;
        case 4: letter = TN_SE; break;
      }

      if (*(stream.next_in) == letter)
      {
        stream.next_in++;
        stream.avail_in--;
        seqData[state] = letter;
        state++;
      }
      else
      {
        for (int i = 0; i < state; i++)
        {
          *(stream.next_out++) = seqData[i];
          stream.avail_out--;
          outSize++;
        }
        //IAC SB followed by s.t. else than COMPRESS(2)
        //enter some special state where we'll remain until IAC SE comes
        //we cannot be in the middle of that IAC SE, because IAC SB IAC SE
        //is not allowed
        if (state >= 2)
          state = 6;
        else
          state = 0;
        //IAC IAC ?? must not be interpreted as IAC followed by IAC ??,
        //so we don't do anything special if we're at second IAC...

        //current char must be added
        *(stream.next_out++) = *(stream.next_in++);
        stream.avail_out--;
        stream.avail_in--;
        outSize++;
      }
      if (state == 5) //If we are here, compressed stream should begin NOW.
      {               //we'll decompress on next run :)
        //initialize zlib - NO ERROR CHECKING!!!
        stream.total_out = 0;
        stream.total_in = 0;
        inflateInit (&stream);

        //return parsed uncompressed part
        instream = true;
        state = 0;
        return outSize;
      }
    }
    else
    {
      //waiting for IAC SE
      if ((state == 6) && (*(stream.next_in) == TN_IAC))
        state = 7;
      else
        if ((state == 7) && (*(stream.next_in) == TN_SE))
          state = 0;
        else
          state = 6;
      //current char must be added
      *(stream.next_out++) = *(stream.next_in++);
      stream.avail_out--;
      stream.avail_in--;
      outSize++;
    }
  }
  return outSize;
}
