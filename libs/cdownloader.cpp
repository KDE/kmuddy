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

#include <kio/job.h>
#include <kstandarddirs.h>
#include <kurl.h>

cDownloader::cDownloader (cMSP *mspObject)
{
  msp = mspObject;
  currentJob = 0;
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
    currentJob = 0;
  }
}

void cDownloader::download (QString remoteURL, QString localFile)
{
  if (currentJob)
    return;
  
  //create all missing directories
  QString lastDir = localFile.section ("/", 0, -2, QString::SectionSkipEmpty);
  KStandardDirs::makeDir (lastDir);
  
  //download!!!
  KUrl url1 (remoteURL);
  KUrl url2;
  url2.setPath (localFile);
  currentJob = KIO::file_copy (url1, url2);
  connect (currentJob, SIGNAL (result (KIO::Job *)), this,
        SLOT (slotResult (KIO::Job *)));
}

void cDownloader::slotResult (KIO::Job *job)
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
  currentJob = 0;
}

