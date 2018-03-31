/***************************************************************************
                          ctranscript.h  -  session transcript
    This file is a part of KMuddy distribution.
                             -------------------
    begin                : Ct oct 31 2002
    copyright            : (C) 2002 by Tomas Mecir
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

#ifndef CTRANSCRIPT_H
#define CTRANSCRIPT_H

#include <QObject>
#include <QFile>

#include "cactionbase.h"

class dlgTranscript;
class dlgDumpBuffer;
class QTimer;

/**
This class handles session transcript feature.
  *@author Tomas Mecir
  */

class cTranscript : public QObject, public cActionBase {
   Q_OBJECT
private:
  QString directory, name;
public: 
  cTranscript (int sess);
  ~cTranscript ();
  
  void load();
  void save();
  
  void addLineToTranscript (cTextChunk *chunk);
  void addLineToAdvTranscript (cTextChunk *chunk);
  
  void stopTranscript ();
  void stopAdvTranscript ();
  void startAdvTranscript ();
  void setEnableRotate (bool val) { rotatedaily = val; };
  bool enableRotate () { return rotatedaily; };
  void setPrependTimestamp (bool val) { includetimestamp = val; };
  bool prependTimestamp () { return includetimestamp; };
public slots:
  void configure ();
  void dumpBuffer ();
  void timeout ();
protected slots:
  void applyTranscript ();
  void doDumpBuffer ();
protected:
  virtual void eventNothingHandler (QString event, int session) override;
  virtual void eventChunkHandler (QString event, int session, cTextChunk *chunk) override;
  
  void startTranscript ();
  
  void setAFName (const QString &what);
  QString getAFName ();

  bool running, advrunning, rotatedaily;
  bool overwrite, includedump;
  bool includetimestamp;
  QFile file, advfile;
  QString fname, advfname, fileformat;
  QTimer *transtimer;

  //type of normal and advanced transcript
  int type, advtype;
  
  dlgTranscript *tdlg;
  dlgDumpBuffer *bdlg;
  
  bool nextLineGagged;
};

#endif
