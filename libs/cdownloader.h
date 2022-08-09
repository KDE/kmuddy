/***************************************************************************
                          cdownloader.h  -  download stuff
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

#ifndef CDOWNLOADER_H
#define CDOWNLOADER_H

//Currently we only can use this object with MSP. Later, it may be used with
//other stuff (MXP) as well. Then this will have to be changed.

#include <qobject.h>
#include <kmuddy_export.h>

class cMSP;

//forward declaration for KIO::Job
namespace KIO { class Job; }

/**
Class used to download stuff.
  *@author Tomas Mecir
  */

//class definition must always exist - MOC depends on that
class KMUDDY_EXPORT cDownloader : public QObject {
   Q_OBJECT
public: 
  cDownloader (cMSP *mspObject);
  ~cDownloader () override;
  bool downloading () { return (currentJob != nullptr); };
  void reset ();
  void download (QString remoteURL, QString localFile);
protected slots:
  void slotResult (KIO::Job *job);
protected:
  cMSP *msp;
  KIO::Job *currentJob;
};

#endif  //CDOWNLOADER_H
