//----------------------------------------------------------------------------
//  EDGE IN THE DARK
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2021  The EDGE Team.
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//  Based on the FITD source code, under the GPLv2, (C) FITD AUTHORS
//
//----------------------------------------------------------------------------

// Dummy-class designed to be a workalike for OSystem-streams:

#include "filestream.h"

namespace Common 
{

ReadFileStream::ReadFileStream() 
{
	_fileHandle = NULL;
}
ReadFileStream::ReadFileStream(const char *filename) 
{
	_fileHandle = NULL;
	open(filename);
}

ReadFileStream::~ReadFileStream() 
{
	close();
}

bool ReadFileStream::open(const char *filename) 
{
	_fileHandle = (void*)fopen(filename, "rb");
	if(_fileHandle)
		return true;
	else
		return false;
}

uint32 ReadFileStream::read(byte *target, uint32 size) 
{
	return fread(target, 1, size, (FILE*)_fileHandle);
}

void ReadFileStream::seek(int32 offset, int whence) 
{
	fseek((FILE*)_fileHandle, offset, whence);
}

void ReadFileStream::close() 
{
	if (_fileHandle)
		fclose((FILE*)_fileHandle);
}

uint32 ReadFileStream::readUint32LE() 
{
	uint32 retval;
	read((byte *)&retval, 4);
	return READ_LE_UINT32(&retval);
}

uint16 ReadFileStream::readUint16LE() 
{
	uint16 retval;
	read((byte *)&retval, 2);
	return READ_LE_UINT16(&retval);
		
}
	
byte ReadFileStream::readByte() 
{
	byte retval;
	read(&retval, 1);

	return retval;
}
	
int32 ReadFileStream::size() 
{
	uint32 oldpos = ftell((FILE*)_fileHandle);
	fseek((FILE*)_fileHandle, 0, SEEK_END);
	uint32 retval = ftell((FILE*)_fileHandle);
	fseek((FILE*)_fileHandle, oldpos, SEEK_SET);

	return retval;
}

bool ReadFileStream::isOpen()
{
	return (_fileHandle ? true : false);
}

} // end of namespace Common


