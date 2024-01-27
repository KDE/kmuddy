/***************************************************************************
                          cdownloader.cpp  -  download stuff
                             -------------------
    begin                : Pi mar 28 2003
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

#include "cdownloader.h"

#include "cmsp.h"
#include <QDir>
#include <QUrl>

#include <kio/job.h>

cDownloader::cDownloader (cMSP *mspObject)
{
  msp = mspObject;
  currentJob = nullptr;
}

cDownloader::~cDownloader ()
{
  reset ();
}

void cDownloader::reset ()
{
  if (currentJob)
  {
    currentJob->kill ();
    currentJob = nullptr;
  }
}

void cDownloader::download (QString remoteURL, QString localFile)
{
  if (currentJob)
    return;
  
  //create all missing directories
  QString lastDir = localFile.section ("/", 0, -2, QString::SectionSkipEmpty);
  QDir().mkpath (lastDir);
  
  //download!!!
  QUrl url1 (remoteURL);
  QUrl url2;
  url2.setPath (localFile);
  currentJob = KIO::file_copy (url1, url2);
  connect (currentJob, &KJob::result, this, &cDownloader::slotResult);
}

void cDownloader::slotResult (KJob *job)
{
  //download complete or failed or something
  int error = job->error ();
  if (error == 0)
    msp->downloadCompleted ();
  else
  {
    QString reason = job->errorString ();
    msp->downloadFailed (reason);
  }
  currentJob = nullptr;
}

#include "moc_cdownloader.cpp"
