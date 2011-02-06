/***************************************************************************
                               cmapfile.h
                             -------------------
    begin                : Tue Mar 13 2001
    copyright            : (C) 2001 by Kmud Developer Team
    email                : kmud-devel@kmud.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CMAPFILE_H
#define CMAPFILE_H


/**
  *@author Kevin Krammer
  */

#include <qbuffer.h>
#include <ktar.h>
#include <kvbox.h>

class CMapFile {
public:
	enum Mode {Read, Write};

	/** trys to open the specified file (assumes it's a tar.gz archive) in the
	* specified mode (either read or write) */
	CMapFile(QString filename, Mode mode);
	~CMapFile();

	/** open a file within the mapfile archive. returns NULL
	*	if filename ws not found in the archive or the archive itself couldn't
	* be opend or a previous opened file was not closed */
	QBuffer* open(QString filename);

	/** closes the file opened by open(). Must be calles before a new file is opened*/
	void close();

protected: // Protected attributes
  /** the tar archive holding all files */
  KTar* _tarFile;
	bool _isOpen;
  QBuffer* _buffer;
  QByteArray _byteArray;
  /** the mode the tar archives was open with */
  Mode _mode;
  /** the name of the open file */
  QString _filename;
};

#endif
