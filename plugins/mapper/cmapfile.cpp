/***************************************************************************
                               cmapfile.cpp
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

#include "cmapfile.h"

CMapFile::CMapFile(QString filename, Mode mode)
{
  QIODevice::OpenModeFlag io_mode = (mode == Read) ? QIODevice::ReadOnly : QIODevice::WriteOnly;

	_tarFile = new KTar(filename);
	if (!_tarFile->open(io_mode))
	{
		delete _tarFile;
		_tarFile = NULL;
	}

	_mode = mode;
	_isOpen = false;
}

CMapFile::~CMapFile()
{
	if (_isOpen)
	{
		close();
	}

	if (_tarFile != NULL)
	{
		_tarFile->close();
		delete _tarFile;
	}
}

QBuffer* CMapFile::open(QString filename)
{
	if (_isOpen)
	{
		return NULL;
	}

	if (_mode == Write)
	{
		_isOpen = true;
		_byteArray.resize(0);
		_buffer = new QBuffer(&_byteArray);
		_buffer->open(QIODevice::WriteOnly);
	}
	else
	{
		const KArchiveEntry* entry = _tarFile->directory()->entry(filename);
		if (entry == NULL || entry->isDirectory())
		{
			return NULL;
		}
		else
		{
			KArchiveFile* file = (KArchiveFile*) entry;
			_byteArray = file->data();
			_buffer = new QBuffer(&_byteArray);
			_buffer->open(QIODevice::ReadOnly);
			_isOpen = true;
		}
	}
	_filename = filename;
	return _buffer;
}

void CMapFile::close()
{
	if (!_isOpen)
	{
		return;
	}

	if (_mode == Write)
	{
		_tarFile->writeFile(_filename, "user", "group", _buffer->buffer().data(), _buffer->size());
	}

	_isOpen = false;
	delete _buffer;
	_buffer = NULL;
}
